#pragma once

#include "defines.hpp"

Status initializeEventSystem(void);

Status registerEvent(void* event);

Status unregisterEvent(void* event);

Status processEvents(void);

void shutdownEventSystem(void);