#include "application.hpp"
#include "logger.hpp"

HakuEngine::HakuEngine()
  :m_window{nullptr}
{
  initializeLogger();
}

HakuEngine::~HakuEngine()
{
  shutdownWindow(m_window);
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
  while(true);
}
