#include "krad_wayland_to_ebml.h"

int kr_wayland_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_wayland_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_wayland_info *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->state);
  res += kr_ebml_pack_string(ebml, 0xe1, actual->display_name);

  return res;
}

int kr_wayland_path_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_wayland_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_wayland_path_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->display_name);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->state);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->width);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->height);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->fullscreen);

  return res;
}

