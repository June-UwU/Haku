#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#define VMA_STATIC_VULKAN_FUNCTIONS	 1
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"
#include <vector>

class gpu_memory_allocator {
public:
	gpu_memory_allocator(VkPhysicalDevice gpu, VkDevice device, VkInstance instance);
	~gpu_memory_allocator();

	gpu_memory_allocator(const gpu_memory_allocator& alloc)			  = delete;
	gpu_memory_allocator operator=(const gpu_memory_allocator& alloc) = delete;

	gpu_memory_allocator(gpu_memory_allocator&& alloc)			  = delete;
	gpu_memory_allocator& operator=(gpu_memory_allocator&& alloc) = delete;
	
	u32		 get_size(VmaAllocation memory);
	VkResult create_image(
		VkImage*				 out_image,
		VmaAllocation*			 out_memory,
		VkDevice				 device,
		VkImageCreateInfo&		 image_info,
		VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
	VkResult create_buffer(
		VkBuffer*				 out_image,
		VmaAllocation*			 out_memory,
		VkDevice				 device,
		VkBufferCreateInfo&		 image_info,
		VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
	void free(VkImage out_image, VmaAllocation out_memory);
	void free(VkBuffer buffer, VmaAllocation memory);
	void copy_to_memory(void* src, VmaAllocation mem, VkDeviceSize offset, VkDeviceSize size);
	void copy_from_memory(VmaAllocation src, VkDeviceSize offset, void* dest, VkDeviceSize size);

private:
	VmaAllocator allocator;
};
