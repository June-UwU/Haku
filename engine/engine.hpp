#pragma once
#include "types.hpp"
#include "defines.hpp"
#include <string>
#include <memory>
#include "renderer/context.hpp"

class window;
class engine {
public:
  engine(s32 argc, char **argv);
  ~engine();

  DISABLE_COPY(engine);
  DISABLE_MOVE(engine);

  s32 run_engine();

private:
  bool running;
  std::string name;
  std::shared_ptr<window> p_main_window;
  std::unique_ptr<context> p_gpu_context;
};

