#include "window.hpp"
#include "logger.hpp"
#include "defines.hpp"

static GLFWwindow* window = nullptr;

static void error_callback(s32 error, const char* description) {
	ERROR << "Code : " << error << " : " << description << "\n";
}

static void key_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods) {
	HAKU_UNUSED(window);
	HAKU_UNUSED(key);
	HAKU_UNUSED(action);
	HAKU_UNUSED(mods);

	TRACE << "key event received\n";
}

bool initialize_windows(u32 height, u32 width) {
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		ERROR << "Failed to initialize GLFW\n";
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(static_cast<s32>(width), static_cast<s32>(height), "Haku", NULL, NULL);

	if (nullptr == window) {
		ERROR << "Failed to create GLFW window\n";
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(window, key_callback);

	return true;
}

bool window_still_open() {
	return false == glfwWindowShouldClose(window);
}

void window_update() {
	glfwPollEvents();
}

void terminate_window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWwindow* get_window() {
	return window;
}
