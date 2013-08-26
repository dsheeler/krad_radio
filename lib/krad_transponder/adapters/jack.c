void jack_adapter_path_av_cb(kr_jack_cb_arg *arg) {
  kr_adapter_path_av_cb_arg cb_arg;
  cb_arg.path = (kr_adapter_path *)arg->user;
  cb_arg.user = cb_arg.path->user;
  cb_arg.audio = arg->audio;
  cb_arg.path->av_cb(&cb_arg);
}

void jack_adapter_path_event_cb(kr_jack_cb_arg *arg) {
  kr_adapter_path_event_cb_arg cb_arg;
  cb_arg.path = (kr_adapter_path *)arg->user;
  cb_arg.user = cb_arg.path->user;
  cb_arg.path->ev_cb(&cb_arg);
}

void jack_adapter_process_cb(kr_jack_cb_arg *arg) {
  kr_adapter_av_cb_arg cb_arg;
  cb_arg.adapter = (kr_adapter *)arg->user;
  cb_arg.user = cb_arg.adapter->user;
  cb_arg.adapter->av_cb(&cb_arg);
}

void jack_adapter_event_cb(kr_jack_cb_arg *arg) {
  kr_adapter_event_cb_arg cb_arg;
  cb_arg.adapter = (kr_adapter *)arg->user;
  cb_arg.user = cb_arg.adapter->user;
  cb_arg.adapter->ev_cb(&cb_arg);
}

void jack_adapter_path_create(kr_adapter_path *path) {

  kr_jack_path_setup jps;

  jps.audio_cb = jack_adapter_path_av_cb;
  jps.event_cb = jack_adapter_path_event_cb;
  jps.user = path;
  memcpy(&jps.info, &path->info.info.jack, sizeof(kr_jack_path_info));
  path->api_path.jack = kr_jack_mkpath(path->adapter->handle.jack, &jps);
}

void jack_adapter_create(kr_adapter *adapter) {

  kr_jack_setup jack_setup;

  snprintf(jack_setup.client_name,
   sizeof(adapter->info.api_info.jack.client_name), "kradradio");
  snprintf(jack_setup.server_name,
   sizeof(adapter->info.api_info.jack.server_name), "%s", "");
  /*
  memcpy(&jack_setup.info, &setup->info.info.jack, sizeof(kr_jack_info));
  */
  jack_setup.user = adapter;
  jack_setup.process_cb = jack_adapter_process_cb;
  jack_setup.event_cb = jack_adapter_event_cb;
  adapter->handle.jack = kr_jack_create(&jack_setup);
}
