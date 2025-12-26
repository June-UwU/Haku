#include "vulkan_driver.hpp"
#include "../../types.hpp"
#include "../../logger.hpp"
#include "../../window/window.hpp"
#include "vulkan_defines.hpp"
#include <ranges>
#include <vector>

const char* ENGINE_NAME = "Void";
std::vector<const char *> requested_layers{
    "VK_LAYER_KHRONOS_validation",
};

std::vector<const char *> request_layers() {
  u32 layer_count{};
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  u32 supported_layer_count = 0; 
  for (auto name : requested_layers) {
    for (auto layers : available_layers) {
      if (0 == strcmp(name, layers.layerName)) {
	supported_layer_count++;
      }
    }
  }

  if (layer_count != supported_layer_count) {
    FATAL << "not all requested layers are supported!";
    std::abort();
  }

  return requested_layers;
}

std::vector<const char *> request_extensions() {
  u32 extension_count{};
  const char **glfw_extension =
      glfwGetRequiredInstanceExtensions(&extension_count);

  std::vector<const char *> extensions(glfw_extension,
                                       glfw_extension + extension_count);
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  return extensions;
}  

vulkan_driver::vulkan_driver(std::shared_ptr<window> p_window)
    : renderer(p_window) {
  create_instance();
}

vulkan_driver::~vulkan_driver() {
  vkDestroyInstance(instance, nullptr);
}

u32 vulkan_driver::create_instance() {
  VkResult local_result{};

  u32 supported_version = 0;
  local_result = vkEnumerateInstanceVersion(&supported_version);
  VALIDATE_RESULT(local_result, "vulkan is not supported");
 
  TRACE << "Vulkan Version " << VK_API_VERSION_MAJOR(supported_version) << "."
        << VK_API_VERSION_MINOR(supported_version) << "\n";

  if (4 > VK_API_VERSION_MINOR(supported_version)) {
    FATAL << "vulkan version is too old";
    std::abort();
  }    
  
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = nullptr;
  app_info.pApplicationName = ENGINE_NAME;
  app_info.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
  app_info.pEngineName = "libVoid";
  app_info.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
  app_info.apiVersion = supported_version;
  
  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pNext = nullptr;

  create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  create_info.pApplicationInfo = &app_info;
  
  std::vector<const char*> validated_layer_list = request_layers();
  create_info.enabledLayerCount = validated_layer_list.size();
  create_info.ppEnabledLayerNames = validated_layer_list.data();

  std::vector<const char*> extension_list = request_extensions(); 
  create_info.enabledExtensionCount = (u32) extension_list.size();
  create_info.ppEnabledExtensionNames = extension_list.data();
  
  local_result = vkCreateInstance(&create_info, nullptr, &instance);
  VALIDATE_RESULT(local_result, "failed to create vulkan instance");
}
