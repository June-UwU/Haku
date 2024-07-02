#include "device.hpp"
#include "defines.hpp"
#include "platform/window.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <set>
#include <map>
#include <cstring>


VkInstance				 instance;
VkDebugUtilsMessengerEXT debug_messenger;
VkPhysicalDevice		 physical_device;
VkDevice				 logical_gpu;
VkQueue					 graphics_queue;
VkQueue					 present_queue;
VkSurfaceKHR			 render_surface;


VkDevice get_device() {
	return logical_gpu;
}

VkQueue get_graphics_queue() {
	return graphics_queue;
}

VkQueue get_present_queue() {
	return present_queue;
}

VkPhysicalDevice get_physical_device() {
	return physical_device;
}

VkInstance get_instance() {
	return instance;
}

VkSurfaceKHR get_render_surface() {
	return render_surface;
}

const std::vector<const char*> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool check_device_extension_support(VkPhysicalDevice gpu) {
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

VkResult create_debug_util_messenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		WARN << "Vulkan Debug Extensions are not available..!!\n";
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void destroy_debug_util_messenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		severity,
	VkDebugUtilsMessageTypeFlagsEXT				type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void*										user_data) {
	switch (type) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
		TRACE << ANSI_COLOR_RESET << "general info : ";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
		TRACE << ANSI_COLOR_RESET << "violation of specifications : ";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
		TRACE << ANSI_COLOR_RESET << "potential non-optimal usage of vulkan : ";
	} break;
	}

	switch (severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
		TRACE << ANSI_COLOR_RESET << callback_data->pMessage << "\n";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
		TRACE << callback_data->pMessage << "\n";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
		WARN << callback_data->pMessage << "\n";
	} break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
		ERROR << callback_data->pMessage << "\n";
	} break;
	}

	return VK_FALSE;
}

void print_available_vulkan_extensions(std::vector<VkExtensionProperties>& extensions) {
	TRACE << "available extensions:\n";

	for (const auto& extension : extensions) {
		TRACE << extension.extensionName << " " << extension.specVersion << ".0 \n";
	}
}

void print_available_layers(std::vector<VkLayerProperties>& avaliable_layers) {
	TRACE << "available layers : \n";
	for (const auto& layers : avaliable_layers) {
		TRACE << layers.layerName << "\nVersion : " << layers.implementationVersion << "." << layers.specVersion << "\nDetails :  " << layers.description << "\n\n";
	}
}

bool check_validation_layer_support(const std::vector<const char*>& validation_layers) {
	u32 layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> avaliable_layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, avaliable_layers.data());

	for (const char* layer_name : validation_layers) {
		bool layerFound = false;

		for (const auto& layer_properties : avaliable_layers) {
			if (strcmp(layer_name, layer_properties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

swap_chain_support query_swapchain_support(VkPhysicalDevice gpu) {
	swap_chain_support details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, get_render_surface(), &details.capabilities);

	u32 format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, get_render_surface(), &format_count, nullptr);

	if (format_count != 0) {
		details.formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, get_render_surface(), &format_count, details.formats.data());
	}

	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, get_render_surface(), &present_mode_count, nullptr);

	if (present_mode_count != 0) {
		details.present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, get_render_surface(), &present_mode_count, details.present_modes.data());
	}

	return details;
}

