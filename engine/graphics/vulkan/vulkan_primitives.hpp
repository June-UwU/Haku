#pragma once
#include "types.hpp"
#include "defines.hpp"
#include <array>
#include "vulkan/vulkan.h"
#include "renderer/vertex_data.hpp"
#include "vulkan_images.hpp"
#include "vulkan_device.hpp"
#include <cstddef>

class vulkan_buffer {
public:
	vulkan_buffer() = delete;
	vulkan_buffer(std::string name, std::shared_ptr<vulkan_device>& device, u64 size, byte* data, VkBufferUsageFlags usage);
	vulkan_buffer(std::string name, std::shared_ptr<vulkan_device>& device, std::vector<byte>& data, VkBufferUsageFlags usage);
	~vulkan_buffer();

	VkResult upload_buffer(u64 size, byte* data);
	VkResult upload_buffer(std::vector<byte>& data);
	void	 bind(VkCommandBuffer cmd, u32 binding, VkDeviceSize offset);
	void	 bind_as_index(VkCommandBuffer cmd);

private:
	std::string					   name;
	VkBuffer					   buffer;
	VmaAllocation				   memory;
	std::shared_ptr<vulkan_device> device;
};

// NOTE : describes how the data in gpu onces uploaded to gpu
// TODO : instanced rendering
static VkVertexInputBindingDescription get_binding_description() {
	VkVertexInputBindingDescription bindings;

	bindings.binding   = 0;
	bindings.stride	   = sizeof(vertex);
	bindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindings;
};

static std::array<VkVertexInputAttributeDescription, 2> get_attribute_descriptions() {
	std::array<VkVertexInputAttributeDescription, 2> attributes;

	attributes[0].binding  = 0;
	attributes[0].location = 0;
	attributes[0].format   = VK_FORMAT_R32G32_SFLOAT;
	attributes[0].offset   = offsetof(vertex, pos);

	attributes[1].binding  = 0;
	attributes[1].location = 1;
	attributes[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[1].offset   = offsetof(vertex, col);

	return attributes;
}