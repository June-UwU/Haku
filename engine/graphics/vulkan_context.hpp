#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"

class vulkan_context {
    public:
        vulkan_context();
    private:
        VkInstance instance;
}