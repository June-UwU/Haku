#include "platform.hpp"
#include "logger.hpp"
#include "event.hpp"
#include <GLFW/glfw3.h>

static GLFWwindow* window = nullptr;

void onClose([[maybe_unused]] GLFWwindow* window) {
  Event closeEvt(EVENT_WINDOW_CLOSED);
  fireEvent(std::move(closeEvt));
}

Status initializeWindow(void) {
  if (!glfwInit()) {
    LOG_WARN("GLFW initialization failed!");
  }

  window = glfwCreateWindow(HAKU_WINDOW_WIDTH, HAKU_WINDOW_HEIGHT, "Haku", NULL, NULL);
  if(nullptr == window) {
    LOG_WARN("GLFW window initialization failed!");
    return FAIL;
  }

  glfwSetWindowCloseCallback(window,onClose);

  return OK;
}

void shutdownWindow() {
  glfwTerminate();
}

void processEvents() {
  if(false == glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}
