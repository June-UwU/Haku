#pragma once
#include "types.hpp"

bool initialize_windows(u32 height, u32 width);
bool window_still_open();
void window_update();
void terminate_window();