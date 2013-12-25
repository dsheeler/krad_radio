#include "krad_x11_helpers.h"

int kr_x11_info_init(struct kr_x11_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_info));

  return 0;
}

int kr_x11_info_valid(struct kr_x11_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_x11_info_random(struct kr_x11_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_x11_path_info_init(struct kr_x11_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_path_info));

  return 0;
}

int kr_x11_path_info_valid(struct kr_x11_path_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_x11_path_info_random(struct kr_x11_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_x11_path_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

