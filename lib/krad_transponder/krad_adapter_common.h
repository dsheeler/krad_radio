#ifndef KRAD_ADAPTER_COMMON_H
#define KRAD_ADAPTER_COMMON_H

typedef struct kr_adapter_info kr_adapter_info;
typedef struct kr_adapter_path_info kr_adapter_path_info;

#include "krad_jack_common.h"

typedef enum {
  KR_ADP_V4L2,
  KR_ADP_DECKLINK,
  KR_ADP_ALSA,
  KR_ADP_X11,
  KR_ADP_WAYLAND,
  KR_ADP_ENCODER,
  KR_ADP_FLYCAP,
  KR_ADP_KRAPI,
  KR_ADP_JACK
} kr_adapter_api;

struct kr_adapter_info {
  kr_adapter_api api;
  union {
    kr_jack_info jack;
    /*
    kr_alsa_info alsa;
    kr_v4l2_info v4l2;
    kr_decklink_info decklink;
    */
  } api_info;
};

struct kr_adapter_path_info {
  kr_adapter_api api;
  char name[64];
  union {
    kr_jack_path_info jack;
    /*
    kr_alsa_path_info alsa;
    kr_v4l2_path_info v4l2;
    kr_decklink_path_info decklink;
    */
  } info;
};

#endif
