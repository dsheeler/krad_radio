#include "krad_wayland_to_text.h"

int kr_wayland_info_to_text(char *text, void *st, int32_t max) {
  int res;
  kr_wayland_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_wayland_info *)st;

  res += snprintf(&text[res],max-res,"state : %d \n",actual->state);
  res += snprintf(&text[res],max-res,"display_name : %s \n",actual->display_name);

  return res;
}

int kr_wayland_path_info_to_text(char *text, void *st, int32_t max) {
  int res;
  kr_wayland_path_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_wayland_path_info *)st;

  res += snprintf(&text[res],max-res,"display_name : %s \n",actual->display_name);
  res += snprintf(&text[res],max-res,"state : %d \n",actual->state);
  res += snprintf(&text[res],max-res,"width : %d \n",actual->width);
  res += snprintf(&text[res],max-res,"height : %d \n",actual->height);
  res += snprintf(&text[res],max-res,"fullscreen : %d \n",actual->fullscreen);

  return res;
}

