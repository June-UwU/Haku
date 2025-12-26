#pragma once
#include <string>
#include "../types.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class window {
public:
  window(std::string name, s32 width, s32 height);
  ~window();

  s32 get_width() { return width; }
  s32 get_height() { return height; }
  GLFWwindow *get_handle() { return handle; }
  
private:
  std::string title;
  s32 width;
  s32 height;
  GLFWwindow* handle;
};  

