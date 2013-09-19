typedef struct {
  kr_wayland_event event;
  void *user;
} kr_wayland_cb_arg;

int wayland_adapter_path_av_cb(kr_wayland_cb_arg *arg) {

  kr_adapter_path_av_cb_arg cb_arg;
  kr_image image;

  image.w = 1280;
  image.h = 720;
  image.px = arg->event.frame_event.buffer;

  cb_arg.path = (kr_adapter_path *)arg->user;
  cb_arg.user = cb_arg.path->user;
  cb_arg.image = image;
  cb_arg.path->av_cb(&cb_arg);

  /*
  if (((time(NULL)) % 5) == 0) {
    uint32_t *p;
    int i;
    int end;
    int offset;
    p = (uint32_t *)image.px;
    end = image.w * image.h;
    offset = rand() >> 4;
    for (i = 0; i < end; i++) {
      p[i] = (i + offset) * 0x0080401;
    }
    return 1;
  }
  */
  return 1;
}

void wayland_adapter_path_event_cb(kr_wayland_cb_arg *arg) {
  /*printk("wayland adapter path event cb happens!");*/
  kr_adapter_path_event_cb_arg cb_arg;
  cb_arg.path = (kr_adapter_path *)arg->user;
  cb_arg.user = cb_arg.path->user;
  cb_arg.path->ev_cb(&cb_arg);
}

void wayland_adapter_event_cb(kr_wayland_cb_arg *arg) {
  /*printk("wayland adapter event cb happens!");*/
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
    return wayland_adapter_path_av_cb(&cb_arg);
  } else {
    wayland_adapter_path_event_cb(&cb_arg);
  }
  return 0;
}

int wayland_adapter_process(kr_adapter *adapter) {
  krad_system_set_thread_name("kr_wayland");
  for(;;) {
    kr_wayland_process(adapter->handle.wayland);
  }
  return 0;
}

void wayland_adapter_path_destroy(kr_adapter_path *path) {
  kr_wayland_unlink(&path->api_path.wayland);
}

void wayland_adapter_path_create(kr_adapter_path *path) {

  printk("wayland adapter path create happens!");

  kr_wayland_path_setup wps;

  wps.callback = wayland_adapter_path_cb;
  wps.user = path;
  wps.info = path->info.info.wayland;
  path->api_path.wayland = kr_wayland_mkpath(path->adapter->handle.wayland,
   &wps);
}

void wayland_adapter_destroy(kr_adapter *adapter) {
  printk("wayland adapter destroy happens!");
  kr_wayland_destroy(&adapter->handle.wayland);
}

void wayland_adapter_create(kr_adapter *adapter) {

  printk("wayland adapter create happens!");
  kr_wayland_setup wayland_setup;

  wayland_setup.info = adapter->info.api_info.wayland;
  adapter->handle.wayland = kr_wayland_create(&wayland_setup);
  adapter->process_function = wayland_adapter_process;
  adapter_process_thread_start(adapter);
}
