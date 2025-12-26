#pragma once
#include "../../types.hpp"
#include <vulkan/vulkan.h>
#include "../render_api.hpp"

class vulkan_driver : public renderer {
public:
  vulkan_driver(std::shared_ptr<window> p_window);
  ~vulkan_driver();

private:
  u32 create_instance();

private:
  VkInstance instance;
};
