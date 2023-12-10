#include "event.hpp"

static 

Status initializeEventSystem(void) {

  return OK;
}

Status registerEvent(void* event);

Status processEvents(void);

void shutdownEventSystem(void);