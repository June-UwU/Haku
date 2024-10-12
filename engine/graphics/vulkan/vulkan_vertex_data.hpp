#pragma once
#include "types.hpp"
#include "defines.hpp"
#include <array>
#include "vulkan/vulkan.h"
#include "renderer/vertex_data.hpp"

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
	attributes[0].offset   = HAKU_OFFSET(vertex, pos);

	attributes[1].binding  = 0;
	attributes[1].location = 1;
	attributes[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[1].offset   = HAKU_OFFSET(vertex, col);

	return attributes;
}