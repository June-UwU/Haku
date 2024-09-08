#pragma once
#include "types.hpp"
#include "defines.hpp"

bool initialize_renderer(u32 height, u32 width);
u32 draw();
void shutdown_renderer();
