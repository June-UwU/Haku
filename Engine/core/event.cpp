#include "logger.hpp"
#include "event.hpp"
#include <array>

constexpr const u32 EVENT_QUEUE_MAXSIZE = 256u;
constexpr const u32 EVENT_SERVICE_FN_MAXSIZE = 256u;

typedef struct EventServiceFn
{
  void* receiver;
  evt_fn Fn;
}EventServiceFn;

static std::array<Event, EVENT_QUEUE_MAXSIZE> EventQueue;
static std::array<std::array<EventServiceFn,EVENT_SERVICE_FN_MAXSIZE>,EVENT_TYPE_COUNT> EventVectorTable;
static u32 EventCount = 0u;

s8 initializeEventSystem(void)
{
  LOG_TRACE("initialzing logger event systems...");
  for(u32 i = 0; i < EVENT_TYPE_COUNT; i++)
  {
    for(auto& EventFunction : EventVectorTable[i])
    {
      EventFunction.receiver = nullptr;
      EventFunction.Fn = nullptr;
    }
  }
  return H_OK;
}
void shutdownEventSystem(void)
{

}

s8 registerEventVector(evt_fn fn, void* receiver, EventType type)
{
  LOG_TRACE("Event Register \n\t Type : %s \n\t Vector : %p \n\t Receiver : %p",EVENT_TYPE_STR[type],fn,receiver);
  auto& eventVector = EventVectorTable[type];
  for(auto& vector : eventVector)
  {
      vector.Fn = fn;
      vector.receiver = receiver;
  }
  return H_OK;
}

s8 unregisterEventVector(evt_fn fn, void* receiver, EventType type)
{
  LOG_TRACE("Event Unregister \n\t Type : %s \n\t Vector : %p \n\t Receiver : %p",EVENT_TYPE_STR[type],fn,receiver);
  auto& eventVector = EventVectorTable[type];

  for(u32 i = 0;  i < EVENT_SERVICE_FN_MAXSIZE; i++)
  {
    if(fn == eventVector[i].Fn && receiver == eventVector[i].receiver)
    {
      LOG_TRACE("Matching Event found\n\t Type : %s \n\t Vector : %p \n\t  Receiver : %p \n\t ",EVENT_TYPE_STR[type],fn,receiver);
      eventVector[i].Fn = nullptr;
    }
  }
  // TODO : make a it better by tracing duplicate registrations
  return H_OK;
}

s8 dispatchEvent(void)
{
  for(const auto& event: EventQueue)
  {
    auto& eventVector = EventVectorTable[event.type];
    for(const auto& eventservicefn : eventVector)
    {
      if(event.receiver == eventservicefn.receiver)
      {
        eventservicefn.Fn(event.sender,event.data);
      }
    }
    EventCount--;
  }
  return H_OK;
}

void onEvent(void* sender, void* receiver, u64 data, EventType type)
{
  EventQueue[EventCount].data = data;
  EventQueue[EventCount].receiver = receiver;
  EventQueue[EventCount].sender = sender;
  EventQueue[EventCount].type = type;
}
