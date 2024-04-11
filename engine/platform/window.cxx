#include "window.hpp"
#include "logger.hpp"
#include <GLFW/glfw3.h>

static GLFWwindow* window = nullptr;

static void error_callback(s32 error, const char* description)
{
	ERROR << "Code : " << error << " : " << description << "\n";
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	TRACE << "key event received\n";
}

bool initialize_windows(u32 height, u32 width)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		ERROR << "Failed to initialize GLFW\n";
		return false;
	}

	window = glfwCreateWindow(width, height, "Haku", NULL, NULL);

	if (nullptr == window)
	{
		ERROR << "Failed to create GLFW window\n";
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);

	return true;
}

bool window_still_open()
{
	return false == glfwWindowShouldClose(window);
}

void window_update()
{
	glfwSwapBuffers(window);

	glfwPollEvents();
}

void terminate_window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}
