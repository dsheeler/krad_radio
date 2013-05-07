#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <getopt.h>
#include <signal.h>
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

#ifndef KRAD_NO_TURBOJPEG
#define KRAD_TURBOJPEG
#include <turbojpeg.h>
#endif

#include "uvch264.h"
#include "krad_system.h"

#ifndef KRAD_V4L2_H
#define KRAD_V4L2_H

#define DEFAULT_V4L2_DEVICE "/dev/video0"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef struct krad_v4l2_ret_buffer_St krad_v4l2_ret_buffer_t;
typedef struct krad_v4l2_buffer_St krad_v4l2_buffer_t;
typedef struct krad_v4l2_St krad_v4l2_t;

struct krad_v4l2_buffer_St {
	void *start;
	size_t length;
	size_t offset;
};


struct krad_v4l2_St {

	int width;
	int height;
	int fps;

	int mode;
	int frames;
	int fd;

	struct timeval timestamp;
	krad_v4l2_buffer_t *buffers;
	unsigned int n_buffers;
	struct v4l2_buffer buf;
  char device[512];
#ifdef KRAD_TURBOJPEG
	tjhandle jpeg_dec;
#endif
	unsigned int encoded_size;
	unsigned char *codec_buffer;
};

/* public */
int krad_v4l2_is_h264_keyframe (unsigned char *buffer);
void krad_v4l2_yuv_mode (krad_v4l2_t *krad_v4l2);
void krad_v4l2_mjpeg_mode (krad_v4l2_t *krad_v4l2);
void krad_v4l2_h264_mode (krad_v4l2_t *krad_v4l2);

int krad_v4l2_mjpeg_to_jpeg (krad_v4l2_t *krad_v4l2, unsigned char *jpeg_buffer,
                             unsigned char *mjpeg_buffer, unsigned int mjpeg_size);

void krad_v4l2_mjpeg_to_rgb (krad_v4l2_t *krad_v4l2, unsigned char *argb_buffer,
                             unsigned char *mjpeg_buffer, unsigned int mjpeg_size);

krad_v4l2_t *krad_v4l2_create();
void krad_v4l2_destroy(krad_v4l2_t *krad_v4l2);

void krad_v4l2_close (krad_v4l2_t *krad_v4l2);
void krad_v4l2_open (krad_v4l2_t *krad_v4l2, char *device, int width, int height, int fps);

void krad_v4l2_stop_capturing (krad_v4l2_t *krad_v4l2);
void krad_v4l2_start_capturing (krad_v4l2_t *krad_v4l2);

char *krad_v4l2_read (krad_v4l2_t *krad_v4l2);
void krad_v4l2_frame_done (krad_v4l2_t *krad_v4l2);

int krad_v4l2_detect_devices ();
int krad_v4l2_get_device_filename (int device_num, char *device_name);

/* private */

void krad_v4l2_free_codec_buffer (krad_v4l2_t *krad_v4l2);
void krad_v4l2_alloc_codec_buffer (krad_v4l2_t *krad_v4l2);

void krad_v4l2_init_device (krad_v4l2_t *krad_v4l2);
void krad_v4l2_uninit_device (krad_v4l2_t *krad_v4l2);
void krad_v4l2_init_mmap (krad_v4l2_t *krad_v4l2);

void errno_exit (const char *s);
int xioctl (int fd, int request, void *arg);

#endif