s32 rate_gpu(VkPhysicalDevice& gpu) {
	constexpr const s32 GPU_NOT_COMPLAINT = 0;
	s32					gpu_score		  = GPU_NOT_COMPLAINT;

	const std::vector<const char*> device_extension{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures   device_features;
	vkGetPhysicalDeviceProperties(gpu, &device_properties);
	vkGetPhysicalDeviceFeatures(gpu, &device_features);

	if (false == check_device_extension_support(gpu)) {
		return GPU_NOT_COMPLAINT;
	}

	swap_chain_support support_capabilities = query_swapchain_support(gpu);
	if (support_capabilities.formats.empty() || support_capabilities.present_modes.empty()) {
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

bool create_vulkan_instance() {
	VkApplicationInfo app_info{};
	app_info.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName	= "Haku";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName		= "Haku Engine";
	app_info.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion			= VK_API_VERSION_1_0;

	VkInstanceCreateInfo instance_create_info{};
	instance_create_info.sType			  = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create_info.pApplicationInfo = &app_info;

	u32			 glfw_extensionCount = 0;
	const char** glfw_extensions;
	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensionCount);

	std::vector<const char*> required_extensions;

	for (u32 i = 0; i < glfw_extensionCount; i++) {
		required_extensions.emplace_back(glfw_extensions[i]);
	}

	const std::vector<const char*> validation_layers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
	const bool enable_validation_layers = false;
#else
	const bool enable_validation_layers = true;
#endif
	if (enable_validation_layers && false == check_validation_layer_support(validation_layers)) {
		ERROR << "validation layer requested, but not available!\n"
			  << ANSI_COLOR_RESET;
		return false;
	}

	if (enable_validation_layers) {
		instance_create_info.enabledLayerCount	 = static_cast<uint32_t>(validation_layers.size());
		instance_create_info.ppEnabledLayerNames = validation_layers.data();
	} else {
		instance_create_info.enabledLayerCount = 0;
	}

	required_extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	required_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	u32 extension_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

	instance_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	instance_create_info.enabledExtensionCount	 = (uint32_t)required_extensions.size();
	instance_create_info.ppEnabledExtensionNames = required_extensions.data();

	VkResult instance_result = vkCreateInstance(&instance_create_info, nullptr, &instance);
	ASSERT(VK_SUCCESS == instance_result, "vulkan instance creation failure");

	return true;
}

bool setup_debug_layer() {
#if defined(NDEBUG)
	return false;
#else
	VkDebugUtilsMessengerCreateInfoEXT create_info{};
	create_info.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType		= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debug_callback;
	create_info.pUserData		= nullptr; // Optional

	if (VK_SUCCESS != create_debug_util_messenger(get_instance(), &create_info, nullptr, &debug_messenger)) {
		return false;
	}

	return true;
#endif
}

bool choose_physical_device() {
	physical_device = VK_NULL_HANDLE;

	u32 device_count = 0;
	vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

	if (0 == device_count) {
		ERROR << "no GPU with vulkan support was found!\n";
		return false;
	}

	std::vector<VkPhysicalDevice> gpus(device_count);
	vkEnumeratePhysicalDevices(instance, &device_count, gpus.data());

	std::multimap<s32, VkPhysicalDevice> gpu_candidates;
	for (auto& gpu : gpus) {
		s32 score = rate_gpu(gpu);
		gpu_candidates.insert(std::make_pair(score, gpu));
	}

	if (gpu_candidates.rbegin()->first > 0) {
		physical_device = gpu_candidates.rbegin()->second;
	} else {
		ERROR << "no GPU with required features\n";
		return false;
	}

	gpu_queue queue_info = find_queue_families(physical_device);
	return queue_info.is_gpu_complaint();
}

bool create_logical_device() {
	gpu_queue queues = find_queue_families(get_physical_device());

	std::vector<VkDeviceQueueCreateInfo> queue_requests;
	std::set<u32>						 queue_family_index = { static_cast<u32>(queues.graphics_index), static_cast<u32>(queues.present_index) };

	f32 graphic_priority = 1.0f;
	for (u32 family : queue_family_index) {
		VkDeviceQueueCreateInfo queue_create_info{};
		queue_create_info.sType			   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = family;
		queue_create_info.queueCount	   = 1;
		queue_create_info.pQueuePriorities = &graphic_priority;

		queue_requests.push_back(queue_create_info);
	}

	VkPhysicalDeviceFeatures device_features{};

	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType				   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pQueueCreateInfos	   = queue_requests.data();
	device_create_info.queueCreateInfoCount	   = queue_requests.size();
	device_create_info.pEnabledFeatures		   = &device_features;
	device_create_info.enabledExtensionCount   = static_cast<u32>(device_extensions.size());
	device_create_info.ppEnabledExtensionNames = device_extensions.data();
	device_create_info.enabledLayerCount	   = 0;

	if (VK_SUCCESS != vkCreateDevice(get_physical_device(), &device_create_info, nullptr, &logical_gpu)) {
		ERROR << "failed to initialize logical gpu..!!\n";
		return false;
	}

	return true;
}

gpu_queue find_queue_families(VkPhysicalDevice device) {
	gpu_queue gpu_info;

	u32 queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(get_physical_device(), &queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(get_physical_device(), &queue_family_count, queue_families.data());

	s32 index = 0;
	for (const auto& family : queue_families) {
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			gpu_info.graphics_index = index;
		}

		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(get_physical_device(), index, render_surface, &present_support);

		if (present_support) {
			gpu_info.present_index = index;
		}

		if (gpu_info.is_gpu_complaint()) {
			break;
		}

		index++;
	}

	return gpu_info;
}

bool retrieve_gpu_queues(VkDevice gpu) {
	gpu_queue queue_info = find_queue_families(physical_device);

	vkGetDeviceQueue(gpu, queue_info.graphics_index, 0, &graphics_queue);
	vkGetDeviceQueue(gpu, queue_info.present_index, 0, &present_queue);

	return true;
}

bool initialize_device() {
	bool instance_created = create_vulkan_instance();
	ASSERT(instance_created, "instance created failure");

	bool debug_layer_enable = setup_debug_layer();
	if (debug_layer_enable) {
		TRACE << "Debugging Support Enabled\n";
	}
		
	ASSERT(VK_SUCCESS == glfwCreateWindowSurface(get_instance(), get_window(), nullptr, &render_surface), "failed to create a render surface");

	bool found_gpu = choose_physical_device();
	ASSERT(found_gpu, "failed to choose gpu\n");

	bool gpu_configured = create_logical_device();
	ASSERT(gpu_configured, "failed to configure gpu\n");

	bool gpu_queue_initialized = retrieve_gpu_queues(get_device());
	ASSERT(gpu_queue_initialized, "failed to retrieve gpu queues\n");

	return true;
}

void destroy_device() {
	vkDestroySurfaceKHR(get_instance(), render_surface, nullptr);
	vkDestroyDevice(get_device(), nullptr);
#ifndef NDEBUG
	destroy_debug_util_messenger(get_instance(), debug_messenger, nullptr);
#endif
	vkDestroyInstance(get_instance(), nullptr);
}
