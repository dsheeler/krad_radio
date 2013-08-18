#include "krad_adapter.h"

typedef union {
  void *exists;
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
  kr_adapter_path *path[48];
  void *user;
  void *cb;
};

int kr_adapter_unlink(kr_adapter_path *path) {

  if (path == NULL) return -1;


  return 0;
}

kr_adapter_path *kr_adapter_mkpath(kr_adapter *adapter,
 kr_adapter_path_setup *setup) {

  kr_adapter_path *path;
  kr_jack_path_setup jack_setup;

  path = NULL;

  if (adapter == NULL) return NULL;
  if (setup == NULL) return NULL;
  if (adapter->handle.exists == NULL) return NULL;
  if (adapter->api != setup->info.api) return NULL;

  //find path in adapter paths array

  switch (path->adapter->api) {
    case KR_ADP_JACK:
      snprintf(jack_setup.info.name, sizeof(jack_setup.info.name), "%s",
       setup->info.name);
      /* FIXME the following things.. */
      jack_setup.info.channels = 2;
      jack_setup.info.direction = KR_JACK_INPUT;
      /* FIXME end of problems */
      path->api_path.jack = kr_jack_mkpath(adapter->handle.jack,
       &jack_setup);
      return path;
    default:
      break;
  }
  return NULL;
}

int kr_adapter_get_info(kr_adapter *adapter, kr_adapter_info *info) {

  if (adapter == NULL) return -1;
  if (info == NULL) return -2;
  memcpy(&adapter->info, info, sizeof(kr_adapter_info));
  return 0;
}

int kr_adapter_destroy(kr_adapter *adapter) {

  if (adapter == NULL) return -1;

  return 0;
}

kr_adapter *kr_adapter_create(kr_adapter_setup *setup2) {

  kr_adapter *adapter;
  kr_jack_setup setup;

  adapter = calloc(1, sizeof(kr_adapter));

  switch (setup2->info.api) {
    case KR_ADP_JACK:
      snprintf(setup.client_name, sizeof(setup.client_name), "kradradio");
      snprintf(setup.server_name, sizeof(setup.server_name), "%s", "");
      adapter->handle.jack = kr_jack_create(&setup);
      return adapter;
    default:
      break;
  }
  return NULL;
}
