#include "krad_alsa_to_json.h"

int kr_alsa_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_alsa_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_alsa_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"card\" : %d,",actual->card);
  res += snprintf(&json[res],max-res,"\"name\" : \"%s\"",actual->name);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_alsa_path_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_alsa_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_alsa_path_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"card_num\" : %d",actual->card_num);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

