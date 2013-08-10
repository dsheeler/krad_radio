#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>
#include <time.h>
#include <sys/stat.h>

#ifndef KRAD_RADIO
#define KRAD_RADIO 1
#endif

#ifndef KRAD_RADIO_H
#define KRAD_RADIO_H

typedef struct kr_radio kr_radio;
typedef struct kr_log kr_log;
typedef struct kr_remote_control kr_remote_control;

#include "krad_radio_version.h"
#include "krad_system.h"
/*
#include "krad_ogg.h"
#include "krad_codec_header.h"
#include "krad_y4m.h"
#include "krad_xmms2.h"
#include "krad_easing.h"
#include "krad_container.h"
#include "krad_ring.h"
#include "krad_resample_ring.h"
#include "krad_tone.h"
#include "krad_jack.h"
#include "krad_vhs.h"
#ifdef KR_LINUX
#include "krad_v4l2.h"
#include "krad_alsa.h"
#endif
#ifdef FRAK_MACOSX
#include "krad_coreaudio.h"
#endif
#include "krad_udp.h"
#include "krad_x11.h"
#include "krad_theora.h"
#include "krad_vpx.h"
#include "krad_opus.h"
#include "krad_vorbis.h"
#include "krad_flac.h"
#include "krad_framepool.h"
#include "krad_decklink.h"
#include "krad_compositor_subunit.h"
#include "krad_sprite.h"
#include "krad_vector.h"
#include "krad_text.h"
#include "krad_perspective.h"
#ifdef KRAD_USE_WAYLAND
#include "krad_wayland.h"
#endif
#include "krad_transponder_graph.h"
*/
#include "krad_timer.h"
#include "krad_ticker.h"
#include "krad_tags.h"
#include "krad_ebml2.h"
#include "krad_app_server.h"
#include "krad_radio_ipc.h"
#include "krad_mixer.h"
#include "krad_compositor.h"
#include "krad_transponder.h"
#include "krad_interweb.h"

#include "krad_osc.h"

extern krad_system_t krad_system;

struct kr_remote_control {
  krad_osc_t *osc;
  krad_interweb_t *interweb;
};

struct kr_log {
  krad_timer_t *startup_timer;
  int verbose;
	char filename[512];
};

struct kr_radio {
  kr_mixer *mixer;
  kr_compositor *compositor;
  kr_transponder *transponder;
  krad_app_server_t *app;
  krad_app_broadcaster_t *system_broadcaster;
  kr_remote_control remote;
  krad_tags *tags;
  kr_log log;
  char sysname[KRAD_SYSNAME_SZ];
};

int kr_radio_daemon(char *sysname);

#endif
