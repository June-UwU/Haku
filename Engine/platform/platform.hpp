#pragma once

#include "defines.hpp"

constexpr u32 HAKU_WINDOW_HEIGHT  = 720;
constexpr u32 HAKU_WINDOW_WIDTH   = 1080;


s32 initializeWindow(void);

void shutdownWindow();

void processEvents();
