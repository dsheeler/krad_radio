#ifdef KRAD_USE_WAYLAND

int wayland_display_unit_render_callback(void *user, kr_wayland_event *event) {

  krad_link_t *krad_link = (krad_link_t *)user;

  int ret;
  char buffer[1];
  int updated;
  krad_frame_t *krad_frame;

  updated = 0;

  krad_frame = krad_compositor_port_pull_frame(krad_link->krad_compositor_port2);

  if (krad_frame != NULL) {
    //FIXME do this first etc
    ret = read(krad_link->krad_compositor_port2->socketpair[1], buffer, 1);
    if (ret != 1) {
      if (ret == 0) {
        printk("Krad OTransponder: port read got EOF");
        return updated;
      }
      printk("Krad OTransponder: port read unexpected read return value %d", ret);
    }

    memcpy(event->frame_event.buffer,
           krad_frame->pixels,
           krad_link->composite_width * krad_link->composite_height * 4);

    krad_framepool_unref_frame(krad_frame);
    updated = 1;
  }
  return updated;
}

int wayland_display_unit_callback(void *user, kr_wayland_event *event) {
  switch (event->type) {
    case KR_WL_FRAME:
      return wayland_display_unit_render_callback(user, event);
    case KR_WL_POINTER:
      break;
    case KR_WL_KEY:
      break;
  }
  return 0;
}

void wayland_display_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  kr_wayland_window_params window_params;

  krad_system_set_thread_name ("kr_wl_dsp");

  printk ("Wayland display thread begins");

  krad_link->wayland = kr_wayland_create();

  krad_link->krad_compositor_port2 = krad_compositor_port_create(krad_link->krad_radio->compositor, "WLOut", KR_VOUT,
                                                                krad_link->composite_width,
                                                                krad_link->composite_height);

  window_params.width = krad_link->composite_width;
  window_params.height = krad_link->composite_height;
  window_params.callback = wayland_display_unit_callback;
  window_params.user = krad_link;

  krad_link->window = kr_wayland_window_create(krad_link->wayland, &window_params);

  printk("Wayland display running");
}

int wayland_display_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  kr_wayland_process(krad_link->wayland);

  return 0;
}

void wayland_display_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  kr_wayland_window_destroy(&krad_link->window);
}

#endif
