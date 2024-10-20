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
	device->free(buffer, memory);
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
	vkCmdBindVertexBuffers(cmd, binding, 1, &buffer, &offset);
}

void vulkan_buffer::bind_as_index(VkCommandBuffer cmd) {
	vkCmdBindIndexBuffer(cmd, buffer, 0, VK_INDEX_TYPE_UINT16);
}

vulkan_image::vulkan_image(
	std::string						name,
	std::shared_ptr<vulkan_device>& device,
	u32								width,
	u32								height,
	std::vector<byte>&				data,
	VkImageUsageFlags				usage,
	VkImageTiling					tiling)
	: name(name)
	, device(device)
	, height(height)
	, width(width)
	, layout(VK_IMAGE_LAYOUT_UNDEFINED) {
	TRACE << "creating image : " << name << "\n";

	VkImageCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	create_info.pNext		  = nullptr;
	create_info.imageType	  = VK_IMAGE_TYPE_2D; // TODO : variable image type
	create_info.extent.width  = width;
	create_info.extent.height = height;
	create_info.extent.depth  = 1;
	create_info.mipLevels	  = 1;
	create_info.arrayLayers	  = 1;
	create_info.format		  = VK_FORMAT_R8G8B8A8_SRGB;
	create_info.tiling		  = VK_IMAGE_TILING_OPTIMAL;
	create_info.initialLayout = layout;
	create_info.usage		  = VK_IMAGE_USAGE_TRANSFER_DST_BIT | usage;
	create_info.sharingMode	  = VK_SHARING_MODE_EXCLUSIVE;
	create_info.samples		  = VK_SAMPLE_COUNT_1_BIT;
	create_info.flags		  = 0;

	VkResult result = device->create_image(&image, &memory, create_info);
	VK_ASSERT(result, "failed to allocate image");

	result = upload_data(data);
	VK_ASSERT(result, "failed to upload data..!");
}

vulkan_image::vulkan_image(
	std::string						name,
	std::shared_ptr<vulkan_device>& device,
	u32								width,
	u32								height,
	byte*							data,
	VkImageUsageFlags				usage,
	VkImageTiling					tiling)
	: name(name)
	, device(device)
	, height(height)
	, width(width)
	, layout(VK_IMAGE_LAYOUT_UNDEFINED) {
	TRACE << "creating image : " << name << "\n" << width << " X " << height << "\n";

	VkImageCreateInfo create_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	create_info.pNext		  = nullptr;
	create_info.imageType	  = VK_IMAGE_TYPE_2D; // TODO : variable image type
	create_info.extent.width  = width;
	create_info.extent.height = height;
	create_info.extent.depth  = 1;
	create_info.mipLevels	  = 1;
	create_info.arrayLayers	  = 1;
	create_info.format		  = VK_FORMAT_R8G8B8A8_SRGB;
	create_info.tiling		  = VK_IMAGE_TILING_OPTIMAL;
	create_info.initialLayout = layout;
	create_info.usage		  = VK_IMAGE_USAGE_TRANSFER_DST_BIT | usage;
	create_info.sharingMode	  = VK_SHARING_MODE_EXCLUSIVE;
	create_info.samples		  = VK_SAMPLE_COUNT_1_BIT;
	create_info.flags		  = 0;
	const u32 size			  = width * height * 4;
	VkResult result = device->create_image(&image, &memory, create_info);
	VK_ASSERT(result, "failed to allocate image");

	result = upload_data(size, data);
	VK_ASSERT(result, "failed to upload data..!");
}

vulkan_image::~vulkan_image() {
	device->free(image, memory);
}

VkResult vulkan_image::upload_data(std::vector<byte>& data) {
	const u32 size = width * height * 4;
	return upload_data(size, data.data());
}

VkResult vulkan_image::upload_data(const u32 size, byte* data) {
	TODO("assertion for size conformance..");
	VkBuffer	  staging;
	VmaAllocation staging_mem;

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

	device->copy_to_device(reinterpret_cast<void*>(data), staging_mem, 0, size);
	result = transition_layout_for_copy(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL); // TODO : start and end
	VK_ASSERT(result, "failed to change layout for copy..!!\n");
	VkBufferImageCopy region;
	region.bufferOffset		 = 0;
	region.bufferRowLength	 = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel	   = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount	   = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };
	device->copy_from_memory_to_image(staging, image, layout, region);

	result = transition_layout_for_copy(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	VK_ASSERT(result, "failed to change layout for copy..!!\n");

	device->free(staging, staging_mem);
	
	return result;
}

VkResult vulkan_image::transition_layout_for_copy(VkImageLayout new_layout) {
	VkImageMemoryBarrier barrier{};
	barrier.sType							= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout						= layout;
	barrier.newLayout						= new_layout;
	barrier.srcQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex				= VK_QUEUE_FAMILY_IGNORED;
	barrier.image							= image;
	barrier.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel	= 0;
	barrier.subresourceRange.levelCount		= 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount		= 1;
	barrier.srcAccessMask					= 0; // TODO
	barrier.dstAccessMask					= 0; // TODO

	VkResult result = device->transition_layout(barrier);
	VK_ASSERT(result, "failed to transition image layout..!!");
	layout = new_layout;

	return result;
}
