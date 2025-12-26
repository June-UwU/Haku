#pragma once
#include "../types.hpp"
#include "render_api.hpp"
#include <memory>

typedef enum render_api {
  VULKAN_API = 0x0,
} render_api;

class window;
class context {
public:
  context(render_api api, std::shared_ptr<window> p_wnd);
  ~context();
private:
  render_api api;
  std::shared_ptr<window> p_window;
  std::unique_ptr<renderer> p_renderer;
};  
