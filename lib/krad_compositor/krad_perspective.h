#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <krad_timer.h>
#include <krad_coder_common.h>

typedef struct krad_perspective_St krad_perspective_t;
typedef struct krad_perspective_St kr_perspective_t;
typedef struct krad_perspective_view_St krad_perspective_view_t;
typedef struct krad_perspective_view_St kr_perspective_view_t;
typedef struct krad_perspective_priv_St kr_perspective_priv_t;
typedef struct krad_pixel_position_St kr_px_pos_t;

struct krad_pixel_position_St {
  uint32_t x;
  uint32_t y;
};

struct krad_perspective_view_St {
  kr_px_pos_t top_left;
  kr_px_pos_t top_right;
  kr_px_pos_t bottom_left;
  kr_px_pos_t bottom_right;
};

struct krad_perspective_St {
  kr_perspective_priv_t *priv;
  uint32_t width;
  uint32_t height;
  kr_perspective_view_t view;
};

int32_t kr_perspective_argb (kr_perspective_t *perspective,
                             uint8_t *out,
                             uint8_t *in);

int32_t kr_perspective (kr_perspective_t *perspective,
                        kr_image_t *out,
                        kr_image_t *in);

int32_t kr_perspective_set (kr_perspective_t *perspective,
                            kr_perspective_view_t *view);

int32_t kr_perspective_destroy (kr_perspective_t **krad_perspective);
kr_perspective_t *kr_perspective_create (uint32_t width, uint32_t height);
