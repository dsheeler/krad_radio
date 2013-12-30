int alsa_audio_cb(void *arg, void *buffer, int frames) {
  kr_adapter_path *path;
  kr_adapter_path_av_cb_arg cb_arg;
  path = (kr_adapter_path *)arg;
  cb_arg.path = path;
  cb_arg.user = cb_arg.path->user;
  cb_arg.path->av_cb(&cb_arg);
  return 0;
}

void alsa_adapter_path_destroy(kr_adapter_path *path) {
  return;
}

void alsa_adapter_path_create(kr_adapter_path *path) {
  /*
  kr_alsa *alsa;
  kr_alsa_path_info info;
  alsa = path->adapter->handle.alsa;
  memset(&info, 0, sizeof(alsa_path_info));
  info = path->info.info.alsa;
  alsa->user = path;
  alsa->audio_cb = alsa_audio_cb;
  kr_alsa_mkpath(alsa);
  */
}

void alsa_adapter_destroy(kr_adapter *adapter) {
  kr_alsa_destroy(adapter->handle.alsa);
  adapter->handle.alsa = NULL;
}

void alsa_adapter_create(kr_adapter *adapter) {
  kr_alsa_info info;
  memset(&info, 0, sizeof(kr_alsa_info));
  info.card = adapter->info.api_info.alsa.card;
  adapter->handle.alsa = kr_alsa_create(info.card);
}
