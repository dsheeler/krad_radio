#include "krad_v4l2_from_ebml.h"

int kr_v4l2_state_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  kr_v4l2_state *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_v4l2_state*)st;

  res += kr_ebml2_unpack_int8(ebml, actual, sizeof(int8_t));

  return res;
}

int kr_v4l2_mode_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  struct kr_v4l2_mode *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_v4l2_mode*)st;

  res += kr_ebml2_unpack_int32(ebml, &actual->width, sizeof(int));
  res += kr_ebml2_unpack_int32(ebml, &actual->height, sizeof(int));
  res += kr_ebml2_unpack_int32(ebml, &actual->num, sizeof(int));
  res += kr_ebml2_unpack_int32(ebml, &actual->den, sizeof(int));
  res += kr_ebml2_unpack_int32(ebml, &actual->format, sizeof(int));

  return res;
}

int kr_v4l2_info_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  res = 0;
  struct kr_v4l2_info *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_v4l2_info*)st;

  res += kr_ebml2_unpack_int32(ebml, &actual->dev, sizeof(int));
  res += kr_ebml2_unpack_int32(ebml, &actual->priority, sizeof(int));
  uber.actual = &(actual->state);
  uber.type = DEBML_KR_V4L2_STATE;
  res += info_unpack_fr_ebml(&ebml[res],&uber);
  uber.actual = &(actual->mode);
  uber.type = DEBML_KR_V4L2_MODE;
  res += info_unpack_fr_ebml(&ebml[res],&uber);

  return res;
}

int kr_v4l2_open_info_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  res = 0;
  struct kr_v4l2_open_info *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_v4l2_open_info*)st;

  res += kr_ebml2_unpack_int32(ebml, &actual->dev, sizeof(int));
  res += kr_ebml2_unpack_int32(ebml, &actual->priority, sizeof(int));
  uber.actual = &(actual->mode);
  uber.type = DEBML_KR_V4L2_MODE;
  res += info_unpack_fr_ebml(&ebml[res],&uber);

  return res;
}

