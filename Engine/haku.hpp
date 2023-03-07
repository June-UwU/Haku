#pragma once

#define HAKU_API

#include "types.hpp"
#include "defines.hpp"
#include "logger.hpp"
#include "application.hpp"

extern s8 createGame();

s32 main(s32 argc, char** argv)
{ 
  HakuEngine* Engine = new HakuEngine(); 
  
  s8 ret_val = Engine->initialize();
  if(H_OK != ret_val)
  {
    LOG_WARN("Engine initialization  faliure...");
    return ret_val;
  }
  createGame();

  return Engine->exec();
}
