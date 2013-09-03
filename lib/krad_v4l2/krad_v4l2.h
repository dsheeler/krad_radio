#ifndef KRAD_V4L2_H
#define KRAD_V4L2_H

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>
#include <asm/types.h>
#include <dirent.h>

#include <linux/videodev2.h>
#include <linux/uvcvideo.h>

#include "krad_system.h"
#include "krad_av.h"
#include "krad_v4l2_common.h"

typedef struct kr_v4l2 kr_v4l2;
typedef struct {
  int dev;
  int priority;
} kr_v4l2_setup;

int kr_v4l2_release(kr_v4l2 *v4l2, kr_image *image);
int kr_v4l2_read(kr_v4l2 *v4l2, kr_image *image);
int kr_v4l2_poll(kr_v4l2 *v4l2, int ms);
int kr_v4l2_capture(kr_v4l2 *v4l2, int on);
int kr_v4l2_mode_set(kr_v4l2 *v4l2, kr_v4l2_mode *mode);
/* set control
 * get controls
 * get modes */
int kr_v4l2_stat(kr_v4l2 *v4l2, kr_v4l2_info **info);
int kr_v4l2_destroy(kr_v4l2 *v4l2);
kr_v4l2 *kr_v4l2_create(kr_v4l2_setup *setup);

int kr_v4l2_dev_count();

#endif
