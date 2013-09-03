#include "krad_v4l2.h"

#define KR_V4L2_BUFS 12

typedef struct {
  void *start;
  size_t length;
  size_t offset;
  struct timeval timestamp;
  struct v4l2_buffer buf;
} kr_v4l2_frame;

struct kr_v4l2 {
  int fd;
  kr_v4l2_info info;
  uint32_t nframes;
  kr_v4l2_frame frames[KR_V4L2_BUFS];
};

static int xioctl(int fd, int request, void *arg);
static void kr_v4l2_unmap(kr_v4l2 *v4l2);
static void kr_v4l2_map(kr_v4l2 *v4l2);
static void kr_v4l2_close(kr_v4l2 *v4l2);
static void kr_v4l2_open(kr_v4l2 *v4l2);

static int xioctl(int fd, int request, void *arg) {
  int r;
  do r = ioctl(fd, request, arg);
  while (-1 == r && EINTR == errno);
  return r;
}

int kr_v4l2_release(kr_v4l2 *v4l2, kr_image *image) {
  if (image == NULL) return -1;
  if (image->owner == NULL) return -1;
  if (-1 == xioctl(v4l2->fd, VIDIOC_QBUF, image->owner)) {
    printke("Krad V4L2: VIDIOC_QBUF");
    return -1;
  }
  image->owner = NULL;
  return 0;
}

int kr_v4l2_poll(kr_v4l2 *v4l2, int ms) {

  struct pollfd fds[1];

  if (v4l2 == NULL) return -1;
  if (v4l2->fd == -1) return -1;
  if (v4l2->info.state != KR_V4L2_CAPTURE) return -1;
  fds[0].fd = v4l2->fd;
  fds[0].events = POLLIN;

  return poll(fds, 1, ms);
}

int kr_v4l2_read(kr_v4l2 *v4l2, kr_image *image) {

  struct v4l2_buffer buf;

  if (v4l2 == NULL) return -1;
  if (image == NULL) return -1;

  memset(&buf, 0, sizeof(buf));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(v4l2->fd, VIDIOC_DQBUF, &buf)) {
    switch (errno) {
      case EAGAIN:
        return 0;
      default:
        printke("Krad V4L2: VIDIOC_DQBUF");
        return -1;
    }
  }

  /*  v4l2->timestamp = buf.timestamp;
  return v4l2->buffers[buf.index].start; */
  image->owner = &v4l2->frames[buf.index].buf;
  return 1;
}

int kr_v4l2_capture(kr_v4l2 *v4l2, int on) {

  uint32_t i;
  struct v4l2_buffer buf;
  enum v4l2_buf_type type;

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (v4l2 == NULL) return -1;
  if (v4l2->fd == -1) return -1;

  if (on == 0) {
    if (v4l2->info.state != KR_V4L2_CAPTURE) {
      return 0;
    }
    if (-1 == xioctl (v4l2->fd, VIDIOC_STREAMOFF, &type)) {
      printke("Krad V4L2: VIDIOC_STREAMOFF");
      v4l2->info.state = KR_V4L2_VOID;
      return -1;
    }
    v4l2->info.state = KR_V4L2_VOID;
    return 0;
  }

  if (v4l2->nframes == 0) return -1;
  for (i = 0; i < v4l2->nframes; i++) {
    memset(&buf, 0, sizeof(buf));
    buf.type = type;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    if (-1 == xioctl(v4l2->fd, VIDIOC_QBUF, &buf)) {
      printke("Krad V4L2: VIDIOC_QBUF");
      return -1;
    }
  }

  if (-1 == xioctl(v4l2->fd, VIDIOC_STREAMON, &type)) {
    printke("Krad V4L2: VIDIOC_STREAMON");
    return -1;
  }
  v4l2->info.state = KR_V4L2_CAPTURE;
  return 0;
}

static void kr_v4l2_unmap(kr_v4l2 *v4l2) {
  int i;
  if (v4l2->nframes > 0) {
    kr_v4l2_capture(v4l2, 0);
    for (i = 0; i < v4l2->nframes; i++) {
  	  if (-1 == munmap(v4l2->frames[i].start, v4l2->frames[i].length)) {
        printke("Krad V4L2: munmap %d", i);
      }
    }
    v4l2->nframes = 0;
  }
}

