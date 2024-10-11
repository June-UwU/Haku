#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#include "vk_mem_alloc.h"
#include "vulkan_memory.hpp"

// this is intend to be a data type that is created and destroyed by device class.
class vulkan_device;
class vulkan_images {
public:
	vulkan_images(std::string name, std::shared_ptr<vulkan_device> device, VkImageCreateInfo& image_info);
	vulkan_images(std::string name, std::shared_ptr<vulkan_device> device, VkImageCreateInfo& image_info, VkImageViewCreateInfo& view_info);
	~vulkan_images();

	vulkan_images(const vulkan_images&)			   = delete;
	vulkan_images& operator=(const vulkan_images&) = delete;

	vulkan_images(vulkan_images&& rhs);
	vulkan_images& operator=(vulkan_images& rhs);

	VkImage		get_image();
	VkImageView get_view();

private:
	inline void create_allocation(std::shared_ptr<vulkan_device> device, VkImageCreateInfo& image_info);

private:
	std::string					   name;
	VkImage						   image;
	VkImageView					   view;
	VmaAllocation				   memory;
	std::shared_ptr<vulkan_device> device;
};