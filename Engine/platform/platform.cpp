#include "platform.hpp"
#include "logger.hpp"
#include "event.hpp"
#include <GL/glew.h>
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

  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 


  window = glfwCreateWindow(HAKU_WINDOW_WIDTH, HAKU_WINDOW_HEIGHT, "Haku", NULL, NULL);
  if(nullptr == window) {
    LOG_WARN("GLFW window initialization failed!");
    return FAIL;
  }
  glfwMakeContextCurrent(window);
  glewExperimental=true;

  if(GLEW_OK != glewInit()) {
    LOG_WARN("GLEW initialization failed!");
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

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
