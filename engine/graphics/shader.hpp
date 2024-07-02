#pragma once
#include "types.hpp"
#include <vector>
#include <vulkan/vulkan.h>

VkShaderModule create_shader_module(std::vector<byte>& byte_code);
void		   destroy_shader_module(VkShaderModule& module);