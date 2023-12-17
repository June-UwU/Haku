#include "platform.hpp"
#include "logger.hpp"

#ifdef HAKU_LINUX
#include "platform_linux.hpp"
#endif // HAKU_LINUX

Status initializeWindow(void) {
  s32 windowStatus = createWindow(HAKU_WINDOW_WIDTH,HAKU_WINDOW_HEIGHT);

  Status status = static_cast<Status>(windowStatus); 
  ASSERT_OK(status)

  return status;
}

void shutdownWindow() {
  destroyWindow();
}

void processEvents() {
  pumpMessages();
}
