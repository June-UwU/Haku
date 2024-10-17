#include "vulkan_primitives.hpp"

vulkan_buffer::vulkan_buffer(std::string name, std::shared_ptr<vulkan_device>& device, u64 size, byte* data, VkBufferUsageFlags usage)
	: name(name)
	, buffer(VK_NULL_HANDLE)
	, memory(VK_NULL_HANDLE)
	, device(device) {
	VkBufferCreateInfo create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	create_info.pNext				  = nullptr;
	create_info.flags				  = 0;
	create_info.size				  = size;
	create_info.usage				  = usage;
	create_info.sharingMode			  = VK_SHARING_MODE_EXCLUSIVE;
	create_info.queueFamilyIndexCount = 0;
	create_info.pQueueFamilyIndices	  = nullptr;

	VkResult result = device->create_buffer(&buffer, &memory, create_info);
	VK_ASSERT(result, "failed to create the buffer..!!");
	upload_buffer(size, data);
}

vulkan_buffer::vulkan_buffer(std::string name, std::shared_ptr<vulkan_device>& device, std::vector<byte>& data, VkBufferUsageFlags usage)
	: name(name)
	, buffer(VK_NULL_HANDLE)
	, memory(VK_NULL_HANDLE) 	
	, device(device) {
	VkBufferCreateInfo create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	create_info.pNext				  = nullptr;
	create_info.flags				  = 0;
	create_info.size				  = data.size();
	create_info.usage				  = usage;
	create_info.sharingMode			  = VK_SHARING_MODE_EXCLUSIVE;
	create_info.queueFamilyIndexCount = 0;
	create_info.pQueueFamilyIndices	  = nullptr;

	VkResult result = device->create_buffer(&buffer, &memory, create_info);
	VK_ASSERT(result, "failed to create the buffer..!!");
	upload_buffer(data);
}

vulkan_buffer::~vulkan_buffer() {
	device->free(buffer,memory);
}

VkResult vulkan_buffer::upload_buffer(u64 size, byte* data) {
	TODO("assertion for size conformance..");
	VkBuffer		   staging;
	VmaAllocation	   staging_mem;
	VkBufferCreateInfo create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	create_info.pNext				  = nullptr;
	create_info.flags				  = 0;
	create_info.size				  = size;
	create_info.usage				  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	create_info.sharingMode			  = VK_SHARING_MODE_EXCLUSIVE;
	create_info.queueFamilyIndexCount = 0;
	create_info.pQueueFamilyIndices	  = nullptr;

	// NOTE : maybe look into the possiblity to get away with sequetial access
	VkResult result = device->create_buffer(&staging, &staging_mem, create_info, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	if (VK_SUCCESS != result) {
		return result;
	}

	device->copy_to_device(data, staging_mem, 0, size);
	device->copy_btw_device_memory(staging, buffer, size);
	device->free(staging, staging_mem);		
	return VK_SUCCESS;
}

VkResult vulkan_buffer::upload_buffer(std::vector<byte>& data) {
	return upload_buffer(data.size(), data.data());
}

void vulkan_buffer::bind(VkCommandBuffer cmd, u32 binding, VkDeviceSize offset) {
	vkCmdBindVertexBuffers(cmd, binding, 1, &buffer,  &offset);
}

void vulkan_buffer::bind_as_index(VkCommandBuffer cmd) {
	vkCmdBindIndexBuffer(cmd, buffer, 0, VK_INDEX_TYPE_UINT16);
}
