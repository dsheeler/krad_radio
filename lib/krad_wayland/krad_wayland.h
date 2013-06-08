#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <signal.h>

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "krad_system.h"

#ifndef KRAD_WAYLAND_H
#define KRAD_WAYLAND_H

#define KR_WL_BUFFER_COUNT 2

typedef struct kr_wayland_st kr_wayland;
typedef struct kr_wayland_window_st kr_wayland_window;
typedef struct kr_wayland_pointer_st kr_wayland_pointer;

struct kr_wayland_pointer_st {
  int x;
  int y;
  int click;
  int mousein;
};

void kr_wayland_set_frame_callback(kr_wayland *wayland,
 int frame_callback(void *, uint32_t), void *pointer);

int kr_wayland_window_create(kr_wayland *wayland,
 int width, int height, void **buffer);

int kr_wayland_open_window(kr_wayland *wayland);
void kr_wayland_close_window(kr_wayland *wayland);

int kr_wayland_get_fd(kr_wayland *wayland);
void kr_wayland_process(kr_wayland *wayland);

void kr_wayland_destroy(kr_wayland *wayland);
kr_wayland *kr_wayland_create();

#endif
