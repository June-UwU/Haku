#pragma once
#include "types.hpp"
#include "vulkan_device.hpp"

VkSubpassDescription make_subpass(
	VkPipelineBindPoint					bind_point,
	std::vector<VkAttachmentReference>& input_attachments,
	std::vector<VkAttachmentReference>& color_attachments,
	std::vector<VkAttachmentReference>& resolve_attachments,
	VkAttachmentReference*				depth_attachment,
	std::vector<u32>&					preserve_list);

VkRenderPassCreateInfo make_renderpass_info(
	std::vector<VkAttachmentDescription>& attachments,
	std::vector<VkSubpassDescription>&	  subpasses,
	std::vector<VkSubpassDependency>&	  dependency);

class vulkan_renderpass {
public:
	vulkan_renderpass(std::shared_ptr<vulkan_device> device, VkRenderPassCreateInfo& create_info);
	~vulkan_renderpass();

	VkRenderPass get();

private:
	std::shared_ptr<vulkan_device> device;
	VkRenderPass				   renderpass;
};