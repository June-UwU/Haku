#pragma once
#include "types.hpp"
#include <vector>
#include <vulkan/vulkan.h>

typedef struct gpu_queue {
	const s32 INVALID_INDEX = -1;
	bool	  is_gpu_complaint() {
		 return (INVALID_INDEX != graphics_index) && (INVALID_INDEX != present_index);
	}

	s32 graphics_index = INVALID_INDEX;
	s32 present_index  = INVALID_INDEX;
} gpu_queue;

typedef struct swap_chain_support {
	VkSurfaceCapabilitiesKHR		capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>	present_modes;
} swap_chain_support;

bool			   initialize_device();
VkDevice		   get_device();
VkPhysicalDevice   get_physical_device();
VkInstance		   get_instance();
VkSurfaceKHR	   get_render_surface();
VkQueue 		   get_graphics_queue();
VkQueue 		   get_present_queue();
gpu_queue		   find_queue_families(VkPhysicalDevice device);
swap_chain_support query_swapchain_support(VkPhysicalDevice gpu);
void			   destroy_device();