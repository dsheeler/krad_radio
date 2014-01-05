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
  int fullscreen;
} kr_wayland_path_info;

#include "gen/krad_wayland_to_ebml.h"
#include "gen/krad_wayland_from_ebml.h"
#include "gen/krad_wayland_to_text.h"
#include "gen/krad_wayland_to_json.h"
#include "gen/krad_wayland_helpers.h"

#endif
