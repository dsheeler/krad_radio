void test_xpdr(kr_xpdr *xpdr) {

  kr_xpdr_path_setup setup;
  kr_xpdr_path *path;
  char *test_name;

  memset(&setup, 0, sizeof(kr_xpdr_path_setup));

  test_name = "Working";
  strcpy(setup.info.name, test_name);
  setup.user = xpdr;
  setup.ev_cb = xpdr_path_event_cb;

  setup.info.input.type = KR_XPDR_ADAPTER;
  setup.info.input.info.adapter_path_info.api = KR_ADP_JACK;
  strcpy(setup.info.input.info.adapter_path_info.info.jack.name, test_name);
  setup.info.input.info.adapter_path_info.info.jack.channels = 2;
  setup.info.input.info.adapter_path_info.info.jack.direction = KR_JACK_INPUT;

  setup.info.output.type = KR_XPDR_MIXER;
  strcpy(setup.info.output.info.mixer_path_info.name, test_name);
  setup.info.output.info.mixer_path_info.channels = 2;
  setup.info.output.info.mixer_path_info.type = KR_MXR_INPUT;

  path = kr_transponder_mkpath(xpdr, &setup);
  if (path == NULL) {
    printke("could not create xpdr path");
  }
}
