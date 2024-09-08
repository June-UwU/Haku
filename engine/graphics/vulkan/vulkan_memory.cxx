#include "vulkan_memory.hpp"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#include "defines.hpp"
#include "vulkan_images.hpp"

gpu_memory_allocator::gpu_memory_allocator(VkPhysicalDevice gpu, VkDevice device, VkInstance instance) {
	VmaAllocatorCreateInfo create_info{};
	create_info.device						   = device;
	create_info.flags						   = 0;
	create_info.instance					   = instance;
	create_info.pAllocationCallbacks		   = nullptr;
	create_info.pDeviceMemoryCallbacks		   = nullptr;
	create_info.pHeapSizeLimit				   = nullptr;
	create_info.physicalDevice				   = gpu;
	create_info.preferredLargeHeapBlockSize	   = 0;
	create_info.pTypeExternalMemoryHandleTypes = nullptr;
	create_info.pVulkanFunctions			   = nullptr;
	create_info.vulkanApiVersion			   = VK_MAKE_VERSION(1, 2, 0);

	auto result = vmaCreateAllocator(&create_info, &allocator);
	VK_ASSERT(result, "gpu memory allocator failure..!!");
}

gpu_memory_allocator::~gpu_memory_allocator() {
	vmaDestroyAllocator(allocator);
}

vulkan_images* gpu_memory_allocator::create_image(std::string name, VkDevice device, VkImageCreateInfo& image_info, VkImageViewCreateInfo& view_info) {
	VmaAllocationCreateInfo create_info = {};
	create_info.usage					= VMA_MEMORY_USAGE_AUTO;
	create_info.flags					= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	create_info.priority				= 1.0f;

	VkImage		  image;
	VmaAllocation memory;

	TRACE << "create image : " << name << "\n";
	VkResult result = vmaCreateImage(allocator, &image_info, &create_info, &image, &memory, nullptr);
	VK_ASSERT(result, "failed to create image");

	view_info.image = image;
	auto ptr		= new vulkan_images(name, device, memory, image, view_info);

	return ptr;
}

void gpu_memory_allocator::free(vulkan_images* image, VkDevice device) {
	vmaDestroyImage(allocator, image->image, image->memory);
	if (VK_NULL_HANDLE == image->view) {
		return;
	}

	vkDestroyImageView(device, image->view, nullptr);
}
