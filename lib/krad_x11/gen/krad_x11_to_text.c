#include "krad_x11_to_text.h"

int kr_x11_info_to_text(char *text, void *st, int32_t max) {
  int res;
  struct kr_x11_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_x11_info *)st;

  res += snprintf(&text[res],max-res,"display : %s \n",actual->display);

  return res;
}

int kr_x11_path_info_to_text(char *text, void *st, int32_t max) {
  int res;
  struct kr_x11_path_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_x11_path_info *)st;

  res += snprintf(&text[res],max-res,"display : %s \n",actual->display);
  res += snprintf(&text[res],max-res,"width : %u \n",actual->width);
  res += snprintf(&text[res],max-res,"height : %u \n",actual->height);
  res += snprintf(&text[res],max-res,"num : %u \n",actual->num);
  res += snprintf(&text[res],max-res,"den : %u \n",actual->den);
  res += snprintf(&text[res],max-res,"x : %d \n",actual->x);
  res += snprintf(&text[res],max-res,"y : %d \n",actual->y);

  return res;
}

