#include "renderer.hpp"
#include "graphics/vulkan/vulkan_context.hpp"

static vulkan_context* context;

bool initialize_renderer() {
	context = new vulkan_context();

	return true;
}

void shutdown_renderer() {
	delete context;
}