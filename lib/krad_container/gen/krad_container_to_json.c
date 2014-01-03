#include "krad_container_to_json.h"

int krad_codec_t_to_json(char *json, void *st, int32_t max) {
  int res;
  krad_codec_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (krad_codec_t *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int krad_container_type_t_to_json(char *json, void *st, int32_t max) {
  int res;
  krad_container_type_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (krad_container_type_t *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int krad_codec_header_St_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int i;
  int res;
  struct krad_codec_header_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct krad_codec_header_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"codec\": ");
  uber.actual = &(actual->codec);
  uber.type = JSON_KRAD_CODEC_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"count\" : %u",actual->count);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

