#include "vulkan_context.hpp"
#include <GLFW/glfw3.h>

std::vector<const char*> request_layers() {
    std::vector<const char*> requested_layer {
        "VK_LAYER_KHRONOS_validation"
    };

    u32 layer_count;
    vkEnumerateInstanceExtensionProperties(&layer_count,nullptr);
    
    std::vector<VkLayerProperties> available_layer(layer_count);
    vkEnumerateDeviceExtensionProperties(&layer_count,available_layer.data());

    std::vector<const char*> layers_available;

    for(auto layer: available_layer) {
        for(auto request: requested_layer) {
            if(0 == strcmp(layer.layerName,request)) {
                layers_available.push_back(layer.layerName);
            }
        }
    }

    return layers_available;
}

std::vector<const char*> request_extensions() {
    u32			 glfw_extension_count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char*> extensions;

    for(u32 i = 0; i < glfw_extensionCount; i++) {
        extension.push_back(glfw_extensions[i]);
    }

    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

vulkan_context::vulkan_context() {
    VkApplicationInfo application_info{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    application_info.pNext = nullptr;
    applicaton_info.pApplicationName = "Haku";
    application_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
    application_info.pEngineName = "Haku Engine";
    application_info.engineVersion = VK_MAKE_VERSION(1,0,0);
    application_info.apiVersion = VK_MAKE_VERSION(1,2,0);

    auto enable_layer = request_layers();
    auto enable_extension = request_extensions();

    VkInstanceCreateInfo instance_create_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instance_create_info.pNext = nullptr;
    instance_create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledLayerCount = enable_layer.size();
    instance_create_info.ppEnabledLayerNames = enable_layer.data();
    instance_create_info.enabledExtensionCount = enable_extension.size();
    instance_create_info.ppEnabledExtensionNames = enable_extension.data();

    VkResult result = vkCreateInstance(&instance_create_info,nullptr,&instance);
    VK_ASSERT(result,"failed to create an instance..!");
}