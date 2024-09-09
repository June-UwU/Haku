#include "vulkan_swapchain.hpp"
#include "vulkan_device.hpp"
#include <algorithm>
#include <limits>

vulkan_swapchain::vulkan_swapchain(const u32 request_width, u32 request_height, VkSurfaceKHR surface, vulkan_device* device)
	: gpu(device) {
	ASSERT(VK_NULL_HANDLE != surface, "surface handle cannot be null for swapchain creation");
	height = request_height;
	width  = request_width;

	create_new_swapchain(surface, device);
}

vulkan_swapchain::~vulkan_swapchain() {
	destroy_swapchain();
}

u32 vulkan_swapchain::get_image_count() {
	return image.size();
}

u32 vulkan_swapchain::accquire_image_index(VkDevice device, VkSemaphore image_available) {
	u32 image_index = -1;
	const u64 INF = std::numeric_limits<u64>::max();
	vkAcquireNextImageKHR(device, swapchain, INF, image_available, VK_NULL_HANDLE, &image_index);
	return image_index;
}

VkSwapchainKHR vulkan_swapchain::get_swapchain() {
	return swapchain;
}

void vulkan_swapchain::create_new_swapchain(VkSurfaceKHR surface, vulkan_device* device) {
	surface_capabilities capabilites = device->get_surface_device_capabilities(surface);

	bool format_found = false;
	for (auto& supported_format : capabilites.surface_formats) {
		if ((supported_format.format == VK_FORMAT_B8G8R8A8_UNORM) && (supported_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)) {
			format		 = supported_format;
			format_found = true;
			break;
		}
	}

	if (false == format_found) {
		ASSERT(0 != capabilites.surface_formats.size(), "The device supports no surface format..!!");
		format = capabilites.surface_formats[0];
	}

	VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
	for (auto supported_modes : capabilites.present_modes) {
		if (VK_PRESENT_MODE_MAILBOX_KHR == supported_modes) {
			present_mode = supported_modes;
			break;
		}
	}

	VkExtent2D swap_extend{
		width,
		height
	};

	VkExtent2D min	   = capabilites.surface_capabilities.maxImageExtent;
	VkExtent2D max	   = capabilites.surface_capabilities.minImageExtent;
	swap_extend.height = std::clamp(swap_extend.height, min.height, max.height);
	swap_extend.width  = std::clamp(swap_extend.width, min.width, max.width);

	VkSwapchainCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	create_info.pNext			 = nullptr;
	create_info.flags			 = 0;
	create_info.surface			 = surface;
	create_info.minImageCount	 = MAX_FRAME_IN_FLIGHT;
	create_info.imageFormat		 = format.format;
	create_info.imageColorSpace	 = format.colorSpace;
	create_info.imageExtent		 = swap_extend;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage		 = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	if (true != device->is_shared_present_queue()) {
		// NOTE : this could go south if the present queue is on a different family index... 0w0
		std::vector<u32> queue_index{
			device->get_graphics_queue_index(),
			device->get_present_queue_index()
		};

		TRACE << "shared queue index : ";
		print_vec(queue_index);

		create_info.imageSharingMode	  = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = queue_index.size();
		create_info.pQueueFamilyIndices	  = queue_index.data();
	} else {
		create_info.imageSharingMode	  = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices	  = nullptr;
	}

	create_info.preTransform   = capabilites.surface_capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode	   = present_mode;
	create_info.clipped		   = VK_TRUE;
	create_info.oldSwapchain   = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(device->get_logical_device(), &create_info, nullptr, &swapchain);
	VK_ASSERT(result, "failed to create swapchain");

	accquire_swapchain_images(device);
	create_depth_buffer(device);
}

void vulkan_swapchain::accquire_swapchain_images(vulkan_device* device) {
	image.resize(0);

	u32		 image_count = 0;
	VkResult result		 = vkGetSwapchainImagesKHR(device->get_logical_device(), swapchain, &image_count, nullptr);
	VK_ASSERT(result, "failed to get swapchain image count...!!");

	image.resize(image_count);
	result = vkGetSwapchainImagesKHR(device->get_logical_device(), swapchain, &image_count, image.data());
	VK_ASSERT(result, "failed to get swapchain images...!!");

	view.resize(image_count);
	for (u32 i = 0; i < image_count; i++) {
		VkImageViewCreateInfo view_info			  = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		view_info.image							  = image[i];
		view_info.viewType						  = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format						  = format.format;
		view_info.subresourceRange.aspectMask	  = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel	  = 0;
		view_info.subresourceRange.levelCount	  = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount	  = 1;

		result = vkCreateImageView(device->get_logical_device(), &view_info, nullptr, &view[i]);
		VK_ASSERT(result, "failed to create image view");
	}
}

void vulkan_swapchain::create_depth_buffer(vulkan_device* device) {
	VkImageCreateInfo depth_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	depth_info.pNext				 = nullptr;
	depth_info.flags				 = 0;
	depth_info.format				 = VK_FORMAT_D24_UNORM_S8_UINT;
	depth_info.imageType			 = VK_IMAGE_TYPE_2D;
	depth_info.extent.width			 = width;
	depth_info.extent.height		 = height;
	depth_info.extent.depth			 = 1;
	depth_info.mipLevels			 = 1;
	depth_info.arrayLayers			 = 1;
	depth_info.samples				 = VK_SAMPLE_COUNT_1_BIT;
	depth_info.tiling				 = VK_IMAGE_TILING_OPTIMAL;
	depth_info.usage				 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depth_info.sharingMode			 = VK_SHARING_MODE_EXCLUSIVE;
	depth_info.queueFamilyIndexCount = 0;
	depth_info.pQueueFamilyIndices	 = nullptr;
	depth_info.initialLayout		 = VK_IMAGE_LAYOUT_UNDEFINED;

	VkImageSubresourceRange depth_sub_resource;
	depth_sub_resource.aspectMask	  = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	depth_sub_resource.baseArrayLayer = 0;
	depth_sub_resource.baseMipLevel	  = 0;
	depth_sub_resource.layerCount	  = 1;
	depth_sub_resource.levelCount	  = 1;

	VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	view_info.pNext			   = nullptr;
	view_info.flags			   = 0;
	view_info.image			   = VK_NULL_HANDLE;
	view_info.viewType		   = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format		   = VK_FORMAT_D24_UNORM_S8_UINT;
	view_info.components.r	   = VK_COMPONENT_SWIZZLE_R;
	view_info.components.g	   = VK_COMPONENT_SWIZZLE_G;
	view_info.components.b	   = VK_COMPONENT_SWIZZLE_B;
	view_info.components.a	   = VK_COMPONENT_SWIZZLE_A;
	view_info.subresourceRange = depth_sub_resource;

	depth_buffer = device->create_image("engine depth buffer", depth_info, view_info);
}

void vulkan_swapchain::destroy_swapchain() {
	gpu->wait_till_idle();
	vkDestroySwapchainKHR(gpu->get_logical_device(), swapchain, nullptr);
	for (u32 i = 0; i < view.size(); i++) {
		vkDestroyImageView(gpu->get_logical_device(), view[i], nullptr);
	}
	gpu->free(depth_buffer);
}

VkFormat vulkan_swapchain::get_swapchain_format() {
	return format.format;
}