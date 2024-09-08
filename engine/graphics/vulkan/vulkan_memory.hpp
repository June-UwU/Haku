#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#define VMA_STATIC_VULKAN_FUNCTIONS	 1
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"
#include <vector>

class vulkan_images;
class gpu_memory_allocator {
public:
	gpu_memory_allocator(VkPhysicalDevice gpu, VkDevice device, VkInstance instance);
	~gpu_memory_allocator();

	gpu_memory_allocator(const gpu_memory_allocator& alloc)			  = delete;
	gpu_memory_allocator operator=(const gpu_memory_allocator& alloc) = delete;

	gpu_memory_allocator(gpu_memory_allocator&& alloc)			  = delete;
	gpu_memory_allocator& operator=(gpu_memory_allocator&& alloc) = delete;

	vulkan_images* create_image(std::string name, VkDevice device, VkImageCreateInfo& image_info, VkImageViewCreateInfo& view_info);
	void		   free(vulkan_images* image, VkDevice device);

private:
	VmaAllocator allocator;
};
