#include "vulkan_driver.hpp"
#include "../../types.hpp"
#include "../../defines.hpp"
#include "../../logger.hpp"
#include "../../window/window.hpp"
#include "vulkan_defines.hpp"
#include <vector>
#include <cstring>

const char* ENGINE_NAME = "Void";
std::vector<const char *> requested_layers {
#if defined(DEBUG)
  "VK_LAYER_KHRONOS_validation",
#endif
};

#if defined(DEBUG)
static VkBool32 debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
               VkDebugUtilsMessageTypeFlagsEXT type,
               const VkDebugUtilsMessengerCallbackDataEXT *p_data,
               void *p_userdata) {
  std::string debug_string("Message Type : ");
  switch (type) {
  case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
    debug_string.append("General\n");
    break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
    debug_string.append("Validation\n");
    break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
    debug_string.append("Performance\n");
    break;
  case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
    debug_string.append("Address Binding\n");
    break;
  }

  debug_string.append("Error Type : ");
  switch (severity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    debug_string.append("Verbose\n");
    debug_string.append(p_data->pMessage);
    TRACE << debug_string << "\n";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    debug_string.append("Info\n");
    debug_string.append(p_data->pMessage);
    TRACE << debug_string << "\n";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    debug_string.append("Warning\n");
    debug_string.append(p_data->pMessage);
    WARN << debug_string << "\n";
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    debug_string.append("Error\n");
    debug_string.append(p_data->pMessage);
    FATAL << debug_string << "\n";
    break;
  default:
    break;
  }

  return VK_FALSE;
}
#endif

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

  if (requested_layers.size() != supported_layer_count) {
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
#if defined(DEBUG)
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
  extensions.push_back("VK_KHR_portability_enumeration");

  return extensions;
}

vulkan_driver::vulkan_driver(std::shared_ptr<window> p_window)
    : renderer(p_window) {
  create_instance();
  set_up_validation();
  pick_physical_device();
  populate_queue_info();
  create_logical_device();
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

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = nullptr;
  app_info.pApplicationName = ENGINE_NAME;
  app_info.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
  app_info.pEngineName = "libVoid";
  app_info.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
  app_info.apiVersion = VK_API_VERSION_1_4;

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

  return 0;
}

u32 vulkan_driver::set_up_validation() {
#if defined(DEBUG)
  VkResult result{};

  VkDebugUtilsMessengerCreateInfoEXT create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.pNext = nullptr;
  create_info.flags = 0;

  // enabling all messages
  create_info.messageSeverity = ~(0x0);
  create_info.messageType = ~(0x0);

  create_info.pfnUserCallback = debug_callback;
  create_info.pUserData = nullptr;

  vkCreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &debugger);

#endif
  return 0;
}

u32 vulkan_driver::pick_physical_device() {
  u32 device_count = 0;
  VkResult result =
      vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
  VALIDATE_RESULT(result, "failed to retrive device count");

  std::vector<VkPhysicalDevice> candidates(device_count);
  result =
      vkEnumeratePhysicalDevices(instance, &device_count, candidates.data());
  VALIDATE_RESULT(result, "failed to retrive devices");

  gpu = VK_NULL_HANDLE;
  for (auto device : candidates) {
    u32 queue_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_property(queue_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_count, queue_property.data());

    u32 family_bits = 0x0;
    // should be 0x7, which is all three family
    constexpr u32 needed_family =
      (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
    for (auto property : queue_property) {
      family_bits = family_bits | (property.queueFlags & needed_family);
    }

    if (nullptr == gpu) {
      gpu = device;
    }

    if (needed_family != family_bits) {
      continue;
    }

    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);

    if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == device_properties.deviceType) {
      gpu = device;
    }
  }

  VkPhysicalDeviceProperties picked_device;
  vkGetPhysicalDeviceProperties(gpu, &picked_device);

  TRACE << "Using : " << picked_device.deviceName << "\n";
  VERIFY_PTR(gpu);
  return 0;
}

u32 vulkan_driver::create_logical_device() {
  std::vector<VkDeviceQueueCreateInfo> queue_list;

  u32 size = queue_info.size();
  std::vector<f32> priority_list(size, 1.0f);
      
  std::unordered_map<u32, u32> queue_list_to_index;
  u32 queue_index = 0;
  for (auto [type, index] : queue_info) {
    if (queue_list_to_index.find(index) != queue_list_to_index.end()) {
      u32 lookup = queue_list_to_index[index];
      queue_list[lookup].queueCount++;
      continue;
    }

    VkDeviceQueueCreateInfo property{};
    property.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    property.queueFamilyIndex = index;
    property.queueCount = 1;
    property.pQueuePriorities = priority_list.data();

    queue_list.push_back(property);
    queue_index++;
  }

  VkPhysicalDeviceFeatures feature_list{};
  std::vector<const char*> extensions = request_extensions();
  
  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pNext = nullptr;
  create_info.flags = 0; // unused
  create_info.pQueueCreateInfos = queue_list.data();
  create_info.queueCreateInfoCount = static_cast<u32>(queue_list.size());
  create_info.enabledLayerCount = static_cast<u32>(requested_layers.size());
  create_info.ppEnabledLayerNames = requested_layers.data();
  create_info.enabledExtensionCount = static_cast<u32>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();
  create_info.pEnabledFeatures = nullptr;

  VkResult result = vkCreateDevice(gpu, &create_info, nullptr, &device);
  VALIDATE_RESULT(result, "failed to create device");
  return 0;
}

u32 vulkan_driver::populate_queue_info() {
  u32 queue_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_property(queue_count);
  vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_count,
                                           queue_property.data());

  u32 index = 0;
  for (auto prop : queue_property) {
    VkQueueFlags flags = prop.queueFlags;

    if (VK_QUEUE_GRAPHICS_BIT & flags) {
      queue_info[VK_QUEUE_GRAPHICS_BIT] = index; 
    }

    if (VK_QUEUE_COMPUTE_BIT & flags) {
      queue_info[VK_QUEUE_COMPUTE_BIT] = index;   
    }

    if (VK_QUEUE_TRANSFER_BIT & flags) {
      queue_info[VK_QUEUE_TRANSFER_BIT] = index;
    }

    index++;
  }
  return 0;
}

