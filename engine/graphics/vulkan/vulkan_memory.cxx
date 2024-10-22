#include "vulkan_memory.hpp"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#include "defines.hpp"

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

u32 gpu_memory_allocator::get_size(VmaAllocation memory) {
	VmaAllocationInfo info;
	vmaGetAllocationInfo(allocator, memory, &info);

	return info.size;
}

VkResult gpu_memory_allocator::create_image(
	VkImage*				 out_image,
	VmaAllocation*			 out_memory,
	VkDevice				 device,
	VkImageCreateInfo&		 image_info,
	VmaAllocationCreateFlags flags) {
	VmaAllocationCreateInfo create_info = {};
	create_info.usage					= VMA_MEMORY_USAGE_AUTO;
	create_info.flags					= flags;
	create_info.priority				= 1.0f;

	VkResult result = vmaCreateImage(allocator, &image_info, &create_info, out_image, out_memory, nullptr);
	VK_ASSERT(result, "failed to create image");

	return result;
}

VkResult gpu_memory_allocator::create_buffer(
	VkBuffer*				 out_buffer,
	VmaAllocation*			 out_memory,
	VkDevice				 device,
	VkBufferCreateInfo&		 buffer_info,
	VmaAllocationCreateFlags flags) {
	VmaAllocationCreateInfo create_info = {};
	create_info.usage					= VMA_MEMORY_USAGE_AUTO;
	create_info.flags					= flags;
	create_info.priority				= 1.0f;

	VkResult result = vmaCreateBuffer(allocator, &buffer_info, &create_info, out_buffer, out_memory, nullptr);
	VK_ASSERT(result, "failed to create buffer");

	return result;
}

void gpu_memory_allocator::free(VkImage out_image, VmaAllocation out_memory) {
	vmaDestroyImage(allocator, out_image, out_memory);
}

void gpu_memory_allocator::free(VkBuffer buffer, VmaAllocation memory) {
	vmaDestroyBuffer(allocator, buffer, memory);
}

void gpu_memory_allocator::copy_to_memory(void* src, VmaAllocation mem, VkDeviceSize offset, VkDeviceSize size) {
	TRACE << "copying to device memory \n -- size : " << size << "\n"; 
	VkResult result = vmaCopyMemoryToAllocation(allocator, src, mem, offset, size);
	VK_ASSERT(result, "failed to copy to device memory");
}

void gpu_memory_allocator::copy_from_memory(VmaAllocation src, VkDeviceSize offset, void* dest, VkDeviceSize size) {
	VkResult result = vmaCopyAllocationToMemory(allocator, src, offset, dest, size);
	VK_ASSERT(result, "failed to copy from device memory");
}
