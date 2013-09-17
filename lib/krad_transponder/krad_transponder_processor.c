void xpdr_compositor_path_frame_cb(kr_compositor_path_frame_cb_arg *arg) {
  /* placeholder... */
}

void xpdr_mixer_path_audio_cb(kr_mixer_path_audio_cb_arg *arg) {
  kr_xpdr_path *path;
  path = (kr_xpdr_path *)arg->user;
  arg->audio = path->audio;
}

void xpdr_adapter_path_av_cb(kr_adapter_path_av_cb_arg *arg) {
  kr_xpdr_path *path;
  path = (kr_xpdr_path *)arg->user;
  path->audio = arg->audio;
/*
  if (arg->put == 1) {
    memcpy(path->pixels, arg->image.px, 640 * 480);
  } else {
    memcpy(arg->image.px, path->pixels, 640 * 480);
  }
*/
}
