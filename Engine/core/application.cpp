#include "application.hpp"
#include "logger.hpp"
#include "platform/platform.hpp"

static bool RUNNING;

HakuEngine::HakuEngine()
{
  RUNNING = true;
  Status status = initializeLogger();
  ASSERT_OK(status)
}

HakuEngine::~HakuEngine()
{
  shutdownWindow();
  shutdownLogger();
}

const Status HakuEngine::initialize() const
{
  LOG_TRACE("Hellow... UwU");
  
  Status status = initializeWindow();
  ASSERT_OK(status)
  
  return OK;
}

const Status HakuEngine::exec()
{
  while(RUNNING)
  {
    processEvents();
  }
  return OK;
}
