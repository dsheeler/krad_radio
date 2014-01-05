#include "krad_wayland_to_json.h"

int kr_wayland_info_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_wayland_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_wayland_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"state\" : %d,",actual->state);
  res += snprintf(&json[res],max-res,"\"display_name\" : \"%s\"",actual->display_name);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_wayland_path_info_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_wayland_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_wayland_path_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"display_name\" : \"%s\",",actual->display_name);
  res += snprintf(&json[res],max-res,"\"state\" : %d,",actual->state);
  res += snprintf(&json[res],max-res,"\"width\" : %d,",actual->width);
  res += snprintf(&json[res],max-res,"\"height\" : %d,",actual->height);
  res += snprintf(&json[res],max-res,"\"fullscreen\" : %d",actual->fullscreen);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

