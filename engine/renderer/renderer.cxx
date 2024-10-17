#include "renderer.hpp"
#include "graphics/vulkan/vulkan_context.hpp"

static vulkan_context* context;

bool initialize_renderer(u32 width, u32 height) {
	context = new vulkan_context(width, height);

	return true;
}

u32 draw() {
	return context->draw_frame();
}

void shutdown_renderer() {
	delete context;
}