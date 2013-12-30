#include "krad_alsa_to_text.h"

int kr_alsa_info_to_text(char *text, void *st, int32_t max) {
  int res;
  struct kr_alsa_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_alsa_info *)st;

  res += snprintf(&text[res],max-res,"card : %d \n",actual->card);
  res += snprintf(&text[res],max-res,"name : %s \n",actual->name);

  return res;
}

int kr_alsa_path_info_to_text(char *text, void *st, int32_t max) {
  int res;
  struct kr_alsa_path_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_alsa_path_info *)st;

  res += snprintf(&text[res],max-res,"card_num : %d \n",actual->card_num);

  return res;
}

