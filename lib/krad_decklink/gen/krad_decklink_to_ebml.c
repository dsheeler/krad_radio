#include "krad_decklink_to_ebml.h"

int kr_decklink_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_decklink_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_decklink_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->device);

  return res;
}

int kr_decklink_path_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_decklink_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_decklink_path_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->device);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->width);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->height);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->num);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->den);
  res += kr_ebml_pack_string(ebml, 0xe1, actual->video_connector);
  res += kr_ebml_pack_string(ebml, 0xe1, actual->audio_connector);

  return res;
}

