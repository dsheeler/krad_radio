#include "krad_x11_to_json.h"

int kr_x11_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_x11_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_x11_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"display\" : \"%s\"",actual->display);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_x11_path_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_x11_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_x11_path_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"display\" : \"%s\",",actual->display);
  res += snprintf(&json[res],max-res,"\"width\" : %u,",actual->width);
  res += snprintf(&json[res],max-res,"\"height\" : %u,",actual->height);
  res += snprintf(&json[res],max-res,"\"num\" : %u,",actual->num);
  res += snprintf(&json[res],max-res,"\"den\" : %u,",actual->den);
  res += snprintf(&json[res],max-res,"\"x\" : %d,",actual->x);
  res += snprintf(&json[res],max-res,"\"y\" : %d",actual->y);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

