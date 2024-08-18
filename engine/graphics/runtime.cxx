#include "runtime.hpp"
#include "device.hpp"
#include "vertex.hpp"
#include "shader.hpp"
#include "file_systems/file.hpp"
#include "platform/window.hpp"
#include "vertex.hpp"
#include "memory.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <limits>
#include "memory.hpp"
#include <glm/gtc/matrix_transform.hpp>

VkBuffer	  buffer;
VmaAllocation buffer_memory;
VkBuffer	  index;
VmaAllocation index_memory;

typedef struct pipeline {
	VkDescriptorSetLayout discriptor_layout;
	VkPipeline			  pipeline;
	VkPipelineLayout	  layout;
} pipeline;

constexpr u32 MAX_IN_FLIGHT_FRAME = 3;

VkFormat				   swap_chain_format;
VkExtent2D				   swap_chain_extent;
VkSwapchainKHR			   swap_chain;
std::vector<VkImage>	   swap_chain_images;
std::vector<VkImageView>   swap_chain_image_views;
std::vector<VkFramebuffer> swapchain_frame_buffer;
VkRenderPass			   render_pass;
pipeline				   render_pipeline;

VkCommandPool				   command_pool;
std::vector<VkCommandBuffer>   command_buffer;
std::vector<VkSemaphore>	   image_available_sema;
std::vector<VkSemaphore>	   render_finish_sema;
std::vector<VkFence>		   in_flight_fence;
std::vector<VkBuffer>		   ubo_buffer;
std::vector<VmaAllocation>	   ubo_alloc;
std::vector<VmaAllocationInfo> ubo_alloc_info;
VkDescriptorPool			   descriptor_pool;
std::vector<VkDescriptorSet>   descriptor_set;

bool create_graphics_discriptor_set_layout() {
	VkDescriptorSetLayoutBinding ubo_binding{};
	ubo_binding.binding			= 0;
	ubo_binding.descriptorCount = 1;
	ubo_binding.descriptorType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo_binding.stageFlags		= VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layout_info{};
	layout_info.sType		 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = 1;
	layout_info.pBindings	 = &ubo_binding;

	auto result = vkCreateDescriptorSetLayout(get_device(), &layout_info, nullptr, &render_pipeline.discriptor_layout);
	ASSERT(VK_SUCCESS == result, "failed to create descriptor layout for graphics pipeline");

	return true;
}

void destroy_command_module() {
	for (u32 i = 0; i < MAX_IN_FLIGHT_FRAME; i++) {
		vkDestroySemaphore(get_device(), image_available_sema[i], nullptr);
		vkDestroySemaphore(get_device(), render_finish_sema[i], nullptr);
		vkDestroyFence(get_device(), in_flight_fence[i], nullptr);
	}
	vkDestroyCommandPool(get_device(), command_pool, nullptr);
}

bool create_sync_objects() {
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	image_available_sema.resize(MAX_IN_FLIGHT_FRAME);
	render_finish_sema.resize(MAX_IN_FLIGHT_FRAME);
	in_flight_fence.resize(MAX_IN_FLIGHT_FRAME);

	for (u32 i = 0; i < MAX_IN_FLIGHT_FRAME; i++) {
		auto img_avail = vkCreateSemaphore(get_device(), &semaphore_info, nullptr, &image_available_sema[i]);
		ASSERT(VK_SUCCESS == img_avail, "failed to create image available semaphore..!");

		auto render_done = vkCreateSemaphore(get_device(), &semaphore_info, nullptr, &render_finish_sema[i]);
		ASSERT(VK_SUCCESS == render_done, "failed to create render finish semaphore..!");

		auto in_flight = vkCreateFence(get_device(), &fence_info, nullptr, &in_flight_fence[i]);
		ASSERT(VK_SUCCESS == in_flight, "failed to create in flight fence..!");
	}

	return true;
}

bool create_command_buffer() {
	VkCommandBufferAllocateInfo alloc_info{};
	alloc_info.sType			  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool		  = command_pool;
	alloc_info.level			  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandBufferCount = 1;

	command_buffer.resize(MAX_IN_FLIGHT_FRAME);
	for (u32 i = 0; i < MAX_IN_FLIGHT_FRAME; i++) {
		auto result = vkAllocateCommandBuffers(get_device(), &alloc_info, &command_buffer[i]);
		ASSERT(VK_SUCCESS == result, "failed to allocate command buffers");
	}

	return true;
}

