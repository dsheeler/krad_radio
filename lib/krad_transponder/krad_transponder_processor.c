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
}

void xpdr_adapter_av_cb(kr_adapter_av_cb_arg *arg) {

  kr_xpdr *xpdr;
  uint32_t ret;

  xpdr = (kr_xpdr *)arg->user;

  kr_adapter_prepare(arg->adapter);

  /* if we are the audio or video clock do a process .. */
  if (1) {
    /* FIXME FIXME FIXME need to reconcile period size */
    ret = kr_mixer_process(xpdr->mixer);
    if (ret > 0) {
      /* printk("mixed %u frames", ret); */
      /* number of frames mixed, so time can be dealt with */
    }
  }
}
