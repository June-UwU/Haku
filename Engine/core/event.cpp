#include "event.hpp"
#include <map>
#include <deque>
#include <vector>

static std::map<EventType, std::vector<EventFn>> eventMap; 
static std::deque<Event> events;

Status initializeEventSystem(void) {
  return OK;
}

Status registerEvent(EventType event, EventFn fn) {
  auto& eventVec = eventMap[event];

  eventVec.push_back(fn);

  return OK;
}

Status handleEvents(void) {
  while(false == events.empty()) {
    Event event = std::move(events.front());
    events.pop_front();
    auto eventVec = eventMap[event.type];

    for(auto& fn : eventVec) {
      Status status = fn(std::move(event));
      if(status != OK) {
        return status;
      }
    }

  }

  return OK;
}

void fireEvent(Event event) {
  events.push_back(event);
}

void shutdownEventSystem(void) {

}
