#ifndef KRAD_CONVERT_H
#define KRAD_CONVERT_H

#include <string.h>
#include <libswscale/swscale.h>

#include "krad_av.h"

typedef struct kr_crop kr_crop;
typedef struct kr_convert kr_convert;

struct kr_crop {
  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t height;
};

struct kr_convert {
  kr_crop crop;
  struct SwsContext *sws;
  int quality;
};

void kr_image_convert_init(kr_convert *conv);
int kr_image_convert(kr_convert *conv, kr_image *dst, kr_image *src);
void kr_image_convert_clear(kr_convert *conv);

#endif
