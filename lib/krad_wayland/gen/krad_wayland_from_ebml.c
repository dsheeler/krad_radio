#include "krad_wayland_from_ebml.h"

int kr_wayland_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_wayland_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_wayland_info *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->state);
  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->display_name , sizeof(actual->display_name));

  return res;
}

int kr_wayland_path_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_wayland_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_wayland_path_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->display_name , sizeof(actual->display_name));
  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->state);
  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->width);
  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->height);
  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->fullscreen);

  return res;
}

