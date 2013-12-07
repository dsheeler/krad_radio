#include "krad_v4l2_to_text.h"

int kr_v4l2_mode_to_text(char *text, void *st, int32_t max) {
  int res;

  res = 0;
  kr_v4l2_mode *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_v4l2_mode*)st;

  res += snprintf(&text[res],max,"width : %d \n",actual->width);
  res += snprintf(&text[res],max,"height : %d \n",actual->height);
  res += snprintf(&text[res],max,"num : %d \n",actual->num);
  res += snprintf(&text[res],max,"den : %d \n",actual->den);
  res += snprintf(&text[res],max,"format : %d \n",actual->format);

  return res;
}
