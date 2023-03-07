#include "application.hpp"
#include "logger.hpp"

HakuEngine::HakuEngine()
{
}


const s8 HakuEngine::initialize() const
{
  LOG_TRACE("Hellow... UwU");
  return H_OK;
}

const s8 HakuEngine::exec()
{
  while(true);
}
