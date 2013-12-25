#include "krad_perspective_to_text.h"

int kr_pos_to_text(char *text, void *st, int32_t max) {
  int res;
  struct kr_pos *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_pos *)st;

  res += snprintf(&text[res],max-res,"x : %u \n",actual->x);
  res += snprintf(&text[res],max-res,"y : %u \n",actual->y);

  return res;
}

int kr_perspective_view_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_perspective_view *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_perspective_view *)st;

  uber.actual = &(actual->top_left);
  uber.type = TEXT_KR_POS;
  res += info_pack_to_text(&text[res],&uber,max-res);
  uber.actual = &(actual->top_right);
  uber.type = TEXT_KR_POS;
  res += info_pack_to_text(&text[res],&uber,max-res);
  uber.actual = &(actual->bottom_left);
  uber.type = TEXT_KR_POS;
  res += info_pack_to_text(&text[res],&uber,max-res);
  uber.actual = &(actual->bottom_right);
  uber.type = TEXT_KR_POS;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

int kr_perspective_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_perspective *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_perspective *)st;

  res += snprintf(&text[res],max-res,"width : %u \n",actual->width);
  res += snprintf(&text[res],max-res,"height : %u \n",actual->height);
  uber.actual = &(actual->view);
  uber.type = TEXT_KR_PERSPECTIVE_VIEW;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

