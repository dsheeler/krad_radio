#include "krad_v4l2.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef struct kr_v4l2_ret_buffer_St kr_v4l2_ret_buffer_t;
typedef struct kr_v4l2_buffer_St kr_v4l2_buffer_t;

struct kr_v4l2_buffer_St {
	void *start;
	size_t length;
	size_t offset;
};

struct kr_v4l2 {
	int width;
	int height;
	int fps;
	int mode;
	int frames;
	int fd;
	struct timeval timestamp;
	kr_v4l2_buffer_t *buffers;
	unsigned int n_buffers;
	struct v4l2_buffer buf;
  char device[512];
};

/*
void kr_v4l2_free_codec_buffer(kr_v4l2 *v4l2);
void kr_v4l2_alloc_codec_buffer(kr_v4l2 *v4l2);
void kr_v4l2_init_device(kr_v4l2 *v4l2);
void kr_v4l2_uninit_device(kr_v4l2 *v4l2);
void kr_v4l2_init_mmap(kr_v4l2 *v4l2);
void errno_exit(const char *s);

void kr_v4l2_close(kr_v4l2 *v4l2);
void kr_v4l2_open(kr_v4l2 *v4l2, char *device, int width, int height, int fps);

void kr_v4l2_stop_capturing(kr_v4l2 *v4l2);
void kr_v4l2_start_capturing(kr_v4l2 *v4l2);

char *kr_v4l2_read(kr_v4l2 *v4l2);
void kr_v4l2_frame_done(kr_v4l2 *v4l2);
int xioctl(int fd, int request, void *arg);


void kr_v4l2_frame_done (kr_v4l2 *v4l2) {
  if (-1 == xioctl (v4l2->fd, VIDIOC_QBUF, &v4l2->buf)) {
    errno_exit ("Krad V4L2: VIDIOC_QBUF");
  }
}

char *kr_v4l2_read (kr_v4l2 *v4l2) {

  v4l2->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  v4l2->buf.memory = V4L2_MEMORY_MMAP;

  if (-1 == xioctl (v4l2->fd, VIDIOC_DQBUF, &v4l2->buf)) {
    switch (errno) {
      case EAGAIN:
        return 0;
      case EIO:
      default:
        errno_exit ("Krad V4L2: VIDIOC_DQBUF");
    }
  }

  v4l2->timestamp = v4l2->buf.timestamp;
  v4l2->encoded_size = v4l2->buf.bytesused;

  return v4l2->buffers[v4l2->buf.index].start;
}

void kr_v4l2_start_capturing (kr_v4l2 *v4l2) {

  unsigned int i;
  enum v4l2_buf_type type;

  for (i = 0; i < v4l2->n_buffers; ++i) {

    struct v4l2_buffer buf;

    CLEAR (buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    if (-1 == xioctl (v4l2->fd, VIDIOC_QBUF, &buf)) {
      errno_exit ("Krad V4L2: VIDIOC_QBUF");
    }
  }

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (-1 == xioctl (v4l2->fd, VIDIOC_STREAMON, &type)) {
    errno_exit ("Krad V4L2: VIDIOC_STREAMON");
  }

  //if (v4l2->mode == V4L2_PIX_FMT_MJPEG) {
  //  video_set_quality(kr_v4l2, 55);
  //}

  //kr_v4l2_uvc_bluelight (kr_v4l2);

	if (v4l2->mode == V4L2_PIX_FMT_H264) {
		kr_v4l2_uvc_h264_reset (kr_v4l2);
		kr_v4l2_uvc_h264_set_rc_mode (kr_v4l2, RATECONTROL_VBR);
		kr_v4l2_uvc_h264_set_bitrate (kr_v4l2, 300);
		printk ("Krad V4L2: Set H264 mode to VBR");
	}
}

void kr_v4l2_stop_capturing (kr_v4l2 *v4l2) {

  enum v4l2_buf_type type;

  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (-1 == xioctl (v4l2->fd, VIDIOC_STREAMOFF, &type)) {
    errno_exit ("Krad V4L2: VIDIOC_STREAMOFF");
  }
}

void kr_v4l2_uninit_device (kr_v4l2 *v4l2) {

  unsigned int i;

  for (i = 0; i < v4l2->n_buffers; ++i)
		if (-1 == munmap (v4l2->buffers[i].start, v4l2->buffers[i].length))
					errno_exit ("Krad V4L2: munmap");

  free (v4l2->buffers);
}

void kr_v4l2_init_mmap (kr_v4l2 *v4l2) {

  struct v4l2_requestbuffers req;

  CLEAR (req);

  req.count = 24;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl (v4l2->fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
		    failfast ("Krad V4L2: %s does not support memory mapping", v4l2->device);
		} else {
		        errno_exit ("Krad V4L2: VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		failfast ("Krad V4L2: Insufficient buffer memory on %s\n", v4l2->device);
	}

	printk ("Krad V4L2: v4l2 says %d buffers", req.count);

	v4l2->buffers = calloc (req.count, sizeof (*v4l2->buffers));

	if (!v4l2->buffers) {
		failfast ("Krad V4L2: Out of memory");
	}

	for (v4l2->n_buffers = 0; v4l2->n_buffers < req.count; ++v4l2->n_buffers) {

		struct v4l2_buffer buf;

		CLEAR (buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = v4l2->n_buffers;

		if (-1 == xioctl (v4l2->fd, VIDIOC_QUERYBUF, &buf)) {
			errno_exit ("Krad V4L2: VIDIOC_QUERYBUF");
		}

		v4l2->buffers[v4l2->n_buffers].length = buf.length;
		v4l2->buffers[v4l2->n_buffers].offset = buf.m.offset;

		v4l2->buffers[v4l2->n_buffers].start = mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, v4l2->fd, buf.m.offset);

		if (MAP_FAILED == v4l2->buffers[v4l2->n_buffers].start) {
			errno_exit ("Krad V4L2: mmap");
		}
	}

	v4l2->n_buffers = req.count;
}

void kr_v4l2_init_device (kr_v4l2 *v4l2) {

	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;

  if (-1 == xioctl (v4l2->fd, VIDIOC_QUERYCAP, &cap)) {
    if (EINVAL == errno) {
      failfast ("Krad V4L2: %s is no V4L2 device", v4l2->device);
    } else {
      errno_exit ("Krad V4L2: VIDIOC_QUERYCAP");
    }
  }

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    failfast ("Krad V4L2: %s is no video capture device", v4l2->device);
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		failfast ("Krad V4L2: %s does not support streaming i/o", v4l2->device);
	}

	CLEAR (cropcap);

  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (0 == xioctl (v4l2->fd, VIDIOC_CROPCAP, &cropcap)) {

    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect;

    if (-1 == xioctl (v4l2->fd, VIDIOC_S_CROP, &crop)) {
      switch (errno) {
        case EINVAL:
          break;
        default:
          break;
      }
    }
  } else {
  }

	CLEAR (fmt);

	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = v4l2->width;
	fmt.fmt.pix.height      = v4l2->height;
	fmt.fmt.pix.bytesperline = v4l2->width;
	fmt.fmt.pix.sizeimage = 96000;

	fmt.fmt.pix.pixelformat = v4l2->mode;
	fmt.fmt.pix.field       = V4L2_FIELD_ANY;

	if (-1 == xioctl (v4l2->fd, VIDIOC_S_FMT, &fmt)) {
		errno_exit ("Krad V4L2: VIDIOC_S_FMT");
	}

	char fourcc[5];
	fourcc[4] = '\0';
	memcpy(&fourcc, (char *)&fmt.fmt.pix.pixelformat, 4);

	printkd ("Krad V4L2: %ux%u FMT %s Stride: %u Size: %u", fmt.fmt.pix.width, fmt.fmt.pix.height, fourcc,
														fmt.fmt.pix.bytesperline, fmt.fmt.pix.sizeimage);

	v4l2->width = fmt.fmt.pix.width;
	v4l2->height = fmt.fmt.pix.height;

	struct v4l2_streamparm stream_parameters;

	CLEAR (stream_parameters);
	stream_parameters.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl (v4l2->fd, VIDIOC_G_PARM, &stream_parameters)) {
		errno_exit ("Krad V4L2: VIDIOC_G_PARM");
	}

	printkd ("Krad V4L2: G Frameinterval %u/%u", stream_parameters.parm.capture.timeperframe.numerator,
										  stream_parameters.parm.capture.timeperframe.denominator);

	stream_parameters.parm.capture.timeperframe.numerator = 1;
	stream_parameters.parm.capture.timeperframe.denominator = v4l2->fps;

	if (-1 == xioctl (v4l2->fd, VIDIOC_S_PARM, &stream_parameters)) {
    printke ("Krad V4L2: unable to set stream parameters as speced");
    printke ("Krad V4L2: error %d, %s", errno, strerror (errno));
	}

	printkd ("Krad V4L2: S Frameinterval %u/%u", stream_parameters.parm.capture.timeperframe.numerator,
										  stream_parameters.parm.capture.timeperframe.denominator);

	if (stream_parameters.parm.capture.timeperframe.denominator != v4l2->fps) {
		printkd ("Krad V4L2: failed to get proper capture fps!");
	}

	kr_v4l2_init_mmap (kr_v4l2);
}

void errno_exit (const char *s) {
  failfast ("%s error %d, %s", s, errno, strerror (errno));
}

int xioctl (int fd, int request, void *arg) {

  int r;

  do r = ioctl (fd, request, arg);
  while (-1 == r && EINTR == errno);

  return r;
}

void kr_v4l2_close (kr_v4l2 *v4l2) {
  kr_v4l2_uninit_device (kr_v4l2);
  close (v4l2->fd);
}

void kr_v4l2_open (kr_v4l2 *v4l2, char *device, int width, int height, int fps) {

	struct stat st;

	strncpy(v4l2->device, device, 512);

	v4l2->width = width;
	v4l2->height = height;
	v4l2->fps = fps;

	if (-1 == stat (v4l2->device, &st)) {
		failfast ("Krad V4L2: Cannot identify '%s': %d, %s", v4l2->device, errno, strerror (errno));
	}

	if (!S_ISCHR (st.st_mode)) {
		failfast ("Krad V4L2: %s is no device", v4l2->device);
	}

	v4l2->fd = open (v4l2->device, O_RDWR | O_NONBLOCK, 0);

	if (-1 == v4l2->fd) {
		failfast ("Krad V4L2: Cannot open '%s': %d, %s", v4l2->device, errno, strerror (errno));
	}

	kr_v4l2_init_device (kr_v4l2);

}
*/
int kr_v4l2_destroy(kr_v4l2 *v4l2) {
  if (v4l2 == NULL) return -1;
  free(v4l2);
  return 0;
}

kr_v4l2 *kr_v4l2_create(kr_v4l2_setup *setup) {

	kr_v4l2 *v4l2;

  if (setup == NULL) return NULL;

	v4l2 = calloc(1, sizeof(kr_v4l2));
	v4l2->mode = V4L2_PIX_FMT_YUYV;

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
