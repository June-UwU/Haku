#include "vulkan_driver.hpp"
#include "../../types.hpp"
#include "../../logger.hpp"
#include "../../window/window.hpp"
#include "vulkan_defines.hpp"
#include <ranges>
#include <vector>
#define ENABLE_VULKAN_VALIDATION true

const std::vector<char const *> validation_layers = {
    "VK_LAYER_KHRONOS_validation"};

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debug_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *) {

  if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError || severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
    FATAL << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
  }
  return VK_FALSE;
}

std::vector<const char *> get_required_extensions() {
  uint32_t glfw_extension_count = 0;
  auto     glfw_extensions     = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  std::vector extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
  if (ENABLE_VULKAN_VALIDATION) {
    extensions.push_back(vk::EXTDebugUtilsExtensionName);
  }

  return extensions;
}

vulkan_driver::vulkan_driver(std::shared_ptr<window> p_window)
    : renderer(p_window) {
}


vulkan_driver::~vulkan_driver() {

}  

u32 vulkan_driver::create_instance() {
  u32 supported_version = 0;

  VkResult local_result = vkEnumerateInstanceVersion(&supported_version);
  VALIDATE_RESULT(local_result, "vulkan is not supported");
  TRACE << "Vulkan Version : " << VK_API_VERSION_MAJOR(supported_version)
	<<"." << VK_API_VERSION_MINOR(supported_version) << "\n";

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  
  
  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pNext = nullptr;

  create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  
}
