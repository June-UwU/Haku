#pragma once
#include "logger.hpp"
#include "defines.hpp"
#include "vulkan/vulkan.h"
#include "vulkan_device.hpp"

inline VkResult begin_command_buffer(VkCommandBuffer cmd, VkCommandBufferUsageFlags flags) {
	VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	begin_info.flags					= flags;
	VkResult result						= vkBeginCommandBuffer(cmd, &begin_info);
	if (VK_SUCCESS != result) {
		WARN << "failed to begin command buffer..!!\n";
		HAKU_FATAL_CRASH();
	}

	return result;
}

inline VkResult submit_command_buffer(VkCommandBuffer cmd, VkQueue queue) {
	VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers	  = &cmd;

	VkResult result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	if (VK_SUCCESS != result) {
		WARN << "failed to submit queue recording..!!\n";
		HAKU_FATAL_CRASH();
	}

	return result;
}

inline std::pair<VkPipelineStageFlags, VkPipelineStageFlags> resolve_image_barrier_stage_masks(
	VkImageLayout		  from,
	VkImageLayout		  to,
	VkImageMemoryBarrier& barrier) {
	std::pair<VkPipelineStageFlags, VkPipelineStageFlags> mask;

	if (from == VK_IMAGE_LAYOUT_UNDEFINED && to == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		mask.first	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		mask.second = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (from == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		mask.first	= VK_PIPELINE_STAGE_TRANSFER_BIT;
		mask.second = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		TRACE << "unsupported layout transition..!!\n";
	}

	return mask;
}

inline VkImageView create_image_view(std::shared_ptr<vulkan_device>& device, VkImage image, VkFormat format) {
	VkImageViewCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	create_info.image							= image;
	create_info.viewType						= VK_IMAGE_VIEW_TYPE_2D;
	create_info.format							= format;
	create_info.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	create_info.subresourceRange.baseMipLevel	= 0;
	create_info.subresourceRange.levelCount		= 1;
	create_info.subresourceRange.baseArrayLayer = 0;
	create_info.subresourceRange.layerCount		= 1;

	VkImageView view = VK_NULL_HANDLE;
	VkResult	result	  = vkCreateImageView(device->get_logical_device(), &create_info, nullptr, &view);
	if (VK_SUCCESS != result) {
		WARN << "failed to create image view..!!\n";
		return VK_NULL_HANDLE;
	}

	return view;
}