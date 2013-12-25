#include "krad_io_to_ebml.h"

int krad_io_mode_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  krad_io_mode_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (krad_io_mode_t *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_io2_St_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_io2_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_io2_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->fd);
  uber.actual = &(actual->mode);
  uber.type = EBML_KRAD_IO_MODE_T;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

