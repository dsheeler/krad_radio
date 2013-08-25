/*
static void xpdr_event_cb(kr_xpdr_event_cb_arg *arg) {

  kr_xpdr *xpdr;

  xpdr = (kr_xpdr *)arg->user;

  printk("yay xpdr event!");
}
*/

static void xpdr_path_event_cb(kr_xpdr_path_event_cb_arg *arg) {
  printk("yay xpdr path event!");
}

static void xpdr_adapter_event_cb(kr_adapter_event_cb_arg *arg) {
  printk("yay adapter event!");
}

static void xpdr_adapter_path_event_cb(kr_adapter_path_event_cb_arg *arg) {
  printk("yay adapter path event!");
}
