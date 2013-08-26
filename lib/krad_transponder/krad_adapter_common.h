#ifndef KRAD_ADAPTER_COMMON_H
#define KRAD_ADAPTER_COMMON_H

typedef struct kr_adapter_info kr_adapter_info;
typedef struct kr_adapter_path_info kr_adapter_path_info;

#include "krad_jack_common.h"

typedef enum {
  KR_ADP_PATH_INPUT = 1,
  KR_ADP_PATH_OUTPUT
} kr_adapter_path_direction;

typedef enum {
  KR_ADP_X11,         /* V      Input iPull                  */
  KR_ADP_V4L2,        /* V      Input ePush                  */
  KR_ADP_FLYCAP,      /* V      Input ePush only             */
  KR_ADP_WAYLAND,     /* V      Output iPush, ePull pos      */
  KR_ADP_ENCODER,     /* V + A  Output iPush only            */
  KR_ADP_DECKLINK,    /* V + A  Input ePush                  */
  KR_ADP_KRAPI,       /* V + A  I/O ANY ePush/ePull          */
  KR_ADP_ALSA,        /*     A  I/O ePush/ePull              */
  KR_ADP_JACK         /*     A  I/O ePush/ePull              */
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
  kr_adapter_path_direction dir;
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
