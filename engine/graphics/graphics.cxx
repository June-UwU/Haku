#define GLFW_INCLUDE_VULKAN
#include "graphics.hpp"
#include "defines.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "command.hpp"

bool initialize_graphics() {
	bool device_initialized = initialize_device();
	ASSERT(device_initialized, "failed to initialize device\n");
	
	bool renderer_initialized = initialize_renderer();
	ASSERT(renderer_initialized, "failed to initialize renderer");

	bool command_initialized = initialize_command_module();
	ASSERT(command_initialized, "failed to initialize command");
	
	return true;
}

u32 accquire_image() {
	wait_for_frame(); // sync cpu with gpu

	u32 image_index = accquire_swap_image();
	record_command_buffer(get_command_buffer(),image_index);

	return image_index;
}

void draw_frame(u32 frame) {
	
}

void present(u32 frame) {
	present_work(frame);
}

void submit(u32 frame) {
	submit_work(frame);
}

void terminate_graphics() {
	destroy_command_module();
	destroy_renderer();
	destroy_device();
}

