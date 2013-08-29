#ifndef KRAD_WAYLAND_COMMON_H
#define KRAD_WAYLAND_COMMON_H

typedef struct {
  int state;
  char display_name[128];
} kr_wayland_info;

typedef struct {
  char display_name[128];
  int state;
  int width;
  int height;
} kr_wayland_path_info;

#endif
