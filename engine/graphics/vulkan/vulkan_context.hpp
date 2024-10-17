#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#include "vulkan_device.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_fence.hpp"
#include "vulkan_renderpass.hpp"
#include "renderer/vertex_data.hpp"

class vulkan_context {
public:
	vulkan_context(u32 width, u32 height);
	~vulkan_context();

	VkInstance get_instance();
	u32		   draw_frame();
	void	   update_projection(glm::mat4& proj);
	void	   update_view(glm::mat4& view);
	void	   update_model(glm::mat4& model);

private:
	void			create_sync_parameter();
	void			destroy_sync_parameter();
	void			submit_command_buffer(u32 index);
	void			present_frame(u32 index, u32 image_index);
	void			reserve_command_buffer();
	VkCommandBuffer accquire_command_buffer(u32 index);
	void			create_renderpass();
	bool			make_default_context_objects(std::shared_ptr<vulkan_device>& device);

private:
	u32								  frame;
	u32								  width;
	u32								  height;
	std::shared_ptr<vulkan_device>	  device;
	VkSurfaceKHR					  surface;
	VkInstance						  instance;
	std::shared_ptr<vulkan_swapchain> swapchain;
	VkDebugUtilsMessengerEXT		  debug_messenger;

	mvp local_constant;
	// recording command buffer
	std::vector<VkCommandBuffer> command_buffer;
	// sync parameters
	std::vector<vulkan_fence*> in_flight;
	std::vector<VkSemaphore>   image_available;
	std::vector<VkSemaphore>   render_complete;
};