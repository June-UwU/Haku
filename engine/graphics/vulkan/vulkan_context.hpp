#pragma once
#include "types.hpp"
#include "vulkan_results.hpp"
#include "vulkan_device.hpp"
#include "vulkan_command_allocator.hpp"

class vulkan_context {
public:
	vulkan_context();
	~vulkan_context();

	VkInstance get_instance();

private:
	std::unique_ptr<vulkan_device>			  device;
	VkInstance								  instance;
	VkSurfaceKHR							  surface;
	VkDebugUtilsMessengerEXT				  debug_messenger;
};