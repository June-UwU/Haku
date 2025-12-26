#include "logger.hpp"
#include "defines.hpp"
#include "engine.hpp"
#include "window/window.hpp"
#include <GLFW/glfw3.h>

void error_callback(s32 error, const char* desc){
  FATAL << "glfw failed with code : " << error
  << " : " << desc <<"\n";
}

engine::engine(s32 argc, char **argv)
  :running(true)    {
  // TODO : parse arguments.
  if (!glfwInit()) {
    FATAL << "failed to initialize GLFW\n";
    goto glfw_init_fail;
  }
  glfwSetErrorCallback(error_callback);

  name = "Haku";
  p_main_window = std::make_shared<window>(name, 1280, 720);
  p_gpu_context = std::make_unique<context>(VULKAN_API, p_main_window);
  return;

glfw_init_fail:
  std::abort();
}

s32 engine::run_engine() {
  while (true == running) {
    glfwPollEvents();
  }

  return SUCCESS;
}

engine::~engine() {
  p_gpu_context = nullptr;
  p_main_window = nullptr;
  glfwTerminate();
}

