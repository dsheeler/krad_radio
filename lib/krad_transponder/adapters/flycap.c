#ifdef KRAD_USE_FLYCAP
#include "krad_fc2.h"
#endif

#ifdef KRAD_USE_FLYCAP
void flycap_capture_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  printk ("Flycap capture begins");

  krad_link->fc = kr_fc2_create ();

  krad_link->capture_width = 640;
  krad_link->capture_height = 480;

  //krad_link->krad_framepool = krad_framepool_create ( krad_link->capture_width,
  //                          krad_link->capture_height,
  //                          DEFAULT_CAPTURE_BUFFER_FRAMES);

  if ((krad_link->composite_width == 0) || (krad_link->composite_height == 0)) {
    krad_compositor_get_resolution (krad_link->krad_radio->compositor,
                                    &krad_link->composite_width,
                                    &krad_link->composite_height);
  }

  krad_link->krad_framepool = krad_framepool_create_for_upscale ( krad_link->capture_width,
                            krad_link->capture_height,
                            DEFAULT_CAPTURE_BUFFER_FRAMES,
                            krad_link->composite_width, krad_link->composite_height);

  krad_link->krad_compositor_port = krad_compositor_port_create (krad_link->krad_radio->compositor,
                                   "FC2in",
                                   KR_VIN,
                                   krad_link->capture_width,
                                   krad_link->capture_height);

  kr_fc2_capture_start (krad_link->fc);
}

int flycap_capture_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  if (krad_link->krad_ticker == NULL) {
    krad_link->krad_ticker = krad_ticker_create (krad_link->krad_radio->compositor->fps_numerator,
                      krad_link->krad_radio->compositor->fps_denominator);
    krad_ticker_start (krad_link->krad_ticker);
  } else {
    krad_ticker_wait (krad_link->krad_ticker);
  }

  krad_frame_t *frame;

  frame = krad_framepool_getframe (krad_link->krad_framepool);

  kr_fc2_capture_image (krad_link->fc, frame);

  krad_compositor_port_push_yuv_frame (krad_link->krad_compositor_port, frame);

  krad_framepool_unref_frame (frame);

  return 0;
}

void flycap_capture_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  kr_fc2_capture_stop (krad_link->fc);

  krad_compositor_port_destroy (krad_link->krad_radio->compositor,
                                krad_link->krad_compositor_port);

  krad_ticker_destroy (krad_link->krad_ticker);
  krad_link->krad_ticker = NULL;

  kr_fc2_destroy (&krad_link->fc);

  printk ("Flycap capture done");
}

#endif

