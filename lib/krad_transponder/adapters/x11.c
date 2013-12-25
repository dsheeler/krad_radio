#ifdef KR_X11

#include "krad_ticker.h"

int x11_adapter_process(kr_adapter_path *path) {
  kr_adapter_path_av_cb_arg cb_arg;
  kr_image image;
  kr_ticker *ticker;
  int ret;
  int num;
  int den;
  ticker = NULL;
  num = 30;
  den = 1;
  krad_system_set_thread_name("kr_x11");
  ticker = krad_ticker_create(num, den);
  krad_ticker_start(ticker);
  for(;;) {
    kr_x11_capture(path->adapter->handle.x11, image.px);
    if (ret == 1) {
      cb_arg.path = path;
      cb_arg.user = cb_arg.path->user;
      cb_arg.image = image;
      cb_arg.path->av_cb(&cb_arg);
    }
    krad_ticker_wait(ticker);
  }
  krad_ticker_destroy(ticker);
  return 0;
}

void x11_adapter_path_destroy(kr_adapter_path *path) {
  //stop the thread
}

void x11_adapter_path_create(kr_adapter_path *path) {
  //path->adapter->handle.x11->screen_width
  //path->adapter->handle.x11->screen_width
  kr_x11_enable_capture(path->adapter->handle.x11, 0);
  adapter_path_process_thread_start(path);
}

void x11_adapter_destroy(kr_adapter *adapter) {
  kr_x11_destroy(adapter->handle.x11);
  adapter->handle.x11 = NULL;
}

void x11_adapter_create(kr_adapter *adapter) {
  /*kr_x11_setup setup;
  memset(&setup, 0, sizeof(kr_x11_setup));
  setup.dev = adapter->info.api_info.x11.dev;
  setup.priority = adapter->info.api_info.x11.priority;*/
  adapter->handle.x11 = kr_x11_create();
  adapter->path_process_function = x11_adapter_process;
}
#endif
