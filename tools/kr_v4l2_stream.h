#ifndef KR_V4L2_STREAM_H
#define KR_V4L2_STREAM_H

#include <stdio.h>
#include <unistd.h>

#include <krad_muxponder.h>
#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>
#include <krad_vpx.h>
#include <krad_vorbis.h>
#include <krad_v4l2.h>
#include <krad_ring.h>
#include <krad_framepool.h>
#include <krad_timer.h>
#include <krad_av.h>
#include <krad_convert.h>

typedef struct kr_v4l2s_params kr_v4l2s_params;

#include "gen/kr_v4l2_stream_config.h"

struct kr_v4l2s_params {
  uint32_t width;
  uint32_t height;
  uint32_t fps_num;
  uint32_t fps_den;
  uint32_t bitrate;
  char host[256];
  int32_t port;
  char mount[256];
  char password[128];
  int device;
};

#endif
