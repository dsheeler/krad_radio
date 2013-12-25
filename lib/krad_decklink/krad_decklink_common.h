#ifndef KRAD_DECKLINK_COMMON_H
#define KRAD_DECKLINK_COMMON_H

typedef struct kr_decklink_info kr_decklink_info;
typedef struct kr_decklink_path_info kr_decklink_path_info;

struct kr_decklink_info {
  char device[64];
};

struct kr_decklink_path_info {
  char device[64];
  uint32_t width;
  uint32_t height;
  uint32_t num;
  uint32_t den;
  char video_connector[64];
  char audio_connector[64];
};

#include "gen/krad_decklink_to_ebml.h"
#include "gen/krad_decklink_from_ebml.h"
#include "gen/krad_decklink_to_text.h"
#include "gen/krad_decklink_helpers.h"

#endif
