#include "krad_sfx_from_ebml.h"

int kr_sfx_control_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_sfx_control *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_sfx_control *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, (int32_t *)actual);

  return res;
}

int kr_sfx_effect_type_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_sfx_effect_type *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_sfx_effect_type *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, (int32_t *)actual);

  return res;
}

int kr_sfx_effect_control_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_sfx_effect_control *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_sfx_effect_control *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, (int32_t *)actual);

  return res;
}

int kr_eq_band_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_eq_band_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_eq_band_info *)st;

  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->db);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->bw);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->hz);

  return res;
}

int kr_eq_info_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int i;
  int res;
  struct kr_eq_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_eq_info *)st;

  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    uber.actual = &(actual->band[i]);
    uber.type = DEBML_KR_EQ_BAND_INFO;
    res += info_unpack_fr_ebml(&ebml[res],&uber);
  }

  return res;
}

int kr_lowpass_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_lowpass_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_lowpass_info *)st;

  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->bw);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->hz);

  return res;
}

int kr_highpass_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_highpass_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_highpass_info *)st;

  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->bw);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->hz);

  return res;
}

int kr_analog_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_analog_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_analog_info *)st;

  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->drive);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->blend);

  return res;
}

