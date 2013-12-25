#include "krad_wayland_helpers.h"

int kr_wayland_info_init(kr_wayland_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_info));

  return 0;
}

int kr_wayland_info_valid(kr_wayland_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_wayland_info_random(kr_wayland_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_wayland_path_info_init(kr_wayland_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_path_info));

  return 0;
}

int kr_wayland_path_info_valid(kr_wayland_path_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_wayland_path_info_random(kr_wayland_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_path_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

