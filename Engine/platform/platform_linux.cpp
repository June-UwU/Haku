#ifdef HAKU_LINUX


#include "platform_linux.hpp"
#include "logger.hpp"
#include <xcb/xcb.h>

typedef struct linux_window
{
  s32 height;
  s32 width;
  s32 cur_scn;
  xcb_connection_t* server;
} linux_window;


HakuWindow createWindow(s32 width, s32 height)
{
  linux_window* ret = new linux_window;

  ret->server = xcb_connect(NULL, NULL);
  const  xcb_setup_t* setup = xcb_get_setup(ret->server);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  xcb_screen_t* screen = iter.data;


  xcb_window_t window = xcb_generate_id(ret->server);
  xcb_create_window(ret->server,XCB_COPY_FROM_PARENT,window,screen->root,0,0,width,height,
      05,XCB_WINDOW_CLASS_INPUT_OUTPUT,screen->root_visual,0,NULL);

  xcb_map_window(ret->server,window);
  xcb_flush(ret->server);

  return ret;
}


void destroyWindow(HakuWindow window)
{
  linux_window* api_wnd = reinterpret_cast<linux_window*>(window);
  xcb_disconnect(api_wnd->server);
  delete api_wnd;
}

#endif // HAKU_LINUX
