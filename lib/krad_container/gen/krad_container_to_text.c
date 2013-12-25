#include "krad_container_to_text.h"

int krad_codec_t_to_text(char *text, void *st, int32_t max) {
  int res;
  krad_codec_t *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (krad_codec_t *)st;

  res += snprintf(&text[res],max-res,"krad_codec_t : %u \n",*actual);

  return res;
}

int krad_container_type_t_to_text(char *text, void *st, int32_t max) {
  int res;
  krad_container_type_t *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (krad_container_type_t *)st;

  res += snprintf(&text[res],max-res,"krad_container_type_t : %u \n",*actual);

  return res;
}

int krad_codec_header_St_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int i;
  int res;
  struct krad_codec_header_St *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct krad_codec_header_St *)st;

  uber.actual = &(actual->codec);
  uber.type = TEXT_KRAD_CODEC_T;
  res += info_pack_to_text(&text[res],&uber,max-res);
  res += snprintf(&text[res],max-res,"count : %u \n",actual->count);

  return res;
}

