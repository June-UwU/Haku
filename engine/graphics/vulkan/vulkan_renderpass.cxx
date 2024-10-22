#include "vulkan_renderpass.hpp"

vulkan_renderpass::vulkan_renderpass(std::shared_ptr<vulkan_device> device, VkRenderPassCreateInfo& create_info)
	: device(device) {
	TRACE << "creating renderpass..!!\n";
	VkResult result = vkCreateRenderPass(device->get_logical_device(), &create_info, nullptr, &renderpass);
	VK_ASSERT(result, "failed to create renderpass");
}

vulkan_renderpass::~vulkan_renderpass() {
	vkDestroyRenderPass(device->get_logical_device(), renderpass, nullptr);
}

VkRenderPass vulkan_renderpass::get() {
	return renderpass;
}

VkSubpassDescription make_subpass(
	VkPipelineBindPoint					bind_point,
	std::vector<VkAttachmentReference>& input_attachments,
	std::vector<VkAttachmentReference>& color_attachments,
	std::vector<VkAttachmentReference>& resolve_attachments,
	VkAttachmentReference*				depth_attachment,
	std::vector<u32>&					preserve_list) {
	VkSubpassDescription subpass;
	subpass.flags					= 0;
	subpass.pipelineBindPoint		= bind_point;
	subpass.inputAttachmentCount	= input_attachments.size();
	subpass.pInputAttachments		= input_attachments.data();
	subpass.colorAttachmentCount	= color_attachments.size();
	subpass.pColorAttachments		= color_attachments.data();
	subpass.pResolveAttachments		= resolve_attachments.data();
	subpass.pDepthStencilAttachment = depth_attachment;
	subpass.preserveAttachmentCount = preserve_list.size();
	subpass.pPreserveAttachments	= preserve_list.data();

	return subpass;
}

VkRenderPassCreateInfo make_renderpass_info(
	std::vector<VkAttachmentDescription>& attachments,
	std::vector<VkSubpassDescription>&	  subpasses,
	std::vector<VkSubpassDependency>&	  dependency) {
	VkRenderPassCreateInfo renderpass_info;

	renderpass_info.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass_info.pNext			= nullptr;
	renderpass_info.flags			= 0;
	renderpass_info.attachmentCount = attachments.size();
	renderpass_info.pAttachments	= attachments.data();
	renderpass_info.subpassCount	= subpasses.size();
	renderpass_info.pSubpasses		= subpasses.data();
	renderpass_info.dependencyCount = dependency.size();
	renderpass_info.pDependencies	= dependency.data();

	return renderpass_info;
}
