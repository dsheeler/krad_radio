#ifndef KRAD_SLICE_H
#define KRAD_SLICE_H

#include <stddef.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "krad_codec_header.h"

typedef struct kr_slice_St kr_slice_t;

struct kr_slice_St {
  uint8_t *data;
  int32_t size;
  int frames;
  int keyframe;
  uint64_t timecode;
  int refs;
  int final;
  int header;
  krad_codec_t codec;
};

void kr_slice_ref (kr_slice_t *kr_slice);
void kr_slice_unref (kr_slice_t *kr_slice);
kr_slice_t *kr_slice_create_with_data (uint8_t *data, uint32_t size);
kr_slice_t *kr_slice_create ();
int kr_slice_set_data (kr_slice_t *kr_slice,
                       uint8_t *data, uint32_t size);
                         
                         
#endif
