#include <memory>
#include "../defines.hpp"
#include "window.hpp"


window::window(std::string name, s32 width, s32 height)
    : title(name), width(width), height(height)
      {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  
  handle = glfwCreateWindow(width, height,
                                      name.c_str(),nullptr, nullptr);                                      
  VERIFY_PTR(handle);
}

window::~window() {
    glfwDestroyWindow(handle);  
}  
