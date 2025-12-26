#pragma once
#include <memory>

class window;
class renderer {
public:
  renderer(std::shared_ptr<window> p_window) : p_window(p_window) {};
  virtual ~renderer() = default;
private:
  std::shared_ptr<window> p_window;
};  
