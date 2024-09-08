#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#include "vk_mem_alloc.h"
#include "vulkan_memory.hpp"

// this is intend to be a data type that is created and destroyed by device class.
class vulkan_device;
class vulkan_images {
	friend class gpu_memory_allocator;

public:
	vulkan_images(std::string name, VkDevice device, VmaAllocation memory, VkImage image);
	vulkan_images(std::string name, VkDevice device, VmaAllocation memory, VkImage image, VkImageViewCreateInfo& view_info);

	vulkan_images(const vulkan_images&)			   = delete;
	vulkan_images& operator=(const vulkan_images&) = delete;

	vulkan_images(vulkan_images&& rhs);
	vulkan_images& operator=(vulkan_images& rhs);

	VkImage		get_image();
	VkImageView get_view();

private:
	std::string	  name;
	VkImage		  image;
	VkImageView	  view;
	VmaAllocation memory;
};