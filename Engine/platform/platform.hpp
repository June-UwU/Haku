#pragma once

#include "defines.hpp"

constexpr u32 HAKU_WINDOW_HEIGHT  = 720;
constexpr u32 HAKU_WINDOW_WIDTH   = 1080;


HakuWindow initializeWindow(void);

void shutdownWindow(HakuWindow window);

bool windowValid(HakuWindow window);

const s32 WindowEvents(void);
