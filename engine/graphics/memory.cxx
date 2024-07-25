#include "memory.hpp"
#include "defines.hpp"
#include "device.hpp"

VkBuffer create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharing) {
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = sharing;

    TRACE << "creating buffer...\n" 
    << "buffer size : " << size << "\n"
    << "usage : " << usage << "\n"
    << "sharing mode : " << sharing << "\n";

    VkBuffer buffer;
    auto create_status = vkCreateBuffer(get_device(), &buffer_info, nullptr, &buffer);
    ASSERT(VK_SUCCESS == create_status, "failed to create buffer");

    return buffer;
}

u32 find_memory_type(u32 filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(get_physical_device(), &memory_properties);

    for(u32 i = 0; i < memory_properties.memoryTypeCount; i++) {
        if((filter & (i << i)) && ((memory_properties.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }

    ASSERT_UNREACHABLE();
}

VkDeviceMemory create_memory_for_buffer(VkBuffer buffer, VkMemoryPropertyFlags properties) {
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(get_device(), buffer, &requirements);

    VkMemoryAllocateInfo allocation_info{};
    allocation_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocation_info.allocationSize = requirements.size;
    allocation_info.memoryTypeIndex = find_memory_type(requirements.memoryTypeBits, properties);

    TRACE << "allocating gpu memory...\n" 
    << "memory size : " << requirements.size << "\n"
    << "memory index : " << allocation_info.memoryTypeIndex << "\n";

    VkDeviceMemory memory;
    auto allocation_status = vkAllocateMemory(get_device(),&allocation_info, nullptr,&memory);
    ASSERT(VK_SUCCESS == allocation_status, "failed to allocate device memory..");

    vkBindBufferMemory(get_device(),buffer,memory,0);
    return memory;  
}

void fill_buffer(VkDeviceMemory buffer, void *data, u32 size) {
    void* mem_mapping = nullptr;

    TRACE << "filling device memory...\n"
    << "data : " << data << "\n"
    << "size : " << size << "\n";

    vkMapMemory(get_device(), buffer, 0, size, 0, &mem_mapping);
    memcpy(mem_mapping,data,static_cast<size_t>(size));
    vkUnmapMemory(get_device(), buffer);
}