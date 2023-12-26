#pragma once

#include "defines.hpp"
#include "logger.hpp"
#include <cstring>
#include <utility>

typedef u8 KeyCode;

typedef enum MouseButton {
    MOUSE_BUTTON_LEFT = 0x0,
    MOUSE_BUTTON_RIGHT = 0x1,
    MOUSE_BUTTON_MIDDLE = 0x2
}MouseButton;

typedef struct MouseState {
    int x;
    int y;
    bool click;
    bool release;
    MouseButton button;
}MouseState;

typedef enum EventType {
    EVENT_NONE = 0x0,
    EVENT_KEY_PRESSED,
    EVENT_KEY_RELEASED,
    EVENT_KEY_TYPED,
    EVENT_MOUSE_PRESSED,
    EVENT_MOUSE_RELEASED,
    EVENT_MOUSE_MOVED,
    EVENT_MOUSE_SCROLLED,
    EVENT_WINDOW_RESIZED,
    EVENT_WINDOW_CLOSED
}EventType;

class Event {
    public:
        Event(): 
            type(EVENT_NONE){}

        Event(const Event& rhs) {
            LOG_WARN("Event copy constructor called %s : %d",__FUNCTION__,__LINE__);
            type = rhs.type;
        }

        Event& operator=(const Event& rhs) {
            LOG_WARN("Event copy assignment called %s : %d",__FUNCTION__,__LINE__);
            type = rhs.type;
            return *this;
        }

        Event(Event&& rhs) {
            type = rhs.type;
        }

        Event& operator=(Event&& rhs) {
            type = rhs.type;
            return *this;
        }


        Event(EventType type) {
            this->type = type;
        }

        Event(EventType type, KeyCode code) 
            : type(type)
            , keyCode(code) {}
        
        Event(EventType type, MouseState state)
            : type(type)
            , mouseState(state) {}

        KeyCode getKeyCode() { return keyCode; }
        MouseState getMouseState() { return mouseState; }

        EventType type;
        
    private:
        KeyCode keyCode;
        MouseState mouseState;
};

typedef Status (*EventFn)(Event data);

[[nodiscard]] Status initializeEventSystem(void);

[[nodiscard]] Status registerEvent(EventType event, EventFn fn);

[[nodiscard]] Status handleEvents(void);

void fireEvent(Event event);

void shutdownEventSystem(void);