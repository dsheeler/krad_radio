#include <stdio.h>
#include <unistd.h>

#ifdef FRAK_MACOSX
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFstring.h>
#include <CoreFoundation/CFbundle.h>
#include "krad_mach.h"
#endif

#ifdef KR_LINUX
#include <krad_muxponder.h>
#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>
#endif

#ifdef FRAK_MACOSX
#include <krad_ticker.h>
#include <krad_mkv.h>
#endif

#include <krad_vpx.h>
#include <krad_vorbis.h>
#include <krad_decklink.h>
#include <krad_decklink_capture.h>

#include <krad_ring.h>
#include <krad_framepool_nc.h>

#include <libswscale/swscale.h>

#define FRAK_MACOSX

#ifndef kr_dlstream_params_St

typedef struct kr_dlstream_params_St kr_dlstream_params_t;

struct kr_dlstream_params_St {
  uint32_t input_width;
  uint32_t input_height;
  uint32_t input_fps_numerator;
  uint32_t input_fps_denominator;
  char *input_device;
  char *video_input_connector;
  char *audio_input_connector;
  uint32_t encoding_width;
  uint32_t encoding_height;
  uint32_t encoding_fps_numerator;
  uint32_t encoding_fps_denominator;
  uint32_t video_bitrate;
  float audio_quality;
  char *host;
  int32_t port;
  char *mount;
  char *password;
};

#endif

void kr_dlstream ();

void kr_dlstream_thread_stop ();
void kr_dlstream_thread_start (kr_dlstream_params_t *params);

int kr_dlstream_device_count();

int kr_dlstream_device_name (int device_num, char *device_name);
