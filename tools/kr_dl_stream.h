#ifndef KR_DL_STREAM_H
#define KR_DL_STREAM_H

#include <stdio.h>
#include <unistd.h>

#include <krad_muxponder.h>
#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>
#include <krad_vpx.h>
#include <krad_vorbis.h>
#include <krad_decklink.h>
#include <krad_decklink_capture.h>
#include <krad_ring.h>
#include <krad_framepool.h>
#include <krad_av.h>
#include <krad_convert.h>

typedef struct kr_dlstream_params kr_dlstream_params;

#include "gen/kr_dl_stream_config.h"

struct kr_dlstream_params {
  uint32_t input_width;
  uint32_t input_height;
  uint32_t input_fps_numerator;
  uint32_t input_fps_denominator;
  char input_device[64];
  char video_input_connector[64];
  char audio_input_connector[64];
  uint32_t encoding_width;
  uint32_t encoding_height;
  uint32_t encoding_fps_numerator;
  uint32_t encoding_fps_denominator;
  uint32_t video_bitrate;
  float audio_quality;
  char host[256];
  int32_t port;
  char mount[256];
  char password[256];
};

#endif
