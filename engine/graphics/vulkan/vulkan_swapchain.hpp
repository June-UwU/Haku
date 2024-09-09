#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#include "vulkan_images.hpp"
#include <vector>
#include <memory>

class vulkan_device;
constexpr u32 MAX_FRAME_IN_FLIGHT = 3;

class vulkan_swapchain {
public:
	vulkan_swapchain(const u32 request_width, u32 request_height, VkSurfaceKHR surface, vulkan_device* device);
	~vulkan_swapchain();

	u32 get_image_count();
	u32 accquire_image_index(VkDevice device, VkSemaphore image_available);
	VkSwapchainKHR get_swapchain();
	VkFormat get_swapchain_format();
private:
	void create_new_swapchain(VkSurfaceKHR surface, vulkan_device* device);
	void accquire_swapchain_images(vulkan_device* device);
	void create_depth_buffer(vulkan_device* device);
	void destroy_swapchain();

private:
	vulkan_device*			 gpu;
	u32						 height;
	u32						 width;
	VkSurfaceFormatKHR		 format;
	VkSwapchainKHR			 swapchain;
	std::vector<VkImage>	 image;
	std::vector<VkImageView> view;
	vulkan_images*			 depth_buffer;
};