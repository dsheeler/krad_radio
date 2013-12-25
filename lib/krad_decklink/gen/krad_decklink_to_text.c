#include "krad_decklink_to_text.h"

int kr_decklink_info_to_text(char *text, void *st, int32_t max) {
  int res;
  struct kr_decklink_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_decklink_info *)st;

  res += snprintf(&text[res],max-res,"device : %s \n",actual->device);

  return res;
}

int kr_decklink_path_info_to_text(char *text, void *st, int32_t max) {
  int res;
  struct kr_decklink_path_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_decklink_path_info *)st;

  res += snprintf(&text[res],max-res,"device : %s \n",actual->device);
  res += snprintf(&text[res],max-res,"width : %u \n",actual->width);
  res += snprintf(&text[res],max-res,"height : %u \n",actual->height);
  res += snprintf(&text[res],max-res,"num : %u \n",actual->num);
  res += snprintf(&text[res],max-res,"den : %u \n",actual->den);
  res += snprintf(&text[res],max-res,"video_connector : %s \n",actual->video_connector);
  res += snprintf(&text[res],max-res,"audio_connector : %s \n",actual->audio_connector);

  return res;
}

