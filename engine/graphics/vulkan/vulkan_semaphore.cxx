#include "vulkan_semphore.hpp"

vulkan_semaphores::vulkan_semaphores(VkDevice device)
	: device(device) {
	VkSemaphoreCreateInfo create_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	create_info.pNext = nullptr;
	create_info.flags = 0;

	VkResult result = vkCreateSemaphore(device, &create_info, nullptr, &semaphore);
	VK_ASSERT(result, "failed to create semaphore..!!");
}

vulkan_semaphores::~vulkan_semaphores() {
	vkDestroySemaphore(device, semaphore, nullptr);
}

VkSemaphore vulkan_semaphores::get() {
	return semaphore;
}