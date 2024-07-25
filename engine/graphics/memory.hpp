#pragma once
#include "types.hpp"
#include <vulkan/vulkan.h>

VkBuffer create_buffer(VkDeviceSize size, VkBufferUsageFlags flag, VkSharingMode sharing);
VkDeviceMemory create_memory_for_buffer(VkBuffer buffer, VkMemoryPropertyFlags sharing);
u32 find_memory_type(u32 filter, VkMemoryPropertyFlags properties);
void fill_buffer(VkDeviceMemory buffer, void *data, u32 size); // fill buffer doesn't do a host flush 

// class memory {
// public:

// private:
//     VkBuffer buffer;
//     VkDeviceMemory memory;
// };