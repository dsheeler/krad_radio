static void xpdr_event_cb(kr_xpdr_event_cb_arg *arg) {

  kr_xpdr *xpdr;

  xpdr = (kr_xpdr *)arg->user;

  printk("yay xpdr event!");
}

static void xpdr_path_event_cb(kr_xpdr_path_event_cb_arg *arg) {
  printk("yay xpdr path event!");
}

static void xpdr_adapter_path_event_cb(kr_adapter_path_event_cb_arg *arg) {
  printk("yay adapter path event!");
}

static void xpdr_adapter_event_cb(kr_adapter_event_cb_arg *arg) {

  kr_xpdr *xpdr;
  uint32_t ret;

  xpdr = (kr_xpdr *)arg->user;

  /* if we are the audio or video clock do a process .. */
  if (kr_adapter_prepare(arg->adapter)) {
    /* FIXME FIXME FIXME need to reconcile period size */
    ret = kr_mixer_process(xpdr->mixer);
    if (ret > 0) {
      /* printk("mixed %u frames", ret); */
      /* number of frames mixed, so time can be dealt with */
    }
  }
}
