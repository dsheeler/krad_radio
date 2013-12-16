#include "krad_decklink.h"

void kr_decklink_destroy(kr_decklink *decklink) {
  int c;
  kr_decklink_stop(decklink);
  for (c = 0; c < 2; c++) {
    free(decklink->samples[c]);
  }
  free(decklink);
}

kr_decklink *kr_decklink_create(char *device) {
  int c;
  kr_decklink *decklink;
  decklink = calloc(1, sizeof(kr_decklink));
  decklink->devicenum = atoi(device);
  if (decklink->devicenum > 0) {
    sprintf(decklink->simplename, "Decklink%d", decklink->devicenum);
  } else {
    sprintf(decklink->simplename, "Decklink");
  }
  for (c = 0; c < 2; c++) {
    decklink->samples[c] = malloc(4 * 8192);
  }
  decklink->capture = krad_decklink_capture_create(decklink->devicenum);
  return decklink;
}

void kr_decklink_set_video_mode(kr_decklink *decklink, int width, int height,
 int fps_numerator, int fps_denominator) {
  printk("Krad Decklink set video mode: %dx%d - %d / %d",
   width, height, fps_numerator, fps_denominator);
  krad_decklink_capture_set_video_mode(decklink->capture, width, height,
   fps_numerator, fps_denominator);
}

void kr_decklink_set_audio_input(kr_decklink *decklink, char *audio_input) {
  krad_decklink_capture_set_audio_input(decklink->capture, audio_input);
}

void kr_decklink_set_video_input(kr_decklink *decklink, char *video_input) {
  krad_decklink_capture_set_video_input(decklink->capture, video_input);
}

void kr_decklink_set_verbose(kr_decklink *decklink, int verbose) {
  decklink->verbose = verbose;
  if (decklink->capture != NULL) {
    krad_decklink_capture_set_verbose(decklink->capture, decklink->verbose);
  }
}

void kr_decklink_start(kr_decklink *decklink) {
  krad_decklink_capture_set_video_callback(decklink->capture, decklink->image_cb);
  krad_decklink_capture_set_audio_callback(decklink->capture, decklink->audio_cb);
  krad_decklink_capture_set_callback_pointer(decklink->capture, decklink->user);
  krad_decklink_capture_set_verbose(decklink->capture, decklink->verbose);
  krad_decklink_capture_start(decklink->capture);
}

void kr_decklink_stop(kr_decklink *decklink) {
  if (decklink->capture != NULL) {
    krad_decklink_capture_stop(decklink->capture);
    decklink->capture = NULL;
  }
}

int kr_decklink_detect_devices() {
  return krad_decklink_cpp_detect_devices();
}

int kr_decklink_get_device_name(int device_num, char *device_name) {
  return krad_decklink_cpp_get_device_name(device_num, device_name);
}
