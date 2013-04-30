#include "krad_codec_header.h"

#ifndef KRAD_PLAYER_COMMON_H
#define KRAD_PLAYER_COMMON_H

typedef enum {
  A,
  V,
  AV
} kr_mtype_t;

typedef struct kr_codeme_St kr_codeme_t;
typedef struct kr_medium_St kr_medium_t;
typedef struct kr_image_St kr_image_t;
typedef struct kr_audio_St kr_audio_t;

typedef struct kr_track_params_St kr_track_params_t;
typedef struct kr_video_params_St kr_video_params_t;
typedef struct kr_audio_params_St kr_audio_params_t;

typedef struct kr_track_info_St kr_track_info_t;

struct kr_codeme_St {
  uint8_t *data;
  size_t sz;
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
  kr_audio_t a;
  kr_image_t v;
  kr_mtype_t mt;
};

struct kr_video_params_St {
  uint32_t width;
  uint32_t height;
};

struct kr_audio_params_St {
  uint32_t rate;
  uint32_t chans;
};

typedef union {
  kr_video_params_t v;
  kr_audio_params_t a;
} kr_medium_params_t;

struct kr_track_info_St {
  kr_medium_params_t params;
  kr_codec_hdr_t header;
};

#endif
