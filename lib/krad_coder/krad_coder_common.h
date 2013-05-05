#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>

#ifndef KRAD_CODER_COMMON_H
#define KRAD_CODER_COMMON_H

#include "krad_player_common.h"
#include "krad_codec_header.h"

typedef enum {
  A,
  V,
  AV
} kr_mtype_t;

typedef struct kr_codeme_St kr_codeme_t;
typedef struct kr_medium_St kr_medium_t;
typedef struct kr_image_St kr_image_t;
typedef struct kr_audio_St kr_audio_t;

struct kr_codeme_St {
  uint8_t *data;
  size_t sz;
  int32_t count;
  uint64_t tc;
  kr_codec_t codec;
  kr_codec_hdr_t *hdr;
  int32_t key;
  uint32_t trk;  
};

struct kr_image_St {
  uint8_t *px;
  uint8_t *ppx[4];
  int32_t pps[4];  
  uint32_t w;
  uint32_t h;
  int32_t fmt;
  uint64_t ptc;
};

struct kr_audio_St {
  float *samples[8];
  uint32_t channels;
  int32_t count;
  int32_t rate;  
  uint64_t stc;
};

struct kr_medium_St {
  uint8_t *data;
  size_t sz;  
  kr_audio_t a;
  kr_image_t v;
  kr_mtype_t mt;
};

int32_t kr_codeme_kludge_destroy (kr_codeme_t **codeme);
int32_t kr_medium_kludge_destroy (kr_medium_t **medium);
kr_medium_t *kr_medium_kludge_create ();
kr_codeme_t *kr_codeme_kludge_create ();

#endif
