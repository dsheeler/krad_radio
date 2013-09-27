void test_jack_input_create(kr_xpdr *xpdr) {

  kr_xpdr_path_setup setup;
  kr_xpdr_path *path;
  char *test_name;
  char *bus_name;
  int channels;

  memset(&setup, 0, sizeof(kr_xpdr_path_setup));
  channels = 2;
  test_name = "Music";
  bus_name = "Master";
  strcpy(setup.info.name, test_name);
  setup.user = xpdr;
  setup.ev_cb = xpdr_path_event_cb;

  setup.info.input.type = KR_XPDR_ADAPTER;
  setup.info.input.info.adapter_path_info.api = KR_ADP_JACK;
  strcpy(setup.info.input.info.adapter_path_info.info.jack.name, test_name);
  setup.info.input.info.adapter_path_info.info.jack.channels = channels;
  setup.info.input.info.adapter_path_info.info.jack.direction = KR_JACK_INPUT;

  setup.info.output.type = KR_XPDR_MIXER;
  strcpy(setup.info.output.info.mixer_path_info.name, test_name);
  strcpy(setup.info.output.info.mixer_path_info.bus, bus_name);
  setup.info.output.info.mixer_path_info.channels = channels;
  setup.info.output.info.mixer_path_info.type = KR_MXR_INPUT;

  path = kr_transponder_mkpath(xpdr, &setup);
  if (path == NULL) {
    printke("could not create xpdr path");
  }
}

void test_jack_output_create(kr_xpdr *xpdr) {

  kr_xpdr_path_setup setup;
  kr_xpdr_path *path;
  char *test_name;
  char *bus_name;
  int channels;

  memset(&setup, 0, sizeof(kr_xpdr_path_setup));
  channels = 2;
  test_name = "Main";
  bus_name = "Master";
  strcpy(setup.info.name, test_name);
  setup.user = xpdr;
  setup.ev_cb = xpdr_path_event_cb;

  setup.info.input.type = KR_XPDR_MIXER;
  strcpy(setup.info.input.info.mixer_path_info.name, test_name);
  strcpy(setup.info.input.info.mixer_path_info.bus, bus_name);
  setup.info.input.info.mixer_path_info.channels = channels;
  setup.info.input.info.mixer_path_info.type = KR_MXR_OUTPUT;

  setup.info.output.type = KR_XPDR_ADAPTER;
  setup.info.output.info.adapter_path_info.api = KR_ADP_JACK;
  strcpy(setup.info.output.info.adapter_path_info.info.jack.name, test_name);
  setup.info.output.info.adapter_path_info.info.jack.channels = channels;
  setup.info.output.info.adapter_path_info.info.jack.direction = KR_JACK_OUTPUT;

  path = kr_transponder_mkpath(xpdr, &setup);
  if (path == NULL) {
    printke("could not create xpdr path");
  }
}

void test_v4l2_input_create(kr_xpdr *xpdr) {

  kr_xpdr_path_setup setup;
  kr_xpdr_path *path;
  int device_num;
  char *test_name;
  int width;
  int height;
  int num;
  int den;

  test_name = "V4L2 Test";
  device_num = 0;
  width = 640;
  height = 360;
  num = 30;
  den = 1;

  memset(&setup, 0, sizeof(kr_xpdr_path_setup));
  strcpy(setup.info.name, test_name);
  setup.user = xpdr;
  setup.ev_cb = xpdr_path_event_cb;

  setup.info.input.type = KR_XPDR_ADAPTER;
  setup.info.input.info.adapter_path_info.api = KR_ADP_V4L2;
  // setup.info.input.info.adapter_path_info.info.v4l2.dev = device_num;
  setup.info.input.info.adapter_path_info.info.v4l2.num = num;
  setup.info.input.info.adapter_path_info.info.v4l2.den = den;
  setup.info.input.info.adapter_path_info.info.v4l2.width = width;
  setup.info.input.info.adapter_path_info.info.v4l2.height = height;

  setup.info.output.type = KR_XPDR_COMPOSITOR;
  strcpy(setup.info.output.info.compositor_path_info.name, test_name);
  setup.info.output.info.compositor_path_info.width = width;
  setup.info.output.info.compositor_path_info.height = height;
  setup.info.output.info.compositor_path_info.type = KR_CMP_INPUT;

  path = kr_transponder_mkpath(xpdr, &setup);
  if (path == NULL) {
    printke("could not create xpdr path");
  }
}

void test_wayland_output_create(kr_xpdr *xpdr) {

  kr_xpdr_path_setup setup;
  kr_xpdr_path *path;
  char *test_name;
  char *display_name;
  int width;
  int height;

  display_name = "";
  test_name = "Wayland Test";
  width = 1280;
  height = 720;

  memset(&setup, 0, sizeof(kr_xpdr_path_setup));
  strcpy(setup.info.name, test_name);
  setup.user = xpdr;
  setup.ev_cb = xpdr_path_event_cb;

  setup.info.input.type = KR_XPDR_COMPOSITOR;
  strcpy(setup.info.input.info.compositor_path_info.name, test_name);
  setup.info.input.info.compositor_path_info.width = width;
  setup.info.input.info.compositor_path_info.height = height;
  setup.info.input.info.compositor_path_info.type = KR_CMP_OUTPUT;

  setup.info.output.type = KR_XPDR_ADAPTER;
  setup.info.output.info.adapter_path_info.api = KR_ADP_WAYLAND;
  strcpy(setup.info.output.info.adapter_path_info.info.wayland.display_name,
   display_name);
  setup.info.output.info.adapter_path_info.info.wayland.width = width;
  setup.info.output.info.adapter_path_info.info.wayland.height = height;

  path = kr_transponder_mkpath(xpdr, &setup);
  if (path == NULL) {
    printke("could not create xpdr path");
  }
}

void test_xpdr(kr_xpdr *xpdr) {
/*  test_jack_output_create(xpdr);
  test_jack_input_create(xpdr);*/
  test_v4l2_input_create(xpdr);
  test_wayland_output_create(xpdr);
}
