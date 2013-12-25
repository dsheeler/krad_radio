#include "krad_x11_to_ebml.h"

int kr_x11_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_x11_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_x11_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->display);

  return res;
}

int kr_x11_path_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_x11_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_x11_path_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->display);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->width);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->height);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->num);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->den);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->x);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->y);

  return res;
}

