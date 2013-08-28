#ifndef KRAD_WAYLAND_COMMON_H
#define KRAD_WAYLAND_COMMON_H

typedef struct {
  int state;
  char server_path[128];
} kr_wayland_info;

typedef struct {
  int state;
  int width;
  int height;
} kr_wayland_path_info;

#endif
