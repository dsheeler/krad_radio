#ifndef KRAD_V4L2_COMMON_H
#define KRAD_V4L2_COMMON_H

typedef enum {
  KR_V4L2_VOID,
  KR_V4L2_OPEN,
  KR_V4L2_CAPTURE
} kr_v4l2_state;

typedef struct {
  int width;
  int height;
  int num;
  int den;
  int format;
} kr_v4l2_mode;

typedef struct {
  int dev;
  int priority;
  kr_v4l2_state state;
  kr_v4l2_mode mode;
} kr_v4l2_info;

typedef struct {
  int dev;
  int priority;
  kr_v4l2_mode mode;
} kr_v4l2_open_info;

#endif
