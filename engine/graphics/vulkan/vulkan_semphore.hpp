#pragma once
#include "types.hpp"
#include "defines.hpp"
#include "vulkan_results.hpp"

class vulkan_semaphores {
public:
	vulkan_semaphores(VkDevice device);
	~vulkan_semaphores();

	VkSemaphore get();

private:
	VkDevice	device;
	VkSemaphore semaphore;
};