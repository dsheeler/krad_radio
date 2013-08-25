void test_input_create(kr_xpdr *xpdr) {

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

void test_output_create(kr_xpdr *xpdr) {

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

void test_xpdr(kr_xpdr *xpdr) {
  test_output_create(xpdr);
  test_input_create(xpdr);
}
