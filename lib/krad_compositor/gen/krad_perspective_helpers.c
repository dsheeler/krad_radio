#include "krad_perspective_helpers.h"

int kr_pos_init(void *st) {
  struct kr_pos *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_pos *)st;
  memset(st, 0, sizeof(struct kr_pos));

  return 0;
}

int kr_pos_valid(void *st) {
  struct kr_pos *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_pos *)st;

  return 0;
}

int kr_pos_random(void *st) {
  struct kr_pos *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_pos *)st;
  memset(st, 0, sizeof(struct kr_pos));

  return 0;
}

int kr_perspective_view_init(void *st) {
  struct kr_perspective_view *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_perspective_view *)st;
  memset(st, 0, sizeof(struct kr_perspective_view));
  kr_pos_init(&actual->top_left);
  kr_pos_init(&actual->top_right);
  kr_pos_init(&actual->bottom_left);
  kr_pos_init(&actual->bottom_right);

  return 0;
}

int kr_perspective_view_valid(void *st) {
  struct kr_perspective_view *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_perspective_view *)st;
  kr_pos_valid(&actual->top_left);
  kr_pos_valid(&actual->top_right);
  kr_pos_valid(&actual->bottom_left);
  kr_pos_valid(&actual->bottom_right);

  return 0;
}

int kr_perspective_view_random(void *st) {
  struct kr_perspective_view *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_perspective_view *)st;
  memset(st, 0, sizeof(struct kr_perspective_view));
  kr_pos_random(&actual->top_left);
  kr_pos_random(&actual->top_right);
  kr_pos_random(&actual->bottom_left);
  kr_pos_random(&actual->bottom_right);

  return 0;
}

int kr_perspective_init(void *st) {
  struct kr_perspective *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_perspective *)st;
  memset(st, 0, sizeof(struct kr_perspective));
  kr_perspective_view_init(&actual->view);

  return 0;
}

int kr_perspective_valid(void *st) {
  struct kr_perspective *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_perspective *)st;
  kr_perspective_view_valid(&actual->view);

  return 0;
}

int kr_perspective_random(void *st) {
  struct kr_perspective *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_perspective *)st;
  memset(st, 0, sizeof(struct kr_perspective));
  kr_perspective_view_random(&actual->view);

  return 0;
}

