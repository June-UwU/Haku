#include "platform.hpp"
#include "logger.hpp"

#ifdef HAKU_LINUX
#include "platform_linux.hpp"
#endif // HAKU_LINUX

Status initializeWindow(void)
{
  Status status = createWindow(HAKU_WINDOW_WIDTH,HAKU_WINDOW_HEIGHT);
  ASSERT_OK(status)

  return status;
}

void shutdownWindow()
{
  destroyWindow();
}

void processEvents()
{
  pumpMessages();
}