bool initialize_command_module() {
	gpu_queue queue_family = find_queue_families(get_physical_device());

	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType			   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.flags			   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = queue_family.graphics_index;

	auto result = vkCreateCommandPool(get_device(), &pool_info, nullptr, &command_pool);
	ASSERT(VK_SUCCESS == result, "failed to initialize command pool");

	bool command_buffer_status = create_command_buffer();
	ASSERT(command_buffer_status, "failed to initialize command buffer");

	bool sync_obj = create_sync_objects();
	ASSERT(sync_obj, "failed to create sync objects");

	return true;
}

bool create_frame_buffers() {
	swapchain_frame_buffer.resize(swap_chain_image_views.size());

	for (u32 i = 0; i < swap_chain_image_views.size(); i++) {
		VkImageView attachments[] = {
			swap_chain_image_views[i]
		};

		VkFramebufferCreateInfo frame_buffer_info{};
		frame_buffer_info.sType			  = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_buffer_info.renderPass	  = render_pass;
		frame_buffer_info.attachmentCount = 1;
		frame_buffer_info.pAttachments	  = attachments;
		frame_buffer_info.width			  = swap_chain_extent.width;
		frame_buffer_info.height		  = swap_chain_extent.height;
		frame_buffer_info.layers		  = 1;

		if (vkCreateFramebuffer(get_device(), &frame_buffer_info, nullptr, &swapchain_frame_buffer[i]) != VK_SUCCESS) {
			FATAL << "failed to create frame buffers !!\n ";

			return false;
		}
	}

	return true;
}

VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
	for (const auto& format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	return formats[0];
}

VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& modes) {
	// TODO : experiment with present modes
	for (const auto& mode : modes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return mode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extend(const VkSurfaceCapabilitiesKHR& capabilities) {
	constexpr u32 limit = std::numeric_limits<u32>::max();
	if (capabilities.currentExtent.width != limit) {
		return capabilities.currentExtent;
	}

	s32 width, height;
	glfwGetFramebufferSize(get_window(), &width, &height);

	VkExtent2D actual_extend = {
		static_cast<u32>(width),
		static_cast<u32>(height)
	};

	actual_extend.width	 = std::clamp(actual_extend.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actual_extend.height = std::clamp(actual_extend.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return actual_extend;
}

bool create_swap_chain_image_views() {
	const u32 buffer_count = swap_chain_images.size();
	swap_chain_image_views.resize(buffer_count);

	for (u32 i = 0; i < buffer_count; i++) {
		VkImageViewCreateInfo view_info{};
		view_info.sType							  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image							  = swap_chain_images[i];
		view_info.viewType						  = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format						  = swap_chain_format;
		view_info.components.r					  = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.g					  = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.b					  = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.a					  = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.subresourceRange.aspectMask	  = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel	  = 0;
		view_info.subresourceRange.levelCount	  = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount	  = 1;

		if (vkCreateImageView(get_device(), &view_info, nullptr, &swap_chain_image_views[i]) != VK_SUCCESS) {
			FATAL << "failed to create swapchain images views\n";
			return false;
		}
	}

	return true;
}

bool create_swap_chain() {
	swap_chain_support support = query_swapchain_support(get_physical_device());

	VkSurfaceFormatKHR format		= choose_swap_surface_format(support.formats);
	VkPresentModeKHR   present_mode = choose_swap_present_mode(support.present_modes);
	VkExtent2D		   extent		= choose_swap_extend(support.capabilities);

	u32 buffer_count = support.capabilities.minImageCount + 1;

	if (support.capabilities.maxImageCount > 0 && buffer_count > support.capabilities.maxImageCount) {
		buffer_count = support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapchain_info{};
	swapchain_info.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.surface			= get_render_surface();
	swapchain_info.minImageCount	= buffer_count;
	swapchain_info.imageFormat		= format.format;
	swapchain_info.imageColorSpace	= format.colorSpace;
	swapchain_info.imageExtent		= extent;
	swapchain_info.imageArrayLayers = 1;
	swapchain_info.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_info.compositeAlpha	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO : this setting needs to be looked into

	auto queue = find_queue_families(get_physical_device());
	u32	 families[]{
		 static_cast<u32>(queue.present_index),
		 static_cast<u32>(queue.graphics_index)
	};

	if (queue.present_index != queue.graphics_index) {
		swapchain_info.imageSharingMode		 = VK_SHARING_MODE_CONCURRENT;
		swapchain_info.queueFamilyIndexCount = 2;
		swapchain_info.pQueueFamilyIndices	 = families;
	} else {
		swapchain_info.imageSharingMode		 = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_info.queueFamilyIndexCount = 0;		// Optional
		swapchain_info.pQueueFamilyIndices	 = nullptr; // Optional
	}

	swapchain_info.preTransform = support.capabilities.currentTransform;
	swapchain_info.presentMode	= present_mode;
	swapchain_info.clipped		= VK_TRUE;
	swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(get_device(), &swapchain_info, nullptr, &swap_chain) != VK_SUCCESS) {
		FATAL << "failed to create swapchain!\n";
		return false;
	}

	vkGetSwapchainImagesKHR(get_device(), swap_chain, &buffer_count, nullptr);
	swap_chain_images.resize(buffer_count);
	vkGetSwapchainImagesKHR(get_device(), swap_chain, &buffer_count, swap_chain_images.data());

	swap_chain_format = format.format;
	swap_chain_extent = extent;
	return true;
}

bool create_render_pass() {
	VkAttachmentDescription color_attachment{};
	color_attachment.format			= swap_chain_format;
	color_attachment.samples		= VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp		= VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachment_reference{};
	attachment_reference.attachment = 0;
	attachment_reference.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint	 = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments	 = &attachment_reference;

	VkRenderPassCreateInfo render_pass_info{};
	render_pass_info.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments	 = &color_attachment;
	render_pass_info.subpassCount	 = 1;
	render_pass_info.pSubpasses		 = &subpass;

	TODO("clean up render pass !!");
	if (vkCreateRenderPass(get_device(), &render_pass_info, nullptr, &render_pass) != VK_SUCCESS) {
		WARN << "failed to create render pass !!\n";
		return false;
	}

	return true;
}

bool create_pipeline_layout() {
	VkPipelineLayoutCreateInfo create_info{};
	create_info.sType		   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	create_info.setLayoutCount = 1;
	create_info.pSetLayouts	   = &render_pipeline.discriptor_layout;

	ASSERT(VK_SUCCESS == vkCreatePipelineLayout(get_device(), &create_info, nullptr, &render_pipeline.layout), "failed to create render pipeline layout");
	return true;
}

void configure_dynamic_state(std::vector<VkDynamicState>& state, VkPipelineDynamicStateCreateInfo& conf) {
	conf.sType			   = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	conf.dynamicStateCount = state.size();
	conf.pDynamicStates	   = state.data();
}

void configure_input_assembly(VkPipelineInputAssemblyStateCreateInfo& state) {
	state.sType					 = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	state.topology				 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	state.primitiveRestartEnable = VK_FALSE;
}

void configure_view_and_scissor(VkPipelineViewportStateCreateInfo& state) {
	state.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	state.viewportCount = 1;
	state.scissorCount	= 1;
}

void configure_rasterizer_state(VkPipelineRasterizationStateCreateInfo& state) {
	state.sType					  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	state.depthClampEnable		  = VK_FALSE;
	state.rasterizerDiscardEnable = VK_FALSE;
	state.polygonMode			  = VK_POLYGON_MODE_FILL;
	state.lineWidth				  = 1.0f;
	state.cullMode				  = VK_CULL_MODE_BACK_BIT;
	state.frontFace				  = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	state.depthBiasEnable		  = VK_FALSE;
	state.depthBiasConstantFactor = 0.0f; // Optional
	state.depthBiasClamp		  = 0.0f; // Optional
	state.depthBiasSlopeFactor	  = 0.0f; // Optional
}

void configure_multiplesample_state(VkPipelineMultisampleStateCreateInfo& state) {
	state.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	state.sampleShadingEnable	= VK_FALSE;
	state.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
	state.minSampleShading		= 1.0f;		// Optional
	state.pSampleMask			= nullptr;	// Optional
	state.alphaToCoverageEnable = VK_FALSE; // Optional
	state.alphaToOneEnable		= VK_FALSE; // Optional
}

void configure_color_blend_attachment_state(VkPipelineColorBlendAttachmentState& state) {
	state.colorWriteMask	  = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	state.blendEnable		  = VK_TRUE;
	state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	state.colorBlendOp		  = VK_BLEND_OP_ADD;
	state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	state.alphaBlendOp		  = VK_BLEND_OP_ADD;
}

void configure_color_blend_state(VkPipelineColorBlendStateCreateInfo& state, const VkPipelineColorBlendAttachmentState& attachment) {
	state.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	state.logicOpEnable		= VK_FALSE;
	state.logicOp			= VK_LOGIC_OP_COPY; // Optional
	state.attachmentCount	= 1;
	state.pAttachments		= &attachment;
	state.blendConstants[0] = 0.0f; // Optional
	state.blendConstants[1] = 0.0f; // Optional
	state.blendConstants[2] = 0.0f; // Optional
	state.blendConstants[3] = 0.0f; // Optional
}

std::vector<VkPipelineShaderStageCreateInfo> configure_shader_info() {
	std::string	   shader_dir		= get_application_directory();
	auto		   vertex_byte_code = read_binary(shader_dir + "vertex.spv");
	VkShaderModule vertex_shader	= create_shader_module(vertex_byte_code);

	auto		   fragment_byte_code = read_binary(shader_dir + "frag.spv");
	VkShaderModule fragment_shader	  = create_shader_module(fragment_byte_code);

	VkPipelineShaderStageCreateInfo vertex_info{};
	vertex_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertex_info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
	vertex_info.module = vertex_shader;
	vertex_info.pName  = "main";

	VkPipelineShaderStageCreateInfo fragment_info{};
	fragment_info.sType	 = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragment_info.stage	 = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragment_info.module = fragment_shader;
	fragment_info.pName	 = "main";

	return { vertex_info, fragment_info };
}

bool create_rendering_pipeline() {
	bool desc_layout = create_graphics_discriptor_set_layout();
	ASSERT(desc_layout, "failed to create graphic descriptor layout");

	bool layout_creation = create_pipeline_layout();
	ASSERT(layout_creation, "failed to create rendering pipeline layout")

	std::vector<VkDynamicState> dynamic_components = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state{};
	configure_dynamic_state(dynamic_components, dynamic_state);

	auto								 binding_description   = vertex::get_vertex_binding_description();
	auto								 attribute_description = vertex::get_attribute_description();
	VkPipelineVertexInputStateCreateInfo vertex_input_state{};
	vertex_input_state.sType						   = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state.vertexBindingDescriptionCount   = 1;
	vertex_input_state.pVertexBindingDescriptions	   = &binding_description; // Optional
	vertex_input_state.vertexAttributeDescriptionCount = attribute_description.size();
	vertex_input_state.pVertexAttributeDescriptions	   = attribute_description.data(); // Optional

	VkPipelineInputAssemblyStateCreateInfo input_assembly{};
	configure_input_assembly(input_assembly);

	VkPipelineViewportStateCreateInfo view_port_state{};
	configure_view_and_scissor(view_port_state);

	VkPipelineRasterizationStateCreateInfo rasterize_state{};
	configure_rasterizer_state(rasterize_state);

	VkPipelineMultisampleStateCreateInfo multisample_state{};
	configure_multiplesample_state(multisample_state);

	VkPipelineColorBlendAttachmentState color_blending_attachment_state{};
	configure_color_blend_attachment_state(color_blending_attachment_state);

	VkPipelineColorBlendStateCreateInfo color_blending_state{};
	configure_color_blend_state(color_blending_state, color_blending_attachment_state);

	std::vector<VkPipelineShaderStageCreateInfo> shader_info = configure_shader_info();

	VkGraphicsPipelineCreateInfo pipeline_info{};
	pipeline_info.sType				  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount		  = shader_info.size();
	pipeline_info.pStages			  = shader_info.data();
	pipeline_info.pVertexInputState	  = &vertex_input_state;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState	  = &view_port_state;
	pipeline_info.pRasterizationState = &rasterize_state;
	pipeline_info.pMultisampleState	  = &multisample_state;
	pipeline_info.pDepthStencilState  = nullptr; // Optional
	pipeline_info.pColorBlendState	  = &color_blending_state;
	pipeline_info.pDynamicState		  = &dynamic_state;
	pipeline_info.layout			  = render_pipeline.layout;
	pipeline_info.renderPass		  = render_pass;
	pipeline_info.subpass			  = 0;
	pipeline_info.basePipelineHandle  = VK_NULL_HANDLE; // Optional
	pipeline_info.basePipelineIndex	  = -1;				// Optional

	ASSERT(VK_SUCCESS == vkCreateGraphicsPipelines(get_device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &render_pipeline.pipeline), "failed to create rendering pipeline")

	for (auto shader_stages : shader_info) {
		destroy_shader_module(shader_stages.module);
	}

	return true;
}

bool initialize_renderer() {
	bool swap_chain_initialized = create_swap_chain();
	ASSERT(swap_chain_initialized, "failed to accquire swapchain\n");

	bool swap_chain_image_views = create_swap_chain_image_views();
	ASSERT(swap_chain_image_views, "failed to initialize swapchain image views\n");

	bool render_pass_create = create_render_pass();
	ASSERT(render_pass_create, "failed to create render pass\n");

	bool rendering_pipeline = create_rendering_pipeline();
	ASSERT(rendering_pipeline, "failed to create a graphics(rendering) pipeline");

	bool frame_buffer = create_frame_buffers();
	ASSERT(frame_buffer, "failed to create rendering frame buffers");

	return true;
}

void destroy_renderer() {
	for (auto frame_buffer : swapchain_frame_buffer) {
		vkDestroyFramebuffer(get_device(), frame_buffer, nullptr);
	}

	vkDestroyPipeline(get_device(), render_pipeline.pipeline, nullptr);
	vkDestroyPipelineLayout(get_device(), render_pipeline.layout, nullptr);
	vkDestroyDescriptorSetLayout(get_device(), render_pipeline.discriptor_layout, nullptr);

	vkDestroyRenderPass(get_device(), render_pass, nullptr);
	for (auto views : swap_chain_image_views) {
		vkDestroyImageView(get_device(), views, nullptr);
	}

	vkDestroySwapchainKHR(get_device(), swap_chain, nullptr);
}

bool initialize_global_ubos() {
	VkDeviceSize size = sizeof(mvp_ubo);

	ubo_buffer.resize(MAX_IN_FLIGHT_FRAME);
	ubo_alloc.resize(MAX_IN_FLIGHT_FRAME);
	ubo_alloc_info.resize(MAX_IN_FLIGHT_FRAME);

	VkBufferCreateInfo ubo_info{};
	ubo_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	ubo_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	ubo_info.size  = size;

	VmaAllocationCreateInfo alloc_create_info{};
	alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;
	alloc_create_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

	for (u32 i = 0; i < MAX_IN_FLIGHT_FRAME; i++) {
		create_buffer(&ubo_info, &alloc_create_info, &ubo_buffer[i], &ubo_alloc[i], &ubo_alloc_info[i]);
	}

	return true;
}

bool initialize_descriptor_pool() {
	VkDescriptorPoolSize pool_size;
	pool_size.type			  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size.descriptorCount = static_cast<u32>(MAX_IN_FLIGHT_FRAME);

	VkDescriptorPoolCreateInfo pool_create_info{};
	pool_create_info.sType		   = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_create_info.poolSizeCount = 1;
	pool_create_info.pPoolSizes	   = &pool_size;
	pool_create_info.maxSets	   = static_cast<u32>(MAX_IN_FLIGHT_FRAME);

	auto result = vkCreateDescriptorPool(get_device(), &pool_create_info, nullptr, &descriptor_pool);
	ASSERT(VK_SUCCESS == result, "failed to create descriptor pool");

	return true;
}

bool initialize_descriptor_sets() {
	std::vector<VkDescriptorSetLayout> layouts(MAX_IN_FLIGHT_FRAME, render_pipeline.discriptor_layout);
	VkDescriptorSetAllocateInfo		   alloc_info{};
	alloc_info.sType			  = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool	  = descriptor_pool;
	alloc_info.descriptorSetCount = static_cast<uint32_t>(MAX_IN_FLIGHT_FRAME);
	alloc_info.pSetLayouts		  = layouts.data();

	descriptor_set.resize(MAX_IN_FLIGHT_FRAME);
	auto result = vkAllocateDescriptorSets(get_device(), &alloc_info, descriptor_set.data());
	ASSERT(VK_SUCCESS == result, "failed to allocate descriptor set");
}

bool initialize_runtime() {
	bool renderer_initialized = initialize_renderer();
	ASSERT(renderer_initialized, "failed to initialize renderer");

	bool command_initialized = initialize_command_module();
	ASSERT(command_initialized, "failed to initialize command");

	bool mvp_initialized = initialize_global_ubos();
	ASSERT(mvp_initialized, "failed to create global ubos");

	bool descriptor_pool = initialize_descriptor_pool();
	ASSERT(mvp_initialized, "failed to create global ubos");

	bool descriptor_set_init = initialize_descriptor_sets();
	ASSERT(descriptor_set_init, "failed to create descriptor sets");

	for (u32 i = 0; i < MAX_IN_FLIGHT_FRAME; i++) {
		VkDescriptorBufferInfo desc_info{};
		desc_info.buffer = ubo_buffer[i];
		desc_info.offset = 0;
		desc_info.range	 = sizeof(mvp_ubo);

		VkWriteDescriptorSet write_conf{};
		write_conf.sType		   = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_conf.dstSet		   = descriptor_set[i];
		write_conf.dstBinding	   = 0;
		write_conf.dstArrayElement = 0;
		write_conf.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_conf.descriptorCount = 1;
		write_conf.pBufferInfo	   = &desc_info;

		vkUpdateDescriptorSets(get_device(), 1, &write_conf, 0, nullptr);
	}

	TODO("this is just fluff, we will remove this later");
	const u32		   size = test_data.size() * sizeof(vertex);
	VkBufferCreateInfo buffer_info{};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size  = size;
	buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	VmaAllocationCreateInfo alloc_info{};
	alloc_info.usage	= VMA_MEMORY_USAGE_AUTO;
	alloc_info.flags	= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	alloc_info.priority = 1.0f;

	create_buffer(&buffer_info, &alloc_info, &buffer, &buffer_memory, nullptr);
	buffer_info.size  = indices.size() * sizeof(u16);
	buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	create_buffer(&buffer_info, &alloc_info, &index, &index_memory, nullptr);

	VkBuffer		  staging;
	VmaAllocation	  staging_mem;
	VmaAllocationInfo staging_mem_info;

	VkBuffer		  index_staging;
	VmaAllocation	  index_staging_mem;
	VmaAllocationInfo index_staging_mem_info;

	buffer_info.size  = size;
	buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	alloc_info.flags  = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
					   VMA_ALLOCATION_CREATE_MAPPED_BIT;
	create_buffer(&buffer_info, &alloc_info, &staging, &staging_mem, &staging_mem_info);
	memcpy(staging_mem_info.pMappedData, test_data.data(), size);

	buffer_info.size = indices.size() * sizeof(u16);

	create_buffer(&buffer_info, &alloc_info, &index_staging, &index_staging_mem, &index_staging_mem_info);
	memcpy(index_staging_mem_info.pMappedData, indices.data(), indices.size() * sizeof(u16));

	VkCommandBufferBeginInfo transfer{};
	transfer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	auto result	   = vkBeginCommandBuffer(command_buffer[0], &transfer);
	ASSERT(VK_SUCCESS == result, "failed to begin transfer queue");

	VkBufferCopy region{};
	region.srcOffset = 0;
	region.dstOffset = 0;
	region.size		 = size;

	vkCmdCopyBuffer(command_buffer[0], staging, buffer, 1, &region);

	region.size = indices.size() * sizeof(u16);

	vkCmdCopyBuffer(command_buffer[0], index_staging, index, 1, &region);

	result = vkEndCommandBuffer(command_buffer[0]);
	ASSERT(VK_SUCCESS == result, "failed to end transfer queue");

	VkSubmitInfo submit_info{};
	submit_info.sType			   = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers	   = &command_buffer[0];

	result = vkQueueSubmit(get_present_queue(), 1, &submit_info, VK_NULL_HANDLE);
	ASSERT(result == VK_SUCCESS, "failed to submit transfer queue work");
	vkQueueWaitIdle(get_present_queue());

	return true;
}

void destroy_ubo_memory() {
	for (auto val : ubo_alloc) {
		free_gpu_memory(val);
	}
}

void destroy_descriptor_pool() {
	vkDestroyDescriptorPool(get_device(), descriptor_pool, nullptr);
}

void destroy_runtime() {
	destroy_descriptor_pool();
	destroy_ubo_memory();
	destroy_command_module();
	destroy_renderer();
}

u32 accquire_image(const u64 frame) {
	auto normalized_frame = frame % MAX_IN_FLIGHT_FRAME;

	constexpr const u64 infinity = std::numeric_limits<u64>::max();
	vkWaitForFences(get_device(), 1, &in_flight_fence[normalized_frame], VK_TRUE, infinity);
	vkResetFences(get_device(), 1, &in_flight_fence[normalized_frame]);

	u32 image = 0;
	vkAcquireNextImageKHR(get_device(), swap_chain, infinity, image_available_sema[normalized_frame], VK_NULL_HANDLE, &image);
	vkResetCommandBuffer(command_buffer[normalized_frame], 0);

	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	auto command_begun = vkBeginCommandBuffer(command_buffer[normalized_frame], &begin_info);
	ASSERT(VK_SUCCESS == command_begun, "failed to begin command buffer");

	VkRenderPassBeginInfo renderpass_info{};
	renderpass_info.sType			  = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpass_info.renderPass		  = render_pass;
	renderpass_info.framebuffer		  = swapchain_frame_buffer[image];
	renderpass_info.renderArea.offset = { 0, 0 };
	renderpass_info.renderArea.extent = swap_chain_extent;

	VkClearValue clear_color		= { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
	renderpass_info.clearValueCount = 1;
	renderpass_info.pClearValues	= &clear_color;

	vkCmdBeginRenderPass(command_buffer[normalized_frame], &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

	return image;
}

void submit_image(const u64 frame, const u32 image) {
	u64 normalized_frame = frame & MAX_IN_FLIGHT_FRAME;

	vkCmdEndRenderPass(command_buffer[normalized_frame]);

	auto end_command_buffer = vkEndCommandBuffer(command_buffer[normalized_frame]);
	ASSERT(VK_SUCCESS == end_command_buffer, "failed to record command buffer");

	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore			 wait_semphores[] = { image_available_sema[normalized_frame] };
	VkPipelineStageFlags waitStages[]	  = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount		  = 1;
	submit_info.pWaitSemaphores			  = wait_semphores;
	submit_info.pWaitDstStageMask		  = waitStages;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers	   = &command_buffer[normalized_frame];

	VkSemaphore signal_semaphores[]	 = { render_finish_sema[normalized_frame] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores	 = signal_semaphores;

	auto queue_submit = vkQueueSubmit(get_graphics_queue(), 1, &submit_info, in_flight_fence[normalized_frame]);
	ASSERT(VK_SUCCESS == queue_submit, "failed to submit to graphics queue");
}

void present_image(const u64 frame, const u32 image) {
	u64 normalized_frame = frame % MAX_IN_FLIGHT_FRAME;

	VkSemaphore signal_semaphores[] = { render_finish_sema[normalized_frame] };

	VkPresentInfoKHR present_info{};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores	= signal_semaphores;

	VkSwapchainKHR swapchains[] = { swap_chain };
	present_info.swapchainCount = 1;
	present_info.pSwapchains	= swapchains;

	present_info.pImageIndices = &image;

	vkQueuePresentKHR(get_present_queue(), &present_info);
}

void record_image(const u64 frame, const u32 image) {
	u64 normalized_frame = frame % MAX_IN_FLIGHT_FRAME;

	//TODO("remove this in the future");
	vkCmdBindPipeline(command_buffer[normalized_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, render_pipeline.pipeline);
	mvp_ubo ubo{};
	ubo.model			 = glm::rotate(glm::mat4(1.0f), frame * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view			 = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.projection		 = glm::perspective(glm::radians(45.0f), swap_chain_extent.width / (float)swap_chain_extent.height, 0.1f, 10.0f);
	ubo.projection[1][1] = -1;
	memcpy(ubo_alloc_info[normalized_frame].pMappedData, &ubo, sizeof(mvp_ubo));

	VkViewport viewport{};
	viewport.x		  = 0.0f;
	viewport.y		  = 0.0f;
	viewport.width	  = static_cast<float>(swap_chain_extent.width);
	viewport.height	  = static_cast<float>(swap_chain_extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(command_buffer[normalized_frame], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swap_chain_extent;
	vkCmdSetScissor(command_buffer[normalized_frame], 0, 1, &scissor);

	VkBuffer	 vertex_buffer[] = { buffer };
	VkDeviceSize offsets[]		 = { 0 };
	vkCmdBindVertexBuffers(command_buffer[normalized_frame], 0, 1, vertex_buffer, offsets);
	vkCmdBindDescriptorSets(command_buffer[normalized_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, render_pipeline.layout, 0, 1, &descriptor_set[normalized_frame], 0, nullptr);
	vkCmdBindIndexBuffer(command_buffer[normalized_frame], index, 0, VK_INDEX_TYPE_UINT16);

	vkCmdDrawIndexed(command_buffer[normalized_frame], indices.size(), 1, 0, 0, 0);
}
