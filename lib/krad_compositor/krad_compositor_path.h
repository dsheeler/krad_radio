#ifndef KRAD_COMPOSITOR_PATH_H
#define KRAD_COMPOSITOR_PATH_H

#include "krad_compositor_subunit.h"
#include "krad_perspective.h"
#include "krad_framepool.h"
#include "krad_ring.h"

typedef enum {
  KR_CMP_NIL,
  KR_CMP_READY,
  KR_CMP_ACTIVE,
  KR_CMP_TERM
} compositor_path_state;

typedef struct kr_compositor_path kr_compositor_path;
typedef struct kr_compositor_path_setup kr_compositor_path_setup;
typedef struct kr_compositor_path_frame_cb_arg kr_compositor_path_frame_cb_arg;
typedef struct kr_compositor_path_info_cb_arg kr_compositor_path_info_cb_arg;

typedef void (kr_compositor_info_cb)(kr_compositor_info_cb_arg *);
typedef void (kr_compositor_path_info_cb)(kr_compositor_path_info_cb_arg *);
typedef void (kr_compositor_path_frame_cb)(kr_compositor_path_frame_cb_arg *);

struct kr_compositor_path_info_cb_arg {
  void *user;
};

struct kr_compositor_path_frame_cb_arg {
  uint32_t channels;
  uint32_t nframes;
  float **samples;
  void *user;
};

struct kr_compositor_path_setup {
  kr_compositor_path_info info;
  void *user;
  kr_compositor_path_frame_cb *cb;
};

struct kr_compositor_path {
  kr_compositor_path_info info;
  void *user;
  kr_compositor_path_frame_cb *cb;
  kr_compositor *compositor;
  krad_frame_t *frame;
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

/*
 kr_compositor_path *kr_compositor_find(kr_compositor *compositor, char *name);
 int kr_compositor_path_ctl(kr_compositor_path *p, XXX);
 int kr_compositor_path_get_info(kr_compositor_path *path,
  kr_compositor_path_info *info);
*/

void cmper_path_release(kr_compositor *compositor, kr_compositor_path *path);

void kr_compositor_path_render(kr_compositor_path *path, cairo_t *cr);

int kr_compositor_unlink(kr_compositor_path *path);
kr_compositor_path *kr_compositor_mkpath(kr_compositor *compositor,
 kr_compositor_path_setup *setup);

#endif
