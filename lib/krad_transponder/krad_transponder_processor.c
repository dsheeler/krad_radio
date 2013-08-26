void xpdr_compositor_path_frame_cb(kr_compositor_path_frame_cb_arg *arg) {
  /* placeholder... */
}

void xpdr_mixer_path_audio_cb(kr_mixer_path_audio_cb_arg *arg) {

  kr_xpdr_path *path;

  path = (kr_xpdr_path *)arg->user;

  printk("yay! xpdr mixer path audio cb!");

  /* This will pull from the last push(s) in the case of mixer input */
  /* This will push to adapters in the case of mixer output */
}

void xpdr_adapter_path_av_cb(kr_adapter_path_av_cb_arg *arg) {

  kr_xpdr_path *path;

  path = (kr_xpdr_path *)arg->user;

  printk("yay adapter path av cb!");

  /* This will push to some tube space with pointers for adapter input */
  path->audio = arg->audio;

  if (path->audio.count > 0) {
    printk("pushed %u audio frames!", path->audio.count);
  }

  /* if it is an adapter wanting output, it will pull from the tube space */
}

void xpdr_adapter_av_cb(kr_adapter_av_cb_arg *arg) {

  kr_xpdr *xpdr;
  uint32_t ret;

  xpdr = (kr_xpdr *)arg->user;

  /* Trigger all inputs on this adapter */
  kr_adapter_process_inputs(arg->adapter);

  /* if we are the audio or video clock do a process .. */
  if (1) {
    ret = kr_mixer_process(xpdr->mixer);
    if (ret > 0) {
      /* printk("mixed %u frames", ret); */
      /* number of frames mixed, so time can be dealt with */
    }
  }
}
