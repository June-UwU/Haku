#include "platform.hpp"
#include "logger.hpp"
#include "event.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct WindowContext {
  GLFWwindow* window;
  u32 height;
  u32 width;
};

static WindowContext* context = nullptr;


void onClose([[maybe_unused]] GLFWwindow* window) {
  Event closeEvt(EVENT_WINDOW_CLOSED);
  fireEvent(std::move(closeEvt));
}

Status initializeWindow(void) {
  if (!glfwInit()) {
    LOG_WARN("GLFW initialization failed!");
  }

  context = new WindowContext;
  context->height = HAKU_WINDOW_HEIGHT;
  context->width = HAKU_WINDOW_WIDTH;

  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 


  context->window = glfwCreateWindow(context->width, context->height, "Haku", NULL, NULL);
  if(nullptr == context->window) {
    LOG_WARN("GLFW window initialization failed!");
    return FAIL;
  }
  glfwMakeContextCurrent(context->window);
  glewExperimental=true;

  if(GLEW_OK != glewInit()) {
    LOG_WARN("GLEW initialization failed!");
    return FAIL;
  }

  glfwSetWindowCloseCallback(context->window,onClose);

  return OK;
}

void shutdownWindow() {
  glfwTerminate();
}

void processEvents() {
  if(false == glfwWindowShouldClose(context->window)) {

    glfwSwapBuffers(context->window);
    glfwPollEvents();
  }
}

u32 getWindowHeight() {
  return context->height;
}

u32 getWindowWidth() {
  return context->width;
}