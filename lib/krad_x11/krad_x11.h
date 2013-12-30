#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/shm.h>

#ifdef KR_X11
#include <xcb/xcb.h>
#include <xcb/xcb_util.h>
#include <xcb/xcb_image.h>
#include <xcb/xproto.h>
#include <xcb/shm.h>
#endif

#include "krad_system.h"
#include "krad_convert.h"
#include "krad_av.h"
#include "krad_x11_common.h"

typedef struct kr_x11 kr_x11;
typedef struct kr_x11_path kr_x11_path;

struct kr_x11 {
  int screen_number;
  int pixels_size;
  unsigned char *pixels;
  uint64_t frames;
  int16_t x;
  int16_t y;
  int stride;
  int width;
  int height;
  int screen_width;
  int screen_height;
  int fullscreen;
  int *krad_x11_shutdown;
#ifdef KR_X11
  xcb_screen_t *screen;
  xcb_screen_iterator_t iter;
  xcb_connection_t *connection;
  xcb_window_t window;
  xcb_shm_segment_info_t shminfo;
  xcb_shm_get_image_cookie_t cookie;
  xcb_shm_get_image_reply_t *reply;
  xcb_image_t *img;
#endif
  uint8_t screen_bit_depth;
  int number;
  int capture_enabled;
};

struct kr_x11_path {
  kr_x11 *x11;
};

kr_x11 *kr_x11_create();
void kr_x11_destroy(kr_x11 *x11);
int kr_x11_capture(kr_x11 *x11, kr_image *image);
void kr_x11_disable_capture(kr_x11 *x11);
void kr_x11_enable_capture(kr_x11 *x11, uint32_t window_id);
