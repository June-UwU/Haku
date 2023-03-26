#include "platform.hpp"
#include "logger.hpp"

#ifdef HAKU_LINUX
#include "platform_linux.hpp"
#endif // HAKU_LINUX

s32 initializeWindow(void)
{
  s32 ret_val = createWindow(HAKU_WINDOW_WIDTH,HAKU_WINDOW_HEIGHT);
  if(H_OK != ret_val)
  {
    LOG_FATAL("Window Creation Failure...");
    return ret_val;
  }
  return ret_val;
}

void shutdownWindow()
{
  destroyWindow();
}

void processEvents()
{
  pumpMessages();
}
