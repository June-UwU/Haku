#pragma once
#include "types.hpp"
#include "defines.hpp"
#include "vertex_data.hpp"

bool initialize_renderer(u32 width, u32 height);
u32 draw();
void shutdown_renderer();
