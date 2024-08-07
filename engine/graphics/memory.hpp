#pragma once
#include "types.hpp"
#define VMA_IMPLEMENTATION
#define VMA_VULKAN_VERSION 1002000 // Vulkan 1.2
#define VMA_STATIC_VULKAN_FUNCTIONS 1
#define VMA_DEBUG_MARGIN 16
#define VMA_DEBUG_DETECT_CORRUPTION 1
#include "vma/vk_mem_alloc.h"

bool initialize_gpu_memory();
void destroy_gpu_memory();