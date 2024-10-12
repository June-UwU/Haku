#include "vulkan_vertex_data.hpp"
#include "vulkan_pipeline.hpp"
#include "file_systems/file.hpp"

vulkan_shader::vulkan_shader(std::shared_ptr<vulkan_device>& device, std::string path, VkShaderStageFlagBits stage)
	: device(device)
	, stage(stage) {
	byte_code = read_binary(path);

	VkShaderModuleCreateInfo create_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	create_info.codeSize = byte_code.size();
	create_info.pCode	 = reinterpret_cast<u32*>(byte_code.data());

	VkResult result = vkCreateShaderModule(device->get_logical_device(), &create_info, nullptr, &shader);
	VK_ASSERT(result, "failed to create shader module..!!");
}

vulkan_shader::~vulkan_shader() {
	vkDestroyShaderModule(device->get_logical_device(), shader, nullptr);
}

VkPipelineShaderStageCreateInfo vulkan_shader::get_shader_info() {
	VkPipelineShaderStageCreateInfo info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	info.pNext	= nullptr;
	info.flags	= 0;
	info.stage	= stage;
	info.module = shader;
	info.pName	= __HAKU_SHADER_ENTRY__;

	return info;
}

vulkan_pipeline::vulkan_pipeline(
	std::shared_ptr<vulkan_device>&				device,
	std::shared_ptr<vulkan_renderpass>&			renderpass,
	u32											width,
	u32											height,
	std::vector<std::shared_ptr<vulkan_shader>> shaders)
	: device(device)
	, renderpass(renderpass) {
	std::vector<VkDynamicState> dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	std::vector<VkPipelineShaderStageCreateInfo> shader_info;
	for (auto val : shaders) {
		shader_info.push_back(val->get_shader_info());
	}

	VkPipelineDynamicStateCreateInfo dynamic_state{};
	dynamic_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
	dynamic_state.pDynamicStates	= dynamic_states.data();

	auto bindings  = get_binding_description();
	auto attribute = get_attribute_descriptions();

	VkPipelineVertexInputStateCreateInfo vertex_input_info{};
	vertex_input_info.sType							  = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount	  = 1;
	vertex_input_info.pVertexBindingDescriptions	  = &bindings;
	vertex_input_info.vertexAttributeDescriptionCount = attribute.size();
	vertex_input_info.pVertexAttributeDescriptions	  = attribute.data();

	VkPipelineInputAssemblyStateCreateInfo input_assembly{};
	input_assembly.sType				  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology				  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x		  = 0.0f;
	viewport.y		  = 0.0f;
	viewport.width	  = (float)width;
	viewport.height	  = (float)height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset		  = { 0, 0 };
	scissor.extent.width  = width;
	scissor.extent.height = height;

	VkPipelineViewportStateCreateInfo viewport_state{};
	viewport_state.sType		 = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state.viewportCount = 1;
	viewport_state.pViewports	 = &viewport;
	viewport_state.scissorCount	 = 1;
	viewport_state.pScissors	 = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType				   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable		   = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode			   = VK_POLYGON_MODE_FILL;
	rasterizer.depthBiasEnable		   = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp		   = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor	   = 0.0f; // Optional
	rasterizer.lineWidth			   = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable	= VK_FALSE;
	multisampling.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading		= 1.0f;		// Optional
	multisampling.pSampleMask			= nullptr;	// Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable		= VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState color_blend_attachment{};
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable	  = VK_FALSE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	color_blend_attachment.colorBlendOp		   = VK_BLEND_OP_ADD;	   // Optional
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	color_blend_attachment.alphaBlendOp		   = VK_BLEND_OP_ADD;	   // Optional

	VkPipelineColorBlendStateCreateInfo color_blending{};
	color_blending.sType			 = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending.logicOpEnable	 = VK_FALSE;
	color_blending.logicOp			 = VK_LOGIC_OP_COPY; // Optional
	color_blending.attachmentCount	 = 1;
	color_blending.pAttachments		 = &color_blend_attachment;
	color_blending.blendConstants[0] = 0.0f; // Optional
	color_blending.blendConstants[1] = 0.0f; // Optional
	color_blending.blendConstants[2] = 0.0f; // Optional
	color_blending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount			= 0;	   // Optional
	pipeline_layout_info.pSetLayouts			= nullptr; // Optional
	pipeline_layout_info.pushConstantRangeCount = 0;	   // Optional
	pipeline_layout_info.pPushConstantRanges	= nullptr; // Optional

	VkResult result = vkCreatePipelineLayout(device->get_logical_device(), &pipeline_layout_info, nullptr, &layout);
	VK_ASSERT(result, "failed to create pipeline layout!");

	VkGraphicsPipelineCreateInfo create_info{};
	create_info.sType				= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	create_info.stageCount			= shader_info.size();
	create_info.pStages				= shader_info.data();
	create_info.pVertexInputState	= &vertex_input_info;
	create_info.pInputAssemblyState = &input_assembly;
	create_info.pViewportState		= &viewport_state;
	create_info.pRasterizationState = &rasterizer;
	create_info.pMultisampleState	= &multisampling;
	create_info.pDepthStencilState	= nullptr; // Optional
	create_info.pColorBlendState	= &color_blending;
	create_info.pDynamicState		= &dynamic_state;
	create_info.layout				= layout;
	create_info.renderPass			= renderpass->get();
	create_info.subpass				= 0;
	create_info.basePipelineHandle	= VK_NULL_HANDLE; // Optional
	create_info.basePipelineIndex	= -1;			  // Optional

	result = vkCreateGraphicsPipelines(device->get_logical_device(), VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline);
	VK_ASSERT(result, "pipeline creation failed..!!");
}

vulkan_pipeline::~vulkan_pipeline() {
	vkDestroyPipeline(device->get_logical_device(), pipeline, nullptr);
	vkDestroyPipelineLayout(device->get_logical_device(), layout, nullptr);
}

void vulkan_pipeline::bind(VkCommandBuffer cmd_buffer) {
	vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}
