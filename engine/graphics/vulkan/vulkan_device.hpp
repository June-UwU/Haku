#pragma once
#include "types.hpp"
#include "vulkan/vulkan.h"
#include "vulkan_command_allocator.hpp"

class vulkan_device {
public:
	vulkan_device(VkInstance instance);
	~vulkan_device();

	VkDevice get_logical_device();
	void	 wait_till_idle();

private:
	void set_queue_index();
	void initialize_device_queues();

private:
	u32 graphics_queue_index = static_cast<u32>(-1);
	u32 transfer_queue_index = static_cast<u32>(-1);
	u32 compute_queue_index	 = static_cast<u32>(-1);

	std::unique_ptr<vulkan_command_allocator> transfer_allocator;
	std::unique_ptr<vulkan_command_allocator> primary_allocator;

	VkQueue			 graphics_queue;
	VkQueue			 compute_queue;
	VkQueue			 transfer_queue;
	VkPhysicalDevice physical_device;
	VkDevice		 logical_device;
};