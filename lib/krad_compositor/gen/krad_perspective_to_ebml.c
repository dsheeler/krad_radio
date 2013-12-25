#include "krad_perspective_to_ebml.h"

int kr_pos_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_pos *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_pos *)st;

  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->x);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->y);

  return res;
}

int kr_perspective_view_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_perspective_view *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_perspective_view *)st;

  uber.actual = &(actual->top_left);
  uber.type = EBML_KR_POS;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->top_right);
  uber.type = EBML_KR_POS;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->bottom_left);
  uber.type = EBML_KR_POS;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->bottom_right);
  uber.type = EBML_KR_POS;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_perspective_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_perspective *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_perspective *)st;

  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->width);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->height);
  uber.actual = &(actual->view);
  uber.type = EBML_KR_PERSPECTIVE_VIEW;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

