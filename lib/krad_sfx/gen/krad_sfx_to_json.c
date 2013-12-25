#include "krad_sfx_to_json.h"

int kr_sfx_control_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_sfx_control *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_sfx_control *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_sfx_effect_type_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_sfx_effect_type *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_sfx_effect_type *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_sfx_effect_control_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_sfx_effect_control *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_sfx_effect_control *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_eq_band_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_eq_band_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_eq_band_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"db\" : %0.2f,",actual->db);
  res += snprintf(&json[res],max-res,"\"bw\" : %0.2f,",actual->bw);
  res += snprintf(&json[res],max-res,"\"hz\" : %0.2f",actual->hz);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_eq_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int i;
  int res;
  struct kr_eq_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_eq_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"band\" : [");
  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    uber.actual = &(actual->band[i]);
    uber.type = JSON_KR_EQ_BAND_INFO;
    res += info_pack_to_json(&json[res],&uber,max-res);
    if (i != (KR_EQ_MAX_BANDS - 1)) {
      res += snprintf(&json[res],max-res,",");
    }
  }
  res += snprintf(&json[res],max-res,"]");
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_lowpass_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_lowpass_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_lowpass_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"bw\" : %0.2f,",actual->bw);
  res += snprintf(&json[res],max-res,"\"hz\" : %0.2f",actual->hz);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_highpass_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_highpass_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_highpass_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"bw\" : %0.2f,",actual->bw);
  res += snprintf(&json[res],max-res,"\"hz\" : %0.2f",actual->hz);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_analog_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_analog_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_analog_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"drive\" : %0.2f,",actual->drive);
  res += snprintf(&json[res],max-res,"\"blend\" : %0.2f",actual->blend);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

