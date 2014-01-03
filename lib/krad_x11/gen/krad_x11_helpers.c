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
    if (!st->display[i]) {
      break;
    }
    if (i == 63 && st->display[i]) {
      return -2;
    }
  }

  return 0;
}

int kr_x11_info_random(struct kr_x11_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->display[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->display[63] = '\0';
    }
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
    if (!st->display[i]) {
      break;
    }
    if (i == 63 && st->display[i]) {
      return -2;
    }
  }

  return 0;
}

int kr_x11_path_info_random(struct kr_x11_path_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_path_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->display[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->display[63] = '\0';
    }
  }

  return 0;
}

