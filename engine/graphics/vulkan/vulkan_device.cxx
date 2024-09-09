#include "vulkan_device.hpp"
#include "vulkan_results.hpp"
#include "defines.hpp"
#include <vector>
#include <map>
#include <set>
#include "vulkan_command_allocator.hpp"

bool check_device_extension_support(VkPhysicalDevice gpu, std::vector<const char*>& device_extensions) {
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extension_count, available_extensions.data());

	std::set<std::string> required_extension(device_extensions.begin(), device_extensions.end());

	for (const auto& extension : available_extensions) {
		required_extension.erase(extension.extensionName);
	}

	return required_extension.empty();
}

u32 rate_gpu(VkPhysicalDevice& device) {
	constexpr const u32 GPU_NOT_COMPLAINT = 0;
	u32					gpu_score		  = GPU_NOT_COMPLAINT;

	const std::vector<const char*> device_extension{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures   device_features;
	vkGetPhysicalDeviceProperties(device, &device_properties);
	vkGetPhysicalDeviceFeatures(device, &device_features);

	std::cout << device_properties.deviceName << "(" << device_properties.driverVersion << ") : ";

	std::vector<const char*> request_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	if (false == check_device_extension_support(device, request_extensions)) {
		return GPU_NOT_COMPLAINT;
	}

	if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == device_properties.deviceType) {
		gpu_score += 1000;
	}

	gpu_score += device_properties.limits.maxImageDimension2D;

	if (false == device_features.geometryShader) {
		return GPU_NOT_COMPLAINT;
	}

	return gpu_score;
}

vulkan_device::vulkan_device(VkInstance instance, VkSurfaceKHR surface) {
	u32		 device_count = 0;
	VkResult device_enumerated =
		vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
	VK_ASSERT(device_enumerated, "failed to enumerate devices");
	TRACE << device_count << " Potential Devices Found.\n";
	ASSERT(0 != device_count, "no complaint physical device exists...!!");

	std::vector<VkPhysicalDevice> devices(device_count);
	device_enumerated = vkEnumeratePhysicalDevices(instance, &device_count, devices.data());
	VK_ASSERT(device_enumerated, "failed to enumerate devices");

	std::multimap<u32, VkPhysicalDevice> gpu_candidates;

	for (auto& gpu : devices) {
		u32 score = rate_gpu(gpu);
		TRACE << score << "\n";
		gpu_candidates.insert(std::make_pair(score, gpu));
	}

	if (0 < gpu_candidates.rbegin()->first) {
		physical_device = gpu_candidates.rbegin()->second;
	} else {
		ERROR << "failed to find a complaint physical device..!!";
		exit(-1);
	}

	set_queue_index(surface);

	constexpr const u32		TO_BE_SET = -1;
	VkDeviceQueueCreateInfo queue_info{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	queue_info.pNext			= nullptr;
	queue_info.flags			= 0;
	queue_info.queueFamilyIndex = TO_BE_SET;
	queue_info.queueCount		= -1;
	queue_info.queueFamilyIndex = -1;

	const u32							 REQUIRED_QUEUE_COUNT = queue_usage.size();
	std::vector<VkDeviceQueueCreateInfo> required_queues(REQUIRED_QUEUE_COUNT);
	u32									 index = 0;

	std::vector<f32> priority_vec(REQUIRED_QUEUE_COUNT, 1.0f);
	for (auto& [family, count] : queue_usage) {
		queue_info.queueFamilyIndex = family;
		queue_info.queueCount		= count;
		queue_info.pQueuePriorities = priority_vec.data();

		required_queues[index] = queue_info;
		index++;
	}

	const std::vector<const char*> device_extensions{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDeviceCreateInfo device_create_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	device_create_info.pNext				   = nullptr;
	device_create_info.flags				   = 0;
	device_create_info.queueCreateInfoCount	   = static_cast<u32>(required_queues.size());
	device_create_info.pQueueCreateInfos	   = required_queues.data();
	device_create_info.enabledExtensionCount   = static_cast<u32>(device_extensions.size());
	device_create_info.ppEnabledExtensionNames = device_extensions.data();
	device_create_info.pEnabledFeatures		   = nullptr;

	VkResult logical_device_created = vkCreateDevice(physical_device, &device_create_info, nullptr, &logical_device);
	VK_ASSERT(logical_device_created, "failed to create logical device");

	initialize_device_queues();
	primary_allocator  = std::make_unique<vulkan_command_allocator>(logical_device, graphics_queue_index, false);
	transfer_allocator = std::make_unique<vulkan_command_allocator>(logical_device, transfer_queue_index, false);
	gpu_allocator	   = std::make_unique<gpu_memory_allocator>(physical_device, logical_device, instance);
}

vulkan_device::~vulkan_device() {
	wait_till_idle();
	gpu_allocator	   = nullptr;
	transfer_allocator = nullptr;
	primary_allocator  = nullptr;
	vkDestroyDevice(logical_device, nullptr);
}

VkDevice vulkan_device::get_logical_device() {
	return logical_device;
}

void vulkan_device::wait_till_idle() {
	VkResult result = vkDeviceWaitIdle(logical_device);
	VK_ASSERT(result, "failed to wait on device idle..!!");
}

surface_capabilities vulkan_device::get_surface_device_capabilities(VkSurfaceKHR surface) {
	surface_capabilities capabilites{};

	VkResult surface_capabilities_query = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilites.surface_capabilities);
	VK_ASSERT(surface_capabilities_query, "failed to query surface capabilities..!!");

	u32		 surface_format_count  = 0;
	VkResult surface_formats_query = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, nullptr);
	VK_ASSERT(surface_formats_query, "failed to get surface format count..!!");

	capabilites.surface_formats.resize(surface_format_count);
	surface_formats_query = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &surface_format_count, capabilites.surface_formats.data());
	VK_ASSERT(surface_formats_query, "failed to get surface formats..!!");

	u32		 present_mode_count = 0;
	VkResult present_mode_query = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
	VK_ASSERT(present_mode_query, "failed to get present mode count..!!");

	capabilites.present_modes.resize(present_mode_count);
	present_mode_query = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, capabilites.present_modes.data());
	VK_ASSERT(present_mode_query, "failed to get present modes..!!");

	return capabilites;
}

