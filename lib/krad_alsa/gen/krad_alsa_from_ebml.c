#include "krad_alsa_from_ebml.h"

int kr_alsa_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_alsa_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_alsa_info *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->lol);

  return res;
}

int kr_alsa_path_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_alsa_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_alsa_path_info *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->cake);

  return res;
}

