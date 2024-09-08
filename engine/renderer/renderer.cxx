#include "renderer.hpp"
#include "graphics/vulkan/vulkan_context.hpp"

static vulkan_context* context;

bool initialize_renderer(u32 height, u32 width) {
	context = new vulkan_context(height, width);

	return true;
}

u32 draw() {
	return context->draw_frame();
}

void shutdown_renderer() {
	delete context;
}