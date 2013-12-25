#include "krad_decklink_from_ebml.h"

int kr_decklink_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_decklink_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_decklink_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->device , sizeof(actual->device));

  return res;
}

int kr_decklink_path_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_decklink_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_decklink_path_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->device , sizeof(actual->device));
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->width);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->height);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->num);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->den);
  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->video_connector , sizeof(actual->video_connector));
  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->audio_connector , sizeof(actual->audio_connector));

  return res;
}

