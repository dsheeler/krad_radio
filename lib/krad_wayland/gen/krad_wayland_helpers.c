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
    if (!st->display_name[i]) {
      break;
    }
    if (i == 127 && st->display_name[i]) {
      return -3;
    }
  }

  return 0;
}

int kr_wayland_info_random(kr_wayland_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_info));
  for (i = 0; i < 128; i++) {
    scale = (double)25 / RAND_MAX;
    st->display_name[i] = 97 + floor(rand() * scale);
    if (i == 127) {
      st->display_name[127] = '\0';
    }
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
    if (!st->display_name[i]) {
      break;
    }
    if (i == 127 && st->display_name[i]) {
      return -2;
    }
  }

  return 0;
}

int kr_wayland_path_info_random(kr_wayland_path_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_wayland_path_info));
  for (i = 0; i < 128; i++) {
    scale = (double)25 / RAND_MAX;
    st->display_name[i] = 97 + floor(rand() * scale);
    if (i == 127) {
      st->display_name[127] = '\0';
    }
  }

  return 0;
}

