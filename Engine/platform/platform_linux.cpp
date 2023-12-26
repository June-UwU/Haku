#ifdef HAKU_LINUX

#include "platform_linux.hpp"
#include "logger.hpp"
#include "event.hpp"
#include <X11/Xlib.h>

typedef struct linux_window
{
  Display* display;
  Window window;
} linux_window;

static linux_window* WINDOW;

s32 createWindow(s32 width, s32 height) {
  WINDOW = new linux_window;

  WINDOW->display = XOpenDisplay(nullptr);
  WINDOW->window = XCreateSimpleWindow(WINDOW->display,DefaultRootWindow(WINDOW->display),0,0,width,height,0,0,WhitePixel(WINDOW->display,0));
  s64 mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask | SubstructureNotifyMask | ClientMessage;
  Atom wmDeleteWindow = XInternAtom(WINDOW->display, "WM_DELETE_WINDOW", False);
  [[maybe_unused]] Atom wmProtocols = XInternAtom(WINDOW->display, "WM_PROTOCOLS", False);
  XSetWMProtocols(WINDOW->display, WINDOW->window, &wmDeleteWindow, 1);

  XSelectInput(WINDOW->display,WINDOW->window,mask);
  XMapWindow(WINDOW->display,WINDOW->window);
  XFlush(WINDOW->display);
  XWindowAttributes attr;
  XGetWindowAttributes(WINDOW->display,WINDOW->window,&attr);
  
  if((attr.your_event_mask & mask) == mask) {
    LOG_TRACE("mask is set properly");
  }

  return OK; 
}


void destroyWindow() {
  XDestroyWindow(WINDOW->display,WINDOW->window);
  XCloseDisplay(WINDOW->display);
  delete WINDOW;
}

void pumpMessages() {
  XFlush(WINDOW->display);
  XEvent event;
  while(XPending(WINDOW->display)) {

    XNextEvent(WINDOW->display,&event);
    switch(event.type) {

      case KeyPress: {
        LOG_TRACE("key press event");
        break;
      }

      case ClientMessage: {

        Atom wmDelete = XInternAtom(WINDOW->display,"WM_DELETE_WINDOW",false);
        Atom wmProtocols = XInternAtom(WINDOW->display, "WM_PROTOCOLS",false);
        if ((wmProtocols == event.xclient.message_type) && (Atom)event.xclient.data.l[0] == wmDelete) {
          LOG_TRACE("window close event");
          Event event(EVENT_WINDOW_CLOSED);
          fireEvent(std::move(event));
        }

        break;
      }

    }

  }
}


#endif // HAKU_LINUX
