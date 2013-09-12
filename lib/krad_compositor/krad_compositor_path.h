#ifndef KRAD_COMPOSITOR_PATH_H
#define KRAD_COMPOSITOR_PATH_H

#include "krad_compositor_subunit.h"
#include "krad_perspective.h"
#include "krad_framepool.h"
#include "krad_ring.h"

struct kr_compositor_path {
  kr_compositor *compositor;
  char sysname[128];
  int type;
  krad_frame_t *frame;
  int source_width;
  int source_height;
  int crop_x;
  int crop_y;
  int crop_width;
  int crop_height;
  kr_easer crop_x_easer;
  kr_easer crop_y_easer;
  kr_easer crop_width_easer;
  kr_easer crop_height_easer;
  struct SwsContext *converter;
  int sws_algorithm;
  krad_perspective_t *perspective;
  kr_perspective_view_t view;
  krad_compositor_subunit_t subunit;
};

void kr_compositor_path_render(kr_compositor_path *path, cairo_t *cr);
void path_release(kr_compositor *compositor, kr_compositor_path *path);

int kr_compositor_unlink(kr_compositor_path *path);
kr_compositor_path *kr_compositor_mkpath(kr_compositor *compositor,
 kr_compositor_path_setup *setup);

#endif
