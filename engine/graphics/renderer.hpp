#pragma once
#include "types.hpp"
#include "command.hpp"
#include <vulkan/vulkan.h>

bool		  initialize_renderer();
VkFramebuffer get_frame_buffer(u32 index);
VkExtent2D	  get_swapchain_extend();
VkPipeline	  get_render_pipeline();
VkRenderPass  get_render_pass();
VkSwapchainKHR get_swap_chain();
void		  destroy_renderer();