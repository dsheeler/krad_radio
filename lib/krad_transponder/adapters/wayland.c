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

int wayland_adapter_process(kr_adapter *adapter) {

  return 0;
}

void wayland_adapter_path_destroy(kr_adapter_path *path) {
  kr_wayland_unlink(&path->api_path.wayland);
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

void wayland_adapter_destroy(kr_adapter *adapter) {
  kr_wayland_destroy(&adapter->handle.wayland);
}

void wayland_adapter_create(kr_adapter *adapter) {

  kr_wayland_setup wayland_setup;

  snprintf(wayland_setup.info.server_path,
   sizeof(adapter->info.api_info.wayland.server_path), "%s", "");
  /*  memcpy(&wayland_setup.info, &setup->info.info.wayland,
   *   sizeof(kr_wayland_info)); */
  wayland_setup.user = adapter;
  adapter->handle.wayland = kr_wayland_create(&wayland_setup);

  // adapter->process_function = wayland_adapter_process;
  // adapter_process_thread_start(adapter);
}
