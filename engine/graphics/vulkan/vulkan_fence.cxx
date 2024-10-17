#include "vulkan_fence.hpp"
#include <limits>

vulkan_fence::vulkan_fence(VkDevice device, bool make_signaled)
	: device(device) {
	fence = VK_NULL_HANDLE;
	ASSERT(VK_NULL_HANDLE != device, "can't create fence with a null device");

	VkFenceCreateInfo create_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	create_info.pNext = nullptr;
	create_info.flags = (true == make_signaled) ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VkResult fence_created = vkCreateFence(device, &create_info, nullptr, &fence);
	VK_ASSERT(fence_created, "failed to create fence...!!");
}

vulkan_fence::~vulkan_fence() {
	if (VK_NULL_HANDLE == fence) {
		return;
	}
	vkDestroyFence(device, fence, nullptr);
}

void vulkan_fence::reset() {
	VkResult reset_result = vkResetFences(device, 1, &fence);
	VK_ASSERT(reset_result, "failed to reset fence...!!");
}

void vulkan_fence::wait() {
	VkResult wait_status = vkWaitForFences(device, 1, &fence, VK_TRUE, std::numeric_limits<u64>::max());
	VK_ASSERT(wait_status, "failed to wait on fence...!!");
}

void vulkan_fence::wait_time_out(u64 ms) {
	VkResult wait_status = vkWaitForFences(device, 1, &fence, VK_TRUE, std::numeric_limits<u64>::max());
	VK_ASSERT(wait_status, "failed to wait on fence...!!");
}

VkFence vulkan_fence::get() {
	return fence;
}
