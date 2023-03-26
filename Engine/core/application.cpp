#include "application.hpp"
#include "logger.hpp"
#include "platform/platform.hpp"

static bool RUNNING;

HakuEngine::HakuEngine()
{
  RUNNING = true;
  initializeLogger();
}

HakuEngine::~HakuEngine()
{
  shutdownWindow();
  shutdownLogger();
}

const s8 HakuEngine::initialize() const
{
  LOG_TRACE("Hellow... UwU");
  
  initializeWindow();

  return H_OK;
}

const s8 HakuEngine::exec()
{
  while(RUNNING)
  {
    processEvents();
  }
  return H_OK;
}
