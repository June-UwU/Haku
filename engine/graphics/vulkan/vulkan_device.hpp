#pragma once
#include "types.hpp"
#include "vulkan/vulkan.h"
#include "vulkan_command_allocator.hpp"
#include "vulkan_memory.hpp"
#include <unordered_map>
#include <vector>

typedef struct surface_capabilities {
	std::vector<VkSurfaceFormatKHR> surface_formats;
	std::vector<VkPresentModeKHR>	present_modes;
	VkSurfaceCapabilitiesKHR		surface_capabilities;
} surface_capabilities;

class vulkan_device {
public:
	vulkan_device(VkInstance instance, VkSurfaceKHR surface);
	~vulkan_device();

	VkDevice			 get_logical_device();
	void				 wait_till_idle();
	surface_capabilities get_surface_device_capabilities(VkSurfaceKHR surface);
	bool				 is_shared_present_queue();
	u32					 get_graphics_queue_index();
	u32					 get_present_queue_index();
	u32					 get_compute_queue_index();
	u32					 get_transfer_queue_index();

	// device functions
	vulkan_images*	create_image(std::string name, VkImageCreateInfo& image_info, VkImageViewCreateInfo& view_info);
	void			free(vulkan_images* image);
	void			submit_commands(VkSubmitInfo& submit_info, VkFence signal);
	void			present(VkPresentInfoKHR& present_info);
	VkCommandBuffer get_graphics_command_buffer(bool is_primary);

private:
	void set_queue_index(VkSurfaceKHR surface);
	void initialize_device_queues();

private:
	std::unique_ptr<gpu_memory_allocator> gpu_allocator;
	std::unordered_map<u32, u32>		  queue_usage;

	u32 graphics_queue_index = static_cast<u32>(-1);
	u32 transfer_queue_index = static_cast<u32>(-1);
	u32 compute_queue_index	 = static_cast<u32>(-1);
	u32 present_queue_index	 = static_cast<u32>(-1);

	std::unique_ptr<vulkan_command_allocator> transfer_allocator;
	std::unique_ptr<vulkan_command_allocator> primary_allocator;

	VkQueue			 present_queue;
	VkQueue			 graphics_queue;
	VkQueue			 compute_queue;
	VkQueue			 transfer_queue;
	VkPhysicalDevice physical_device;
	VkDevice		 logical_device;
};