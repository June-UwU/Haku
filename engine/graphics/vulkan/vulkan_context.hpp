#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#include "vulkan_device.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_fence.hpp"
#include "vulkan_renderpass.hpp"

class vulkan_context {
public:
	vulkan_context(u32 width, u32 height);
	~vulkan_context();

	VkInstance get_instance();
	u32		   draw_frame();

private:
	void			create_sync_parameter();
	void			destroy_sync_parameter();
	void			submit_command_buffer(u32 index);
	void			present_frame(u32 index, u32 image_index);
	void			reserve_command_buffer();
	VkCommandBuffer accquire_command_buffer(u32 index);
	void 			create_renderpass();

private:
	u32						 frame;
	u32						 width;
	u32						 height;
	vulkan_device*			 device;
	VkSurfaceKHR			 surface;
	VkInstance				 instance;
	vulkan_swapchain*		 swapchain;
	VkDebugUtilsMessengerEXT debug_messenger;
	
	vulkan_renderpass*		 renderpass; 		
	// recording command buffer
	std::vector<VkCommandBuffer> command_buffer;

	// sync parameters
	std::vector<vulkan_fence*> in_flight;
	std::vector<VkSemaphore>   image_available;
	std::vector<VkSemaphore>   render_complete;
};