#include "krad_v4l2_to_text.h"

int kr_v4l2_state_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  kr_v4l2_state *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_v4l2_state*)st;

  res += snprintf(&text[res],max,"kr_v4l2_state : %u \n",*actual);

  return res;
}

int kr_v4l2_mode_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  struct kr_v4l2_mode *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_v4l2_mode*)st;

  res += snprintf(&text[res],max,"width : %d \n",actual->width);
  res += snprintf(&text[res],max,"height : %d \n",actual->height);
  res += snprintf(&text[res],max,"num : %d \n",actual->num);
  res += snprintf(&text[res],max,"den : %d \n",actual->den);
  res += snprintf(&text[res],max,"format : %d \n",actual->format);

  return res;
}

int kr_v4l2_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  res = 0;
  struct kr_v4l2_info *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_v4l2_info*)st;

  res += snprintf(&text[res],max,"dev : %d \n",actual->dev);
  res += snprintf(&text[res],max,"priority : %d \n",actual->priority);
  uber.actual = &(actual->state);
  uber.type = TEXT_KR_V4L2_STATE;
  res += info_pack_to_text(&text[res],&uber,max-res);
  uber.actual = &(actual->mode);
  uber.type = TEXT_KR_V4L2_MODE;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

int kr_v4l2_open_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  res = 0;
  struct kr_v4l2_open_info *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_v4l2_open_info*)st;

  res += snprintf(&text[res],max,"dev : %d \n",actual->dev);
  res += snprintf(&text[res],max,"priority : %d \n",actual->priority);
  uber.actual = &(actual->mode);
  uber.type = TEXT_KR_V4L2_MODE;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

