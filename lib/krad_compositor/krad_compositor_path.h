#ifndef KRAD_COMPOSITOR_PATH_H
#define KRAD_COMPOSITOR_PATH_H

#include "krad_compositor_subunit.h"
#include "krad_perspective.h"
#include "krad_framepool.h"
#include "krad_ring.h"

struct kr_compositor_path {

  kr_compositor *compositor;

  char sysname[128];
  int direction;

  krad_frame_t *last_frame;
  krad_ringbuffer_t *frame_ring;

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

  int crop_start_pixel[4];

  struct SwsContext *sws_converter;
  int sws_algorithm;
  int yuv_color_depth;

  int io_params_updated;
  int comp_params_updated;

  uint64_t start_timecode;

  int local;
  int localframe_state;
  int shm_sd;
  int msg_sd;
  char *local_buffer;
  int local_buffer_size;
  krad_frame_t *local_frame;

  krad_compositor_subunit_t subunit;

  krad_perspective_t *perspective;
  kr_perspective_view_t view;

  int socketpair[2];
};

#endif
