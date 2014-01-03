#include "krad_container_from_ebml.h"

int krad_codec_t_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  krad_codec_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (krad_codec_t *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, (int32_t *)actual);

  return res;
}

int krad_container_type_t_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  krad_container_type_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (krad_container_type_t *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, (int32_t *)actual);

  return res;
}

int krad_codec_header_St_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int i;
  int res;
  struct krad_codec_header_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct krad_codec_header_St *)st;

  uber.actual = &(actual->codec);
  uber.type = DEBML_KRAD_CODEC_T;
  res += info_unpack_fr_ebml(&ebml[res],&uber);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->count);

  return res;
}

