#include "krad_v4l2_helpers.h"

int kr_v4l2_mode_init(struct kr_v4l2_mode *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_v4l2_mode));

  return 0;
}

int kr_v4l2_mode_valid(struct kr_v4l2_mode *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_v4l2_mode_random(struct kr_v4l2_mode *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_v4l2_mode));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_v4l2_info_init(struct kr_v4l2_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_v4l2_info));
  kr_v4l2_mode_init(&st->mode);

  return 0;
}

int kr_v4l2_info_valid(struct kr_v4l2_info *st) {
  if (st == NULL) {
    return -1;
  }

  kr_v4l2_mode_valid(&st->mode);

  return 0;
}

int kr_v4l2_info_random(struct kr_v4l2_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_v4l2_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  kr_v4l2_mode_random(&st->mode);

  return 0;
}

int kr_v4l2_open_info_init(struct kr_v4l2_open_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_v4l2_open_info));
  kr_v4l2_mode_init(&st->mode);

  return 0;
}

int kr_v4l2_open_info_valid(struct kr_v4l2_open_info *st) {
  if (st == NULL) {
    return -1;
  }

  kr_v4l2_mode_valid(&st->mode);

  return 0;
}

int kr_v4l2_open_info_random(struct kr_v4l2_open_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_v4l2_open_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  kr_v4l2_mode_random(&st->mode);

  return 0;
}

