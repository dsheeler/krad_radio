#include "krad_jack.h"

typedef struct kr_adapter kr_adapter;
typedef struct kr_adapter_path kr_adapter_path;

typedef union {
  kr_jack *jack;
/*
  kr_alsa *alsa;
  kr_decklink *decklink;
  kr_fc2 *flycap;
  kr_jack *x11;
  kr_wayland *wayland;
  kr_v4l2 *v4l2;
  kr_encoder *encoder;
  kr_shmapi krad;
*/
} kr_adapter_handle;

typedef union {
  kr_jack_path *jack;
/*
  kr_alsa_path *alsa;
  kr_decklink_path *decklink;
  kr_fc2_path *flycap;
  kr_x11_path *x11;
  kr_wayland_path *wayland;
  kr_v4l2_path *v4l2;
  kr_encoder_path *encoder;
  kr_shmapi_path krad;
*/
} kr_adapter_api_path;

struct kr_adapter_path {
  kr_adapter_api api;
  kr_adapter_api_path api_path;
  kr_adapter *adapter;
};

struct kr_adapter {
  kr_adapter_api api;
  kr_adapter_info info;
  kr_adapter_handle handle;
  kr_transponder *xpdr;
};

/*
static void adapter_create(kr_adapter *adapter) {

  kr_jack_setup setup;

  switch (adapter->api) {
    case KR_ADP_JACK:
      snprintf(setup.client_name, sizeof(setup.client_name), "kradradio");
      snprintf(setup.server_name, sizeof(setup.server_name), "%s", "");
      adapter->handle.jack = kr_jack_create(&setup);
      break;
    default:
      break;
  }
}

static void adapter_mkpath(kr_xpdr_path *path) {

  kr_jack_path_setup setup;

  switch (path->adapter->api) {
    case KR_ADP_JACK:
      snprintf(setup.info.name, sizeof(setup.info.name), "%s",
       path->info.name);
      setup.info.channels = 2; //FIXME just this
      setup.info.direction = path->info.direction;
      path->adapter_path.jack = kr_jack_mkpath(path->adapter->handle.jack,
       &setup);
      break;
    default:
      break;
  }
}

static void adapter_setup(kr_xpdr_path *path, kr_xpdr_path_setup *setup) {

  int i;
  kr_adapter *adapter;
  kr_transponder *xpdr;

  xpdr = path->xpdr;
  path->info.adapter = setup->adapter_api;
  //find adapter instance

  for (i = 0; i < KR_XPDR_PATHS_MAX; i++) {
    if (xpdr->adapter[i] != NULL) {
      adapter = xpdr->adapter[i];
      if (adapter->api == setup->adapter_api) {
        //compare instance string
        //&& (strncmp(xpdr->adapter))) {
        //if we find it return
        path->adapter = adapter;
        return;
      }
    }
  }

  //or create it
  if (path->adapter == NULL) {
    for (i = 0; i < KR_XPDR_PATHS_MAX; i++) {
      if (xpdr->adapter[i] == NULL) {
        xpdr->adapter[i] = calloc(1, sizeof(kr_adapter));
        adapter = xpdr->adapter[i];
        adapter->xpdr = xpdr;
        adapter->api = setup->adapter_api;
        adapter_create(adapter);
        path->adapter = adapter;
        return;
      }
    }
  }
}
*/
