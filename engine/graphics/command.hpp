#pragma once
#include "types.hpp"
#include <vulkan/vulkan.h>

bool initialize_command_module();
void wait_for_frame();
void present_work(u32 frame);
void submit_work(u32 frame);
u32 accquire_swap_image();
u32 submit_commands(u32 frame);
VkCommandBuffer get_command_buffer();
void record_command_buffer(VkCommandBuffer buffer, u32 image_index);
void bind_pipeline(VkCommandBuffer buffer, VkPipeline pipeline);
void set_extend_and_scissor_to_max(VkCommandBuffer buffer);
void draw(VkCommandBuffer buffer, u32 vertex_count, u32 instance, u32 first_vertex, u32 first_instance);
void end_recording_command_buffer(VkCommandBuffer buffer);
void destroy_command_module();