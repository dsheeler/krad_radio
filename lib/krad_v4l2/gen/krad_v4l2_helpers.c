#include "krad_v4l2_helpers.h"

int kr_v4l2_state_to_index(int val) {
  switch (val) {
    case KR_V4L2_VOID:
      return 0;
    case KR_V4L2_OPEN:
      return 1;
    case KR_V4L2_CAPTURE:
      return 2;
  }
  return -1;
}

char *kr_strfr_kr_v4l2_state(int val) {
  switch (val) {
    case KR_V4L2_VOID:
      return "kr_v4l2_void";
    case KR_V4L2_OPEN:
      return "kr_v4l2_open";
    case KR_V4L2_CAPTURE:
      return "kr_v4l2_capture";
  }
  return NULL;
}

int kr_strto_kr_v4l2_state(char *string) {
  if (!strcmp(string,"kr_v4l2_void")) {
    return KR_V4L2_VOID;
  }
  if (!strcmp(string,"kr_v4l2_open")) {
    return KR_V4L2_OPEN;
  }
  if (!strcmp(string,"kr_v4l2_capture")) {
    return KR_V4L2_CAPTURE;
  }

  return -1;
}

int kr_v4l2_mode_init(void *st) {
  struct kr_v4l2_mode *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_v4l2_mode *)st;
  memset(st, 0, sizeof(struct kr_v4l2_mode));

  return 0;
}

int kr_v4l2_mode_valid(void *st) {
  struct kr_v4l2_mode *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_v4l2_mode *)st;

  return 0;
}

int kr_v4l2_mode_random(void *st) {
  struct kr_v4l2_mode *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_v4l2_mode *)st;
  memset(st, 0, sizeof(struct kr_v4l2_mode));

  return 0;
}

int kr_v4l2_info_init(void *st) {
  struct kr_v4l2_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_v4l2_info *)st;
  memset(st, 0, sizeof(struct kr_v4l2_info));
  kr_v4l2_mode_init(&actual->mode);

  return 0;
}

int kr_v4l2_info_valid(void *st) {
  struct kr_v4l2_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_v4l2_info *)st;
  kr_v4l2_mode_valid(&actual->mode);

  return 0;
}

int kr_v4l2_info_random(void *st) {
  struct kr_v4l2_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_v4l2_info *)st;
  memset(st, 0, sizeof(struct kr_v4l2_info));
  kr_v4l2_mode_random(&actual->mode);

  return 0;
}

int kr_v4l2_open_info_init(void *st) {
  struct kr_v4l2_open_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_v4l2_open_info *)st;
  memset(st, 0, sizeof(struct kr_v4l2_open_info));
  kr_v4l2_mode_init(&actual->mode);

  return 0;
}

int kr_v4l2_open_info_valid(void *st) {
  struct kr_v4l2_open_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_v4l2_open_info *)st;
  kr_v4l2_mode_valid(&actual->mode);

  return 0;
}

int kr_v4l2_open_info_random(void *st) {
  struct kr_v4l2_open_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_v4l2_open_info *)st;
  memset(st, 0, sizeof(struct kr_v4l2_open_info));
  kr_v4l2_mode_random(&actual->mode);

  return 0;
}

