#pragma once
#include "types.hpp"
#include <GLFW/glfw3.h>

bool		initialize_windows(u32 height, u32 width);
bool		window_still_open();
void		window_update();
void		terminate_window();
GLFWwindow* get_window();