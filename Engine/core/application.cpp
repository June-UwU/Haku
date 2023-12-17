#include "application.hpp"
#include "logger.hpp"
#include "platform/platform.hpp"
#include "event.hpp"

static bool RUNNING;

Status exitApplication(Event event) {
  RUNNING = false;
  return OK;
}

HakuEngine::HakuEngine()
{
  RUNNING = true;
  Status status = initializeLogger();
  ASSERT_OK(status)
}

HakuEngine::~HakuEngine() {
  shutdownWindow();
  shutdownLogger();
}

const Status HakuEngine::initialize() const {
  LOG_TRACE("Hellow... UwU");
  
  Status status = initializeWindow();
  ASSERT_OK(status)

  status = initializeEventSystem();
  ASSERT_OK(status)

  status = registerEvent(EVENT_WINDOW_CLOSED, exitApplication);
  ASSERT_OK(status)

  return OK;
}

const Status HakuEngine::exec() {
  while(RUNNING) {
    processEvents();
    Status status = handleEvents();
    ASSERT_OK(status)
    
  }
  return OK;
}
