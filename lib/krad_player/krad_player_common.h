#ifndef KRAD_PLAYER_COMMON_H
#define KRAD_PLAYER_COMMON_H

#include "krad_codec_header.h"
#include "krad_coder_common.h"

typedef struct kr_track_params_St kr_track_params_t;
typedef struct kr_video_params_St kr_video_params_t;
typedef struct kr_audio_params_St kr_audio_params_t;

typedef struct kr_track_info_St kr_track_info_t;

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