static void kr_v4l2_map(kr_v4l2 *v4l2) {

  int i;
  struct v4l2_buffer buf;
  struct v4l2_requestbuffers req;

  memset(&req, 0, sizeof(req));
  req.count = KR_V4L2_BUFS;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl(v4l2->fd, VIDIOC_REQBUFS, &req)) {
    if (EINVAL == errno) {
      printke("Krad V4L2: device does not support memory mapping");
    } else {
      printke("Krad V4L2: VIDIOC_REQBUFS");
    }
    return;
  }
  if (req.count < 2) {
    printke("Krad V4L2: Insufficient buffer memory");
    return;
  }
  v4l2->nframes = req.count;
	printk("Krad V4L2: %d buffers", v4l2->nframes);
	for (i = 0; i < v4l2->nframes; i++) {
    memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (-1 == xioctl (v4l2->fd, VIDIOC_QUERYBUF, &buf)) {
			printke("Krad V4L2: VIDIOC_QUERYBUF");
      v4l2->nframes = 0;
      return;
    }
    v4l2->frames[i].buf = buf;
		v4l2->frames[i].length = buf.length;
		v4l2->frames[i].offset = buf.m.offset;
		v4l2->frames[i].start = mmap(NULL, buf.length,
     PROT_READ | PROT_WRITE, MAP_SHARED, v4l2->fd, buf.m.offset);
		if (MAP_FAILED == v4l2->frames[i].start) {
			printke("Krad V4L2: mmap");
      v4l2->nframes = 0;
      return;
		}
	}
}

int kr_v4l2_mode_set(kr_v4l2 *v4l2, kr_v4l2_mode *mode) {

	struct v4l2_format fmt;
	struct v4l2_streamparm stream_parameters;

  kr_v4l2_unmap(v4l2);
  memset(&stream_parameters, 0, sizeof(stream_parameters));
	memset(&fmt, 0, sizeof(fmt));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  stream_parameters.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = mode->width;
  fmt.fmt.pix.height = mode->height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_ANY;
  if (-1 == xioctl (v4l2->fd, VIDIOC_S_FMT, &fmt)) {
    printke("Krad V4L2: VIDIOC_S_FMT");
    return -1;
  }
  /*
  if (-1 == xioctl (v4l2->fd, VIDIOC_G_PARM, &stream_parameters)) {
    printke("Krad V4L2: VIDIOC_G_PARM");
    return -1;
  }
  */
  stream_parameters.parm.capture.timeperframe.numerator = mode->num;
  stream_parameters.parm.capture.timeperframe.denominator = mode->den;
  if (-1 == xioctl (v4l2->fd, VIDIOC_S_PARM, &stream_parameters)) {
    printke("Krad V4L2: unable to set stream parameters as speced");
    printke("Krad V4L2: error %d, %s", errno, strerror (errno));
    return -1;
  }
  kr_v4l2_map(v4l2);
  if (v4l2->nframes == 0) {
    return -1;
  }
  return 0;
}

static void kr_v4l2_close(kr_v4l2 *v4l2) {
  if (v4l2->fd > -1) {
    kr_v4l2_unmap(v4l2);
    close(v4l2->fd);
    v4l2->fd = -1;
    v4l2->info.state = KR_V4L2_VOID;
  }
}

static void kr_v4l2_open(kr_v4l2 *v4l2) {

	struct stat st;
  char device[128];
	struct v4l2_capability cap;

  snprintf(device, sizeof(device), "/dev/video%d", v4l2->info.dev);
	if (-1 == stat(device, &st)) {
		printke("Krad V4L2: Cannot identify '%s': %d, %s", device, errno,
     strerror(errno));
    return;
  }
	if (!S_ISCHR(st.st_mode)) {
		printke("Krad V4L2: %s is no device", device);
    return;
	}
	v4l2->fd = open(device, O_RDWR | O_NONBLOCK, 0);
	if (-1 == v4l2->fd) {
    printke("Krad V4L2: Cannot open '%s': %d, %s", device, errno,
     strerror(errno));
    return;
	}

  if (-1 == xioctl(v4l2->fd, VIDIOC_QUERYCAP, &cap)) {
    printke("Krad V4L2: VIDIOC_QUERYCAP");
    kr_v4l2_close(v4l2);
    return;
  } else {
  	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
      printke("Krad V4L2: %s is no video capture device", device);
      kr_v4l2_close(v4l2);
      return;
  	}
  	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
	  	printke("Krad V4L2: %s does not support streaming i/o", device);
      kr_v4l2_close(v4l2);
      return;
    }
  }
  v4l2->info.state = KR_V4L2_OPEN;
}

int kr_v4l2_destroy(kr_v4l2 *v4l2) {
  if (v4l2 == NULL) return -1;
  kr_v4l2_close(v4l2);
  free(v4l2);
  return 0;
}

kr_v4l2 *kr_v4l2_create(kr_v4l2_setup *setup) {

	kr_v4l2 *v4l2;

  if (setup == NULL) return NULL;

	v4l2 = calloc(1, sizeof(kr_v4l2));
  v4l2->info.dev = setup->dev;
  v4l2->info.priority = setup->priority;
  kr_v4l2_open(v4l2);

	return v4l2;
}

int kr_v4l2_dev_count() {

	DIR *dp;
	struct dirent *ep;
	int count;

	count = 0;
	dp = opendir("/dev");

	if (dp == NULL) {
		printke("Couldn't open the /dev directory");
		return 0;
	}

	while ((ep = readdir(dp))) {
		if (memcmp(ep->d_name, "video", 5) == 0) {
			printk("Found V4L2 Device: /dev/%s", ep->d_name);
			count++;
		}
	}
	closedir(dp);
	return count;
}
