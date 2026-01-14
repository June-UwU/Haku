#pragma once
#include "../../types.hpp"
#include <vulkan/vulkan.h>
#include "../render_api.hpp"
#include <unordered_map>

class vulkan_driver : public renderer {
public:
  vulkan_driver(std::shared_ptr<window> p_window);
  ~vulkan_driver();

private:
  u32 create_instance();
  u32 set_up_validation();
  u32 populate_queue_info();
  u32 pick_physical_device();
  u32 create_logical_device();
  
private:
  VkDevice device;
  VkInstance instance;
  VkPhysicalDevice gpu;
  std::unordered_map<u32, u32> queue_info;
#if defined(DEBUG)
  VkDebugUtilsMessengerEXT debugger;
#endif
};
