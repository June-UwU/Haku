#pragma once
#include "types.hpp"
#include "defines.hpp"
#include "vulkan_results.hpp"

class vulkan_command_allocator {
public:
	vulkan_command_allocator(VkDevice device, u32 family_index, bool is_transient);
	~vulkan_command_allocator();

	VkCommandBuffer allocate_command_buffer(bool is_primary);
	void			free_command_buffer(VkCommandBuffer buffer);
	void			reclaim_allocator();

private:
	VkDevice	  device;
	VkCommandPool command_pool;
};