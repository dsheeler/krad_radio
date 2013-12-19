#include "krad_sfx_to_ebml.h"

int kr_sfx_control_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  kr_sfx_control *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_sfx_control*)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_sfx_effect_type_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  kr_sfx_effect_type *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_sfx_effect_type*)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_sfx_effect_control_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  kr_sfx_effect_control *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_sfx_effect_control*)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_eq_band_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  struct kr_eq_band_info *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_eq_band_info*)st;

  res += kr_ebml_pack_float(ebml, 0xe1, actual->db);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->bw);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->hz);

  return res;
}

int kr_eq_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int i;
  int res;
  res = 0;
  struct kr_eq_info *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_eq_info*)st;

  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    uber.actual = &(actual->band[i]);
    uber.type = EBML_KR_EQ_BAND_INFO;
    res += info_pack_to_ebml(&ebml[res],&uber);
  }

  return res;
}

int kr_lowpass_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  struct kr_lowpass_info *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_lowpass_info*)st;

  res += kr_ebml_pack_float(ebml, 0xe1, actual->bw);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->hz);

  return res;
}

int kr_highpass_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  struct kr_highpass_info *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_highpass_info*)st;

  res += kr_ebml_pack_float(ebml, 0xe1, actual->bw);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->hz);

  return res;
}

int kr_analog_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  struct kr_analog_info *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_analog_info*)st;

  res += kr_ebml_pack_float(ebml, 0xe1, actual->drive);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->blend);

  return res;
}

