#include "memory.hpp"
#include "device.hpp"

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
}

void destroy_gpu_memory() {
	vmaDestroyAllocator(allocator);
}