#include "krad_x11_from_ebml.h"

int kr_x11_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_x11_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_x11_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->display , sizeof(actual->display));

  return res;
}

int kr_x11_path_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_x11_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_x11_path_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->display , sizeof(actual->display));
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->width);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->height);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->num);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->den);
  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->x);
  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->y);

  return res;
}

