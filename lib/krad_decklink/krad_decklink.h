#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>

#include "krad_system.h"
#include "krad_av.h"

#define KRAD_DECKLINK_H
#include "krad_decklink_capture.h"

#define DEFAULT_DECKLINK_DEVICE "0"

typedef struct kr_decklink kr_decklink;

struct kr_decklink {
	krad_decklink_capture_t *capture;
	int (*image_cb)(void *, kr_image *);
	int (*audio_cb)(void *, void *, int);
	void *user;
	int verbose;
	float *samples[16];
	unsigned char *captured_frame_rgb;
	int devicenum;
	char simplename[64];
};

void kr_decklink_destroy(kr_decklink *decklink);
kr_decklink *kr_decklink_create(char *device);
/* FIXME use a single set params function */
void kr_decklink_set_verbose(kr_decklink *decklink, int verbose);
void kr_decklink_set_video_mode(kr_decklink *decklink, int width, int height,
 int fps_numerator, int fps_denominator);
void kr_decklink_set_audio_input(kr_decklink *decklink, char *audio_input);
void kr_decklink_set_video_input(kr_decklink *decklink, char *video_input);

void kr_decklink_start(kr_decklink *decklink);
void kr_decklink_stop(kr_decklink *decklink);
int kr_decklink_detect_devices();
int kr_decklink_get_device_name(int device_num, char *device_name);
