#pragma once
#include "types.hpp"

bool initialize_graphics();
u32 accquire_image();
void present(u32 frame);
void submit(u32 frame);
void draw_frame(u32 frame);
void terminate_graphics();
