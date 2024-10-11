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

VkResult gpu_memory_allocator::create_image(VkImage* out_image, VmaAllocation* out_memory, VkDevice device, VkImageCreateInfo& image_info) {
	VmaAllocationCreateInfo create_info = {};
	create_info.usage					= VMA_MEMORY_USAGE_AUTO;
	create_info.flags					= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	create_info.priority				= 1.0f;

	VkResult result = vmaCreateImage(allocator, &image_info, &create_info, out_image, out_memory, nullptr);
	VK_ASSERT(result, "failed to create image");

	return result;
}

void gpu_memory_allocator::free(VkImage out_image, VmaAllocation out_memory) {
	vmaDestroyImage(allocator, out_image, out_memory);
}
