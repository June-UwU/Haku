#include "memory.hpp"
#include "device.hpp"
#include "defines.hpp"
#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

VmaAllocator allocator;

bool initialize_gpu_memory() {
	VmaAllocatorCreateInfo allocation_create_info = {};
	allocation_create_info.flags				  = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
	allocation_create_info.vulkanApiVersion		  = VK_API_VERSION_1_2;
	allocation_create_info.physicalDevice		  = get_physical_device();
	allocation_create_info.device				  = get_device();
	allocation_create_info.instance				  = get_instance();
	allocation_create_info.pVulkanFunctions		  = nullptr;

	vmaCreateAllocator(&allocation_create_info, &allocator);

	return true;
}

void destroy_gpu_memory() {
	vmaDestroyAllocator(allocator);
}

void free_gpu_memory(VmaAllocation alloc) {
	vmaFreeMemory(allocator, alloc);
}

void create_buffer(VkBufferCreateInfo* buffer_info, VmaAllocationCreateInfo* alloc_create_info, VkBuffer* buffer, VmaAllocation* alloc, VmaAllocationInfo* alloc_info) {
	auto result = vmaCreateBuffer(allocator, buffer_info, alloc_create_info, buffer, alloc, alloc_info);
	ASSERT(result == VK_SUCCESS, "failed to allocate memory");
}