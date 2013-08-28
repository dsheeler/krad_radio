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
#include "krad_v4l2_common.h"

typedef struct kr_v4l2 kr_v4l2;
typedef struct kr_v4l2_path kr_v4l2_path;

typedef struct kr_v4l2_setup kr_v4l2_setup;
typedef struct kr_v4l2_path_setup kr_v4l2_path_setup;

struct kr_v4l2_setup {
  int devnum;
  kr_v4l2_info info;
};

struct kr_v4l2_path_setup {
  kr_v4l2 *v4l2;
};

int kr_v4l2_unlink(kr_v4l2_path *path);
kr_v4l2_path *kr_v4l2_mkpath(kr_v4l2_path_setup *setup);

int kr_v4l2_destroy(kr_v4l2 *v4l2);
kr_v4l2 *kr_v4l2_create(kr_v4l2_setup *setup);

int kr_v4l2_dev_count();

#endif
