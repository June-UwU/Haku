#pragma once
#include "types.hpp"
#include "defines.hpp"
#include <string>
#include <memory>


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
  std::unique_ptr<window> main_window;  
};  

