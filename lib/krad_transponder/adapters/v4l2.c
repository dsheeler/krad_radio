/*
void v4l2_adapter_path_event_cb(kr_v4l2_cb_arg *arg) {
  kr_adapter_path_event_cb_arg cb_arg;
  cb_arg.path = (kr_adapter_path *)arg->user;
  cb_arg.user = cb_arg.path->user;
  cb_arg.path->ev_cb(&cb_arg);
}

void v4l2_adapter_process_cb(kr_v4l2_cb_arg *arg) {
  kr_adapter_av_cb_arg cb_arg;
  cb_arg.adapter = (kr_adapter *)arg->user;
  cb_arg.user = cb_arg.adapter->user;
  cb_arg.adapter->av_cb(&cb_arg);
}

void v4l2_adapter_event_cb(kr_v4l2_cb_arg *arg) {
  kr_adapter_event_cb_arg cb_arg;
  cb_arg.adapter = (kr_adapter *)arg->user;
  cb_arg.user = cb_arg.adapter->user;
  cb_arg.adapter->ev_cb(&cb_arg);
}
*/

int v4l2_adapter_process(kr_adapter_path *path) {
  kr_adapter_path_av_cb_arg cb_arg;
  kr_image image;
  int ret;
  krad_system_set_thread_name("kr_v4l2");
  kr_v4l2_capture(path->adapter->handle.v4l2, 1);
  for(;;) {
    ret = kr_v4l2_read(path->adapter->handle.v4l2, &image);
    if (ret == 1) {
      cb_arg.path = path;
      cb_arg.user = cb_arg.path->user;
      cb_arg.image = image;
      cb_arg.path->av_cb(&cb_arg);
      printk("wee!");
    } else {
      usleep(25000);
    }
  }
  return 0;
}

void v4l2_adapter_path_destroy(kr_adapter_path *path) {
  kr_v4l2_capture(path->adapter->handle.v4l2, 0);
  //stop the thread
}

void v4l2_adapter_path_create(kr_adapter_path *path) {

  int ret;
  kr_v4l2_mode mode;

  mode = path->info.info.v4l2;
  ret = kr_v4l2_mode_set(path->adapter->handle.v4l2, &mode);
  if (ret == 0) {
    adapter_path_process_thread_start(path);
  }
}

void v4l2_adapter_destroy(kr_adapter *adapter) {
  kr_v4l2_destroy(adapter->handle.v4l2);
  adapter->handle.v4l2 = NULL;
}

void v4l2_adapter_create(kr_adapter *adapter) {

  kr_v4l2_setup setup;

  memset(&setup, 0, sizeof(kr_v4l2_setup));
  setup.dev = 0;
  setup.priority = 0;
  adapter->handle.v4l2 = kr_v4l2_create(&setup);
  adapter->path_process_function = v4l2_adapter_process;
}
