#pragma once
#include "types.hpp"
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

typedef struct vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription get_vertex_binding_description() {
        VkVertexInputBindingDescription binding_description{};

        // TODO : we are not doing instance rendering at the moment, we porbably should do that
        binding_description.binding = 0;
        binding_description.stride = sizeof(vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 2> get_attribute_description() {
        std::array<VkVertexInputAttributeDescription,2> attributes{};

        attributes[0].binding = 0;
        attributes[0].location = 0;
        attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[0].offset = offsetof(vertex,pos);

        attributes[1].binding = 0;
        attributes[1].location = 1;
        attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[1].offset = offsetof(vertex,color);

        return attributes;
    }
}vertex;


const std::vector<vertex> test_data = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};