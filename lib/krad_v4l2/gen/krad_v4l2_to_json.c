#include "krad_v4l2_to_json.h"

int kr_v4l2_state_to_json(char *json, void *st, int32_t max) {
  int res;
  res = 0;
  kr_v4l2_state *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_v4l2_state*)st;

  res += snprintf(&json[res],max,"\"%u\"",*actual);

  return res;
}

int kr_v4l2_mode_to_json(char *json, void *st, int32_t max) {
  int res;
  res = 0;
  struct kr_v4l2_mode *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_v4l2_mode*)st;

  res += snprintf(&json[res],max,"{");
  res += snprintf(&json[res],max,"\"width\" : %d,",actual->width);
  res += snprintf(&json[res],max,"\"height\" : %d,",actual->height);
  res += snprintf(&json[res],max,"\"num\" : %d,",actual->num);
  res += snprintf(&json[res],max,"\"den\" : %d,",actual->den);
  res += snprintf(&json[res],max,"\"format\" : %d",actual->format);
  res += snprintf(&json[res],max,"}");

  return res;
}

int kr_v4l2_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  res = 0;
  struct kr_v4l2_info *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_v4l2_info*)st;

  res += snprintf(&json[res],max,"{");
  res += snprintf(&json[res],max,"\"dev\" : %d,",actual->dev);
  res += snprintf(&json[res],max,"\"priority\" : %d,",actual->priority);
  res += snprintf(&json[res],max,"\"state\": ");
  uber.actual = &(actual->state);
  uber.type = JSON_KR_V4L2_STATE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,",");
  res += snprintf(&json[res],max,"\"mode\": ");
  uber.actual = &(actual->mode);
  uber.type = JSON_KR_V4L2_MODE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,"}");

  return res;
}

int kr_v4l2_open_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  res = 0;
  struct kr_v4l2_open_info *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_v4l2_open_info*)st;

  res += snprintf(&json[res],max,"{");
  res += snprintf(&json[res],max,"\"dev\" : %d,",actual->dev);
  res += snprintf(&json[res],max,"\"priority\" : %d,",actual->priority);
  res += snprintf(&json[res],max,"\"mode\": ");
  uber.actual = &(actual->mode);
  uber.type = JSON_KR_V4L2_MODE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,"}");

  return res;
}

