#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#include "krad_codec_header.h"

#ifndef KRAD_OGG2_H
#define KRAD_OGG2_H

#include "krad_ogg_crc.h"

#ifndef KRAD_OGG_MAX_TRACKS
#define KRAD_OGG_MAX_TRACKS 10
#endif

typedef struct kr_ogg kr_ogg;
typedef struct kr_ogg_track kr_ogg_track;

struct kr_ogg {
  kr_ogg_track *tracks;
  size_t hdr_sz;
  uint8_t *hdr;
};

#include "krad_ogg2_io.h"

kr_ogg *kr_ogg_create();
int kr_ogg_destroy(kr_ogg **ogg);
int kr_ogg_add_track(kr_ogg *ogg, krad_codec_header_t *hdr);
int kr_ogg_generate_header(kr_ogg *ogg);
int kr_ogg_add_data(kr_ogg *ogg, int track, int64_t granule_position,
 uint8_t *data, size_t size, uint8_t *page);

#endif
