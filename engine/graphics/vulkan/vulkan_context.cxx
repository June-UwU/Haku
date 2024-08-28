#include "vulkan_context.hpp"
#include "platform/window.hpp"
#include <GLFW/glfw3.h>
#include <vector>

void print_available_extensions() {
	u32		 available_layer_count = 0;
	VkResult result				   = vkEnumerateInstanceExtensionProperties(nullptr, &available_layer_count, nullptr);
	VK_ASSERT(result, "failed to enumerate extension count..!!");

	std::vector<VkExtensionProperties> properties(available_layer_count);
	result = vkEnumerateInstanceExtensionProperties(nullptr, &available_layer_count, properties.data());
	VK_ASSERT(result, "failed to enumerate extensions..!!");

	TRACE << "Available Extensions : \n";

	for (auto& extensions : properties) {
		TRACE << extensions.extensionName << "\n";
	}
}

std::vector<const char*> request_layers(std::vector<const char*>& requested_layers) {
#if defined(NDEBUG)
	return {};
#else
	u32 layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> available_layer(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layer.data());

	std::vector<const char*> layers_available;

	for (auto& layer : available_layer) {
		for (auto request : requested_layers) {
			if (0 == strcmp(layer.layerName, request)) {
				layers_available.push_back(request);
			}
		}
	}

	return layers_available;
#endif
}

std::vector<const char*> request_extensions() {
	u32			 glfw_extension_count = 0;
	const char** glfw_extensions	  = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector<const char*> extensions;

	for (u32 i = 0; i < glfw_extension_count; i++) {
		extensions.push_back(glfw_extensions[i]);
	}

	extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
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
#if defined(NDEBUG)
#else
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
#endif
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

void setup_debug_instance(VkInstance instance, VkDebugUtilsMessengerEXT* messenger) {
#if defined(_DEBUG)
	VkDebugUtilsMessengerCreateInfoEXT create_info{};
	create_info.sType			= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType		= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debug_callback;
	create_info.pUserData		= nullptr; // Optional

	VkResult messenger_status = create_debug_util_messenger(instance, &create_info, nullptr, messenger);
	VK_ASSERT(messenger_status, "failed to create debug util messanger");
#else
#endif
}

vulkan_context::vulkan_context() {
	VkApplicationInfo application_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	application_info.pNext				= nullptr;
	application_info.pApplicationName	= "Haku";
	application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	application_info.pEngineName		= "Haku Engine";
	application_info.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
	application_info.apiVersion			= VK_MAKE_VERSION(1, 2, 0);

	std::vector<const char*> requested_layer{
		"VK_LAYER_KHRONOS_validation"
	};

	auto enable_layer = request_layers(requested_layer);
	for (auto val : enable_layer) {
		TRACE << val << "\n";
	}

	auto enable_extension = request_extensions();

	VkInstanceCreateInfo instance_create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instance_create_info.pNext = nullptr;
	instance_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	instance_create_info.pApplicationInfo		 = &application_info;
	instance_create_info.enabledLayerCount		 = enable_layer.size();
	instance_create_info.ppEnabledLayerNames	 = enable_layer.data();
	instance_create_info.enabledExtensionCount	 = enable_extension.size();
	instance_create_info.ppEnabledExtensionNames = enable_extension.data();

	//	print_available_extensions();

	VkResult result = vkCreateInstance(&instance_create_info, nullptr, &instance);
	VK_ASSERT(result, "failed to create an instance..!");
	setup_debug_instance(instance, &debug_messenger);

	VkResult surface_created = glfwCreateWindowSurface(instance, get_window(), nullptr, &surface);
	VK_ASSERT(surface_created, "failed to create window surface..!!");

	device			   = std::make_unique<vulkan_device>(instance);
}

vulkan_context::~vulkan_context() {
	device			   = nullptr;
	vkDestroySurfaceKHR(instance, surface, nullptr);
	destroy_debug_util_messenger(instance, debug_messenger, nullptr);
	vkDestroyInstance(instance, nullptr);
}

VkInstance vulkan_context::get_instance() {
	return instance;
}
