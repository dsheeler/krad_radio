#ifndef KRAD_PERSPECTIVE_H
#define KRAD_PERSPECTIVE_H

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include "krad_timer.h"
#include "krad_av.h"

typedef struct kr_perspective kr_perspective;
typedef struct kr_perspective_view kr_perspective_view;
typedef struct kr_perspective_priv kr_perspective_priv;
typedef struct kr_pos kr_pos;

struct kr_pos {
  uint32_t x;
  uint32_t y;
};

struct kr_perspective_view {
  kr_pos top_left;
  kr_pos top_right;
  kr_pos bottom_left;
  kr_pos bottom_right;
};

struct kr_perspective {
  kr_perspective_priv *priv;
  uint32_t width;
  uint32_t height;
  kr_perspective_view view;
};

int32_t kr_perspective_argb(kr_perspective *p, uint8_t *out, uint8_t *in);
int32_t kr_perspective_apply(kr_perspective *p, kr_image *out, kr_image *in);
int32_t kr_perspective_set(kr_perspective *p, kr_perspective_view *view);
int32_t kr_perspective_destroy(kr_perspective **perspective);
kr_perspective *kr_perspective_create(uint32_t width, uint32_t height);

#endif
