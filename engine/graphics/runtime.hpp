#pragma once
#include "types.hpp"
#include "defines.hpp"
#include <vulkan/vulkan.h>

bool initialize_runtime();
void destroy_runtime();

u32	 accquire_image(const u64 frame);
void record_image(const u64 frame, const u32 image);
void submit_image(const u64 frame, const u32 image);
void present_image(const u64 frame, const u32 image);