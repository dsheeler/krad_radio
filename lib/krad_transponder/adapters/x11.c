#ifdef KR_LINUX
#ifdef KRAD_USE_X11
void x11_capture_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_system_set_thread_name ("kr_x11_cap");

  printk ("X11 capture thread begins");

  krad_link->krad_x11 = krad_x11_create();

  if (krad_link->video_source == X11) {
    krad_link->krad_framepool = krad_framepool_create ( krad_link->krad_x11->screen_width,
                              krad_link->krad_x11->screen_height,
                              DEFAULT_CAPTURE_BUFFER_FRAMES);
  }

  krad_x11_enable_capture (krad_link->krad_x11, 0);

  krad_link->krad_compositor_port = krad_compositor_port_create (krad_link->krad_radio->compositor,
                                   "X11In",
                                   KR_VIN,
                                   krad_link->krad_x11->screen_width, krad_link->krad_x11->screen_height);
}

int x11_capture_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_frame_t *krad_frame;

  if (krad_link->krad_ticker == NULL) {
    krad_link->krad_ticker = krad_ticker_create (krad_link->krad_radio->compositor->fps_numerator,
                      krad_link->krad_radio->compositor->fps_denominator);
    krad_ticker_start (krad_link->krad_ticker);
  } else {
    krad_ticker_wait (krad_link->krad_ticker);
  }

  krad_frame = krad_framepool_getframe (krad_link->krad_framepool);

  krad_x11_capture (krad_link->krad_x11, (unsigned char *)krad_frame->pixels);

  krad_compositor_port_push_rgba_frame (krad_link->krad_compositor_port, krad_frame);

  krad_framepool_unref_frame (krad_frame);

  return 0;
}

void x11_capture_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_compositor_port_destroy (krad_link->krad_radio->compositor, krad_link->krad_compositor_port);

  krad_ticker_destroy (krad_link->krad_ticker);
  krad_link->krad_ticker = NULL;

  krad_x11_destroy (krad_link->krad_x11);

  printk ("X11 capture thread exited");
}

#endif
#endif
