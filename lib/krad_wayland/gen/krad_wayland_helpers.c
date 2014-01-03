#include "krad_wayland_helpers.h"

int kr_wayland_info_init(kr_wayland_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_info));
  for (i = 0; i < 128; i++) {
  }

  return 0;
}

int kr_wayland_info_valid(kr_wayland_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 128; i++) {
  }

  return 0;
}

int kr_wayland_info_random(kr_wayland_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_info));
  for (i = 0; i < 128; i++) {
  }

  return 0;
}

int kr_wayland_path_info_init(kr_wayland_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_path_info));
  for (i = 0; i < 128; i++) {
  }

  return 0;
}

int kr_wayland_path_info_valid(kr_wayland_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 128; i++) {
  }

  return 0;
}

int kr_wayland_path_info_random(kr_wayland_path_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_path_info));
  for (i = 0; i < 128; i++) {
  }

  return 0;
}

