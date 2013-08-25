void xpdr_mixer_path_audio_cb(kr_mixer_path_audio_cb_arg *arg) {


  printk("yay! xpdr mixer path audio cb!");

}

void xpdr_adapter_path_av_cb(kr_adapter_path_av_cb_arg *arg) {

  printk("yay adapter path av cb!");
}

void xpdr_adapter_av_cb(kr_adapter_av_cb_arg *arg) {

  kr_xpdr *xpdr;
  uint32_t ret;

  xpdr = (kr_xpdr *)arg->user;
  if (1) { /* if we are the clock .. */
    ret = kr_mixer_process(xpdr->mixer);
    /* printk("mixed %u frames", ret); */
  }
}
