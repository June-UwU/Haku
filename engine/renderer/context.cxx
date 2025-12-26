#include "context.hpp"
#include "vulkan/vulkan_driver.hpp"

context::context(render_api api, std::shared_ptr<window> p_wnd)
  :api(api)    {
  switch (api) {
  case VULKAN_API:
    p_renderer = std::make_unique<vulkan_driver>(p_wnd);
    break;
  }
}


context::~context() {
  p_window = nullptr;
}  

