#include "renderer.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "graphics/vulkan/vulkan_context.hpp"
#include <chrono>

static vulkan_context* context;
static mvp			   model_view_proj;
int					   renderheight;
int					   renderwidth;

bool initialize_renderer(u32 width, u32 height) {
	context = new vulkan_context(width, height);
	renderheight		  = height;
	renderwidth			  = width;

	model_view_proj.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model_view_proj.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model_view_proj.proj  = glm::perspective(glm::radians(45.0f), renderwidth / (float)renderheight, 0.1f, 10.0f);
	model_view_proj.proj[1][1] *= -1;

	context->update_view(model_view_proj.view);
	context->update_model(model_view_proj.model);
	context->update_projection(model_view_proj.proj);

	return true;
}

u32 draw() {
	static auto start_time = std::chrono::high_resolution_clock::now();

	auto end_time = std::chrono::high_resolution_clock::now();
	auto time	  = (end_time - start_time).count();

	model_view_proj.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model_view_proj.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model_view_proj.proj  = glm::perspective(glm::radians(45.0f), renderwidth / (float)renderheight, 0.1f, 10.0f);
	model_view_proj.proj[1][1] *= -1;

	context->update_view(model_view_proj.view);
	context->update_model(model_view_proj.model);
	context->update_projection(model_view_proj.proj);

	return context->draw_frame();
}

void shutdown_renderer() {
	delete context;
}