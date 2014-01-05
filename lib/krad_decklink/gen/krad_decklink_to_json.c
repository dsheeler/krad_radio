#include "krad_decklink_to_json.h"

int kr_decklink_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_decklink_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_decklink_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"device\" : \"%s\"",actual->device);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_decklink_path_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_decklink_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_decklink_path_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"device\" : \"%s\",",actual->device);
  res += snprintf(&json[res],max-res,"\"width\" : %u,",actual->width);
  res += snprintf(&json[res],max-res,"\"height\" : %u,",actual->height);
  res += snprintf(&json[res],max-res,"\"num\" : %u,",actual->num);
  res += snprintf(&json[res],max-res,"\"den\" : %u,",actual->den);
  res += snprintf(&json[res],max-res,"\"video_connector\" : \"%s\",",actual->video_connector);
  res += snprintf(&json[res],max-res,"\"audio_connector\" : \"%s\"",actual->audio_connector);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

