#include "krad_sfx_to_text.h"

int kr_sfx_control_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  kr_sfx_control *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_sfx_control*)st;

  res += snprintf(&text[res],max-res,"kr_sfx_control : %u \n",*actual);

  return res;
}

int kr_sfx_effect_type_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  kr_sfx_effect_type *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_sfx_effect_type*)st;

  res += snprintf(&text[res],max-res,"kr_sfx_effect_type : %u \n",*actual);

  return res;
}

int kr_sfx_effect_control_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  kr_sfx_effect_control *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_sfx_effect_control*)st;

  res += snprintf(&text[res],max-res,"kr_sfx_effect_control : %u \n",*actual);

  return res;
}

int kr_eq_band_info_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  struct kr_eq_band_info *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_eq_band_info*)st;

  res += snprintf(&text[res],max-res,"db : %0.2f \n",actual->db);
  res += snprintf(&text[res],max-res,"bw : %0.2f \n",actual->bw);
  res += snprintf(&text[res],max-res,"hz : %0.2f \n",actual->hz);

  return res;
}

int kr_eq_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int i;
  int res;
  res = 0;
  struct kr_eq_info *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_eq_info*)st;

  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    uber.actual = &(actual->band[i]);
    uber.type = TEXT_KR_EQ_BAND_INFO;
    res += info_pack_to_text(&text[res],&uber,max-res);
  }

  return res;
}

int kr_lowpass_info_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  struct kr_lowpass_info *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_lowpass_info*)st;

  res += snprintf(&text[res],max-res,"bw : %0.2f \n",actual->bw);
  res += snprintf(&text[res],max-res,"hz : %0.2f \n",actual->hz);

  return res;
}

int kr_highpass_info_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  struct kr_highpass_info *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_highpass_info*)st;

  res += snprintf(&text[res],max-res,"bw : %0.2f \n",actual->bw);
  res += snprintf(&text[res],max-res,"hz : %0.2f \n",actual->hz);

  return res;
}

int kr_analog_info_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  struct kr_analog_info *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_analog_info*)st;

  res += snprintf(&text[res],max-res,"drive : %0.2f \n",actual->drive);
  res += snprintf(&text[res],max-res,"blend : %0.2f \n",actual->blend);

  return res;
}

