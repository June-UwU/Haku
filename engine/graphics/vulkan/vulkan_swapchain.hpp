#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#include "vulkan_images.hpp"
#include "vulkan_renderpass.hpp"
#include <vector>
#include <memory>

class vulkan_device;
constexpr u32 MAX_FRAME_IN_FLIGHT = 3;

class vulkan_swapchain {
public:
	vulkan_swapchain(const u32 request_width, u32 request_height, VkSurfaceKHR surface, std::shared_ptr<vulkan_device> device);
	~vulkan_swapchain();

	u32								   get_image_count();
	u32								   accquire_image_index(VkDevice device, VkSemaphore image_available);
	VkSwapchainKHR					   get_swapchain();
	VkFormat						   get_swapchain_format();
	VkFormat						   get_depth_format();
	std::shared_ptr<vulkan_renderpass> get_3d_renderpass();
	void							   start_renderpass(VkCommandBuffer cmd_buffer, u32 image_index);
	void							   end_renderpass(VkCommandBuffer cmd_buffer);

private:
	void create_new_swapchain(VkSurfaceKHR surface, std::shared_ptr<vulkan_device> device);
	void accquire_swapchain_images(std::shared_ptr<vulkan_device> device);
	void create_depth_buffer(std::shared_ptr<vulkan_device> device);
	void destroy_swapchain();
	void create_renderpass();
	void create_framebuffer();

private:
	std::shared_ptr<vulkan_device>	   device;
	u32								   height;
	u32								   width;
	VkSurfaceFormatKHR				   format;
	VkSwapchainKHR					   swapchain;
	std::vector<VkImage>			   image;
	std::vector<VkImageView>		   view;
	std::vector<VkFramebuffer>		   frame_buffer;
	std::unique_ptr<vulkan_images>	   depth_buffer;
	std::shared_ptr<vulkan_renderpass> renderpass3d;
};