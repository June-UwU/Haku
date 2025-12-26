#include "context.hpp"

context::context(render_api api, std::shared_ptr<window> p_wnd)
  :api(api)    {
  switch (api) {
  case VULKAN_API:
    break;
  }
}


context::~context() {
  p_window = nullptr;
}  

