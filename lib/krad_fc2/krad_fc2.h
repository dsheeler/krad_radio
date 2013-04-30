#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <C/FlyCapture2_C.h>

#include <libavutil/pixfmt.h>

#include "krad_framepool.h"
#include "krad_system.h"

typedef struct kr_fc2_St kr_fc2_t;

struct kr_fc2_St {
  fc2Version version;
  char version_str[512];

  fc2CameraInfo cam_info;

  fc2Context context;
  fc2PGRGuid guid;
  uint32_t num_cameras;
  
  fc2Image image;
  fc2Image image2;
  fc2TimeStamp prevTimestamp;  
};

void kr_fc2_capture_start (kr_fc2_t *fc);
void kr_fc2_capture_stop (kr_fc2_t *fc);

void kr_fc2_capture_image (kr_fc2_t *fc, krad_frame_t *frame);

kr_fc2_t *kr_fc2_create ();
int kr_fc2_destroy (kr_fc2_t **fc);
