#ifndef KRAD_DECKLINK_COMMON_H
#define KRAD_DECKLINK_COMMON_H

typedef struct kr_decklink_path_info kr_decklink_path_info;

struct kr_decklink_path_info {
  uint32_t width;
  uint32_t height;
  uint32_t num;
  uint32_t den;
  char input_device[64];
  char video_connector[64];
  char audio_connector[64];
};

#endif
