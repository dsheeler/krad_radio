#include "krad_perspective_from_ebml.h"

int kr_pos_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_pos *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_pos *)st;

  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->x);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->y);

  return res;
}

int kr_perspective_view_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_perspective_view *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_perspective_view *)st;

  uber.actual = &(actual->top_left);
  uber.type = DEBML_KR_POS;
  res += info_unpack_fr_ebml(&ebml[res],&uber);
  uber.actual = &(actual->top_right);
  uber.type = DEBML_KR_POS;
  res += info_unpack_fr_ebml(&ebml[res],&uber);
  uber.actual = &(actual->bottom_left);
  uber.type = DEBML_KR_POS;
  res += info_unpack_fr_ebml(&ebml[res],&uber);
  uber.actual = &(actual->bottom_right);
  uber.type = DEBML_KR_POS;
  res += info_unpack_fr_ebml(&ebml[res],&uber);

  return res;
}

int kr_perspective_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_perspective *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_perspective *)st;

  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->width);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->height);
  uber.actual = &(actual->view);
  uber.type = DEBML_KR_PERSPECTIVE_VIEW;
  res += info_unpack_fr_ebml(&ebml[res],&uber);

  return res;
}

