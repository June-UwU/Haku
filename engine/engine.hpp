#pragma once
#include "types.hpp"
#include <string>

typedef struct engine {
  std::string name;
}engine;

engine* initialize_engine(s32 argc, char **argv);
void destroy_engine(engine* p_engine);

