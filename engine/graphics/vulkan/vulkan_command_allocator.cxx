#include "vulkan_command_allocator.hpp"
#include <vulkan/vulkan.h>

vulkan_command_allocator::vulkan_command_allocator(VkDevice device, u32 family_index, bool is_transient)
	: device(device) {
	VkCommandPoolCreateInfo create_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	create_info.pNext			 = nullptr;
	create_info.flags			 = (true == is_transient) ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	create_info.queueFamilyIndex = family_index;

	VkResult command_pool_created = vkCreateCommandPool(device, &create_info, nullptr, &command_pool);
	VK_ASSERT(command_pool_created, "failed to create command pool");
}

vulkan_command_allocator::~vulkan_command_allocator() {
	vkDestroyCommandPool(device, command_pool, nullptr);
}

VkCommandBuffer vulkan_command_allocator::allocate_command_buffer(bool is_primary) {
	VkCommandBufferAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	alloc_info.pNext			  = nullptr;
	alloc_info.commandPool		  = command_pool;
	alloc_info.level			  = (true == is_primary) ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	VkResult		buffer_allocated = vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);
	VK_ASSERT(buffer_allocated, "failed to allocate command buffer..!!");

	return command_buffer;
}

void vulkan_command_allocator::free_command_buffer(VkCommandBuffer buffer) {
	vkFreeCommandBuffers(device, command_pool, 1, &buffer);
}

void vulkan_command_allocator::reclaim_allocator() {
	VkResult reset_status = vkResetCommandPool(device, command_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
	VK_ASSERT(reset_status, "failed to reset command pool");
}
