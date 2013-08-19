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
  kr_adapter_api_path api_path;
  void *user;
  void *cb;
  kr_adapter_path_info info;
  kr_adapter *adapter;
};

struct kr_adapter {
  kr_adapter_info info;
  kr_adapter_handle handle;
  kr_adapter_path *path[KR_ADAPTER_PATHS_MAX];
  void *user;
  void *cb;
};

static int path_setup_check(kr_adapter_path_setup *setup);
static void path_create(kr_adapter_path *path, kr_adapter_path_setup *setup);
static void path_destroy(kr_adapter_path *path);

static int path_setup_check(kr_adapter_path_setup *setup) {
  if (setup == NULL) return -1;
  /* FIXME check things */
  return 0;
}

static void path_create(kr_adapter_path *path, kr_adapter_path_setup *setup) {

  memcpy(&path->info, &setup->info, sizeof(kr_adapter_path_info));
  path->user = setup->user;
  path->cb = setup->cb;
  kr_jack_path_setup jack_path_setup;

  printk("ok we are going to create an adapter path");

  switch (path->adapter->info.api) {
    case KR_ADP_JACK:
      memcpy(&jack_path_setup.info, &setup->info.info.jack,
       sizeof(kr_jack_path_info));
      jack_path_setup.cb = setup->cb;
      jack_path_setup.user = setup->user;
      path->api_path.jack = kr_jack_mkpath(path->adapter->handle.jack,
       &jack_path_setup);
    default:
      break;
  }
}

static void path_destroy(kr_adapter_path *path) {
  switch (path->info.api) {
    case KR_ADP_JACK:
      kr_jack_unlink(path->api_path.jack);
      break;
    default:
      break;
  }
}

static kr_adapter_path *path_alloc(kr_adapter *adapter) {

  int i;

  for (i = 0; i < KR_ADAPTER_PATHS_MAX; i++) {
    if (adapter->path[i] == NULL) {
      adapter->path[i] = calloc(1, sizeof(kr_adapter_path));
      adapter->path[i]->adapter = adapter;
      return adapter->path[i];
    }
  }
  return NULL;
}

int kr_adapter_unlink(kr_adapter_path *path) {

  int i;
  kr_adapter *adapter;

  if (path == NULL) return -1;
  adapter = path->adapter;

  for (i = 0; i < KR_ADAPTER_PATHS_MAX; i++) {
    if (adapter->path[i] == path) {
      path_destroy(path);
      adapter->path[i] = NULL;
      /* adapter->info.active_paths--; */
      return 0;
    }
  }
  return -1;
}

kr_adapter_path *kr_adapter_mkpath(kr_adapter *adapter,
 kr_adapter_path_setup *setup) {

  kr_adapter_path *path;

  path = NULL;

  printk("we got this far");

  if (adapter == NULL) return NULL;
  printk("we got this far 1");
  if (setup == NULL) return NULL;
  printk("we got this far 2");
  if (adapter->handle.exists == NULL) return NULL;
  printk("we got this far 3");
  if (adapter->info.api != setup->info.api) return NULL;

  printk("we are this close");

  if (path_setup_check(setup)) return NULL;
  path = path_alloc(adapter);
  if (path == NULL) return NULL;
  path_create(path, setup);
  /* adapter->info.active_paths++; */

  return path;
}

int kr_adapter_get_info(kr_adapter *adapter, kr_adapter_info *info) {
  if (adapter == NULL) return -1;
  if (info == NULL) return -2;
  memcpy(info, &adapter->info, sizeof(kr_adapter_info));
  return 0;
}

int kr_adapter_destroy(kr_adapter *adapter) {

  int i;

  if (adapter == NULL) return -1;

  for (i = 0; i < KR_ADAPTER_PATHS_MAX; i++) {
    if (adapter->path[i] != NULL) {
      kr_adapter_unlink(adapter->path[i]);
    }
  }

  switch (adapter->info.api) {
    case KR_ADP_JACK:
      kr_jack_destroy(adapter->handle.jack);
    default:
      break;
  }

  free(adapter);

  return 0;
}

kr_adapter *kr_adapter_create(kr_adapter_setup *setup) {

  kr_adapter *adapter;
  kr_jack_setup jack_setup;

  adapter = calloc(1, sizeof(kr_adapter));
  memcpy(&adapter->info, &setup->info, sizeof(kr_adapter_info));

  switch (adapter->info.api) {
    case KR_ADP_JACK:
      snprintf(jack_setup.client_name,
       sizeof(setup->info.api_info.jack.client_name), "kradradio");
      snprintf(jack_setup.server_name,
       sizeof(setup->info.api_info.jack.server_name), "%s", "");
      /*
      memcpy(&jack_setup.info, &setup->info.info.jack, sizeof(kr_jack_info));
      */
      jack_setup.user = setup->user;
      jack_setup.cb = setup->cb;
      adapter->handle.jack = kr_jack_create(&jack_setup);
      return adapter;
    default:
      break;
  }
  return NULL;
}