bool vulkan_device::is_shared_present_queue() {
	std::vector<u32> family_index{
		graphics_queue_index,
		transfer_queue_index,
		compute_queue_index
	};

	for (const auto index : family_index) {
		if (index == present_queue_index) {
			return true;
		}
	}

	return false;
}

u32 vulkan_device::get_graphics_queue_index() {
	return graphics_queue_index;
}

u32 vulkan_device::get_present_queue_index() {
	return present_queue_index;
}

u32 vulkan_device::get_compute_queue_index() {
	return compute_queue_index;
}

u32 vulkan_device::get_transfer_queue_index() {
	return transfer_queue_index;
}

vulkan_images* vulkan_device::create_image(std::string name, VkImageCreateInfo& image_info, VkImageViewCreateInfo& view_info) {
	return gpu_allocator->create_image(name, logical_device, image_info, view_info);
}

void vulkan_device::free(vulkan_images* image) {
	gpu_allocator->free(image, logical_device);
}

void vulkan_device::submit_commands(VkSubmitInfo& submit_info, VkFence signal) {
	VkResult result = vkQueueSubmit(graphics_queue, 1, &submit_info, signal);
	VK_ASSERT(result, "failed to submit queue");
}

void vulkan_device::present(VkPresentInfoKHR& present_info) {
	VkResult result = vkQueuePresentKHR(present_queue, &present_info);
	VK_ASSERT(result, "failed to presnt the recorded queue");
}

VkCommandBuffer vulkan_device::get_graphics_command_buffer(bool is_primary) {
	return primary_allocator->allocate_command_buffer(is_primary);
}

void vulkan_device::set_queue_index(VkSurfaceKHR surface) {
	u32 queue_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, nullptr);

	std::vector<VkQueueFamilyProperties> queues(queue_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, queues.data());

	u32 index = 0;
	for (auto& queue : queues) {
		if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			graphics_queue_index = index;
			queue_usage[index]++;
		}

		VkBool32 present_support		= VK_FALSE;
		auto	 present_support_result = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, index, surface, &present_support);
		VK_ASSERT(present_support_result, "failed to query physical device...!!");

		if ((VK_TRUE == present_support) && (-1 == present_queue_index)) {
			present_queue_index = index;
			queue_usage[index]++;
		}

		if ((queue.queueFlags & VK_QUEUE_COMPUTE_BIT) && (0 == queue_usage[index])) {
			compute_queue_index = index;
			queue_usage[index]++;
		}

		if ((queue.queueFlags & VK_QUEUE_TRANSFER_BIT) && (0 == queue_usage[index])) {
			transfer_queue_index = index;
			queue_usage[index]++;
		}

		index++;
	}

	constexpr const u32 INVALID_INDEX = -1;
	if (INVALID_INDEX == compute_queue_index) {
		index = 0;
		for (auto& queue : queues) {
			if ((queue.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
				compute_queue_index = index;
				queue_usage[index]++;
			}

			index++;
		}
	}

	if (INVALID_INDEX == transfer_queue_index) {
		index = 0;
		for (auto& queue : queues) {
			if ((queue.queueFlags & VK_QUEUE_TRANSFER_BIT)) {
				transfer_queue_index = index;
				queue_usage[index]++;
			}

			index++;
		}
	}

	ASSERT(INVALID_INDEX != graphics_queue_index, "failed to find graphics queue index...!!");
	ASSERT(INVALID_INDEX != compute_queue_index, "failed to find transfer queue index...!!");
	ASSERT(INVALID_INDEX != transfer_queue_index, "failed to find transfer queue index...!!");
	ASSERT(INVALID_INDEX != present_queue_index, "failed to find transfer queue index...!!");
	TRACE << "Graphics Queue : " << graphics_queue_index << "\n"
		  << "Compute Queue : " << compute_queue_index << "\n"
		  << "Transfer Queue : " << transfer_queue_index << "\n"
		  << "Present Queue : " << present_queue_index << "\n";
}

void vulkan_device::initialize_device_queues() {
	TRACE << "initializing device command queues...\n";
	vkGetDeviceQueue(logical_device, graphics_queue_index, 0, &graphics_queue);
	vkGetDeviceQueue(logical_device, compute_queue_index, 0, &compute_queue);
	vkGetDeviceQueue(logical_device, transfer_queue_index, 0, &transfer_queue);
	vkGetDeviceQueue(logical_device, present_queue_index, 0, &present_queue);
}