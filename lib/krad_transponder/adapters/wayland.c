typedef struct {
  kr_wayland_event event;
  void *user;
} kr_wayland_cb_arg;

void wayland_adapter_path_av_cb(kr_wayland_cb_arg *arg) {
  kr_adapter_path_av_cb_arg cb_arg;
  kr_image image;

  image.w = 1280;
  image.h = 720;
  image.px = arg->event.frame_event.buffer;

  cb_arg.path = (kr_adapter_path *)arg->user;
  cb_arg.user = cb_arg.path->user;
  cb_arg.image = image;
  cb_arg.path->av_cb(&cb_arg);
}

void wayland_adapter_path_event_cb(kr_wayland_cb_arg *arg) {
  kr_adapter_path_event_cb_arg cb_arg;
  cb_arg.path = (kr_adapter_path *)arg->user;
  cb_arg.user = cb_arg.path->user;
  cb_arg.path->ev_cb(&cb_arg);
}

void wayland_adapter_process_cb(kr_wayland_cb_arg *arg) {
  kr_adapter_av_cb_arg cb_arg;
  cb_arg.adapter = (kr_adapter *)arg->user;
  cb_arg.user = cb_arg.adapter->user;
  cb_arg.adapter->av_cb(&cb_arg);
}

void wayland_adapter_event_cb(kr_wayland_cb_arg *arg) {
  kr_adapter_event_cb_arg cb_arg;
  cb_arg.adapter = (kr_adapter *)arg->user;
  cb_arg.user = cb_arg.adapter->user;
  cb_arg.adapter->ev_cb(&cb_arg);
}

int wayland_adapter_path_cb(void *user, kr_wayland_event *event) {

  kr_wayland_cb_arg cb_arg;
  cb_arg.event = *event;
  cb_arg.user = user;

  if (cb_arg.event.type == KR_WL_FRAME) {
    wayland_adapter_path_av_cb(&cb_arg);
  } else {
    wayland_adapter_path_event_cb(&cb_arg);
  }
  return 0;
}

void wayland_adapter_path_create(kr_adapter_path *path) {

  kr_wayland_path_setup wps;

  wps.info.width = 1280;
  wps.info.height = 720;
  wps.callback = wayland_adapter_path_cb;
  wps.user = path;
  memcpy(&wps.info, &path->info.info.wayland, sizeof(kr_wayland_path_info));
  path->api_path.wayland = kr_wayland_mkpath(path->adapter->handle.wayland,
   &wps);
}

void wayland_adapter_create(kr_adapter *adapter) {

  kr_wayland_setup wayland_setup;

  snprintf(wayland_setup.info.server_path,
   sizeof(adapter->info.api_info.wayland.server_path), "%s", "");
  /*
  memcpy(&wayland_setup.info, &setup->info.info.wayland, sizeof(kr_wayland_info));
  */
  wayland_setup.user = adapter;
  /* wayland_setup.process_cb = wayland_adapter_process_cb;
     wayland_setup.event_cb = wayland_adapter_event_cb; */
  adapter->handle.wayland = kr_wayland_create(&wayland_setup);
}


/*
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
*/
