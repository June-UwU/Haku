#define GLFW_INCLUDE_VULKAN
#include "graphics.hpp"
#include "defines.hpp"
#include "device.hpp"
#include "runtime.hpp"
#include "memory.hpp"

bool initialize_graphics() {
	bool device_initialized = initialize_device();
	ASSERT(device_initialized, "failed to initialize device\n");

	bool gpu_memory_allocator = initialize_gpu_memory();
	ASSERT(gpu_memory_allocator, "failed to initialize gpu memory allocator");
	
	bool runtime_init = initialize_runtime();
	ASSERT(runtime_init, "failed to initialize runtime structures\n");

	return true;
}

void terminate_graphics() {
	destroy_runtime();
	destroy_gpu_memory();
	destroy_device();
}

void wait_on_gpu_till_idle() {
	vkDeviceWaitIdle(get_device());
}

