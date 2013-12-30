#ifndef KR_X11_STREAM_H
#define KR_X11_STREAM_H
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
#include <krad_x11.h>
#include <krad_timer.h>
#include <krad_image_pool.h>
#include <krad_convert.h>

typedef struct kr_x11_stream_params kr_x11_stream_params;

#include "gen/kr_x11_stream_config.h"

struct kr_x11_stream_params {
  uint32_t width;
  uint32_t height;
  uint32_t fps_num;
  uint32_t fps_den;
  uint32_t bitrate;
  char host[256];
  int32_t port;
  char mount[512];
  char password[256];
  uint32_t window_id;
};

#endif
