#include "krad_x11_helpers.h"

int kr_x11_info_init(struct kr_x11_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_info));
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_x11_info_valid(struct kr_x11_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_x11_info_random(struct kr_x11_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_info));
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_x11_path_info_init(struct kr_x11_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_path_info));
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_x11_path_info_valid(struct kr_x11_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_x11_path_info_random(struct kr_x11_path_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_path_info));
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

