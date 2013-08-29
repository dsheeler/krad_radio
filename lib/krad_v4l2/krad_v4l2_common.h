#ifndef KRAD_V4L2_COMMON_H
#define KRAD_V4L2_COMMON_H

typedef struct {
  int state;
  char device_name[128];
} kr_v4l2_info;

typedef struct {
  int state;
  char device_name[128];
  int width;
  int height;
} kr_v4l2_path_info;

#endif
