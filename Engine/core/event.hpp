#pragma once

#include "defines.hpp"

typedef enum EventType
{
  WINDOW_CLOSE_EVENT,
  EVENT_TYPE_COUNT
}EventType;

typedef struct Event
{
  u64 type;
  void* sender;
  void* receiver;
  u64 data;
}Event;

constexpr const char* EVENT_TYPE_STR[EVENT_TYPE_COUNT]
{
  "WINDOW_CLOSE_EVENT"
};

typedef s8(*evt_fn)(void* sender, u64 data);

s8 initializeEventSystem(void);

void shutdownEventSystem(void);

s8 registerEventVector(evt_fn fn, void* receiver, EventType type);

s8 unregisterEventVector(evt_fn fn, void* receiver, EventType type);

s8 dispatchEvent(void);

void onEvent(void* sender, void* receiver, u64 data, EventType type);
