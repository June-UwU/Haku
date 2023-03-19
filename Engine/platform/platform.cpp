#include "platform.hpp"
#include "logger.hpp"

#ifdef HAKU_LINUX
#include "platform_linux.hpp"
#endif // HAKU_LINUX
 
HakuWindow initializeWindow(void)
{
  HakuWindow window = createWindow(HAKU_WINDOW_WIDTH,HAKU_WINDOW_HEIGHT);
  if(NULL == window)
  {
    LOG_FATAL("Window Creation Failure...");
    window = nullptr;
  }
  return window;
}

void shutdownWindow(HakuWindow window)
{
  destroyWindow(window);
}

bool windowValid(HakuWindow window)
{
  if(NULL == window)
  {
    return false;
  }
  return true;
}

const s32 WindowEventLoop(void)
{
  return H_OK;
}
