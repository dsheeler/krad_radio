void xpdr_mixer_path_audio_cb(kr_mixer_path_audio_cb_arg *arg) {


  printk("yay! xpdr mixer path audio cb!");

}

void xpdr_adapter_path_av_cb(kr_adapter_path_av_cb_arg *arg) {

  printk("yay adapter path av cb!");
}

void xpdr_adapter_av_cb(kr_adapter_av_cb_arg *arg) {

  printk("yay adapter av cb!");
}
