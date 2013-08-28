#include "krad_adapter.h"

typedef union {
  void *exists;
  kr_jack *jack;
  kr_v4l2 *v4l2;
  kr_wayland *wayland;
/*
  kr_alsa *alsa;
  kr_decklink *decklink;
  kr_fc2 *flycap;
  kr_jack *x11;
  kr_encoder *encoder;
  kr_shmapi krad;
*/
} kr_adapter_handle;

typedef union {
  kr_jack_path *jack;
  kr_v4l2_path *v4l2;
  kr_wayland_path *wayland;
/*
  kr_alsa_path *alsa;
  kr_decklink_path *decklink;
  kr_fc2_path *flycap;
  kr_x11_path *x11;
  kr_encoder_path *encoder;
  kr_shmapi_path krad;
*/
} kr_adapter_api_path;

struct kr_adapter_path {
  void *user;
  kr_adapter_path_av_cb *av_cb;
  kr_adapter_path_event_cb *ev_cb;
  kr_adapter *adapter;
  kr_adapter_api_path api_path;
  kr_adapter_path_info info;
};

struct kr_adapter {
  kr_adapter_info info;
  kr_adapter_handle handle;
  kr_adapter_path *path[KR_ADAPTER_PATHS_MAX];
  void *user;
  kr_adapter_event_cb *ev_cb;
};

#include "adapters/jack.c"
#include "adapters/wayland.c"
#include "adapters/v4l2.c"

static int path_setup_check(kr_adapter_path_setup *setup);
static void path_create(kr_adapter_path *path, kr_adapter_path_setup *setup);
static void path_destroy(kr_adapter_path *path);
static int path_prepare(kr_adapter_path *path);

static int path_setup_check(kr_adapter_path_setup *setup) {
  if (setup == NULL) return -1;
  /* FIXME check things */
  return 0;
}

static void path_create(kr_adapter_path *path, kr_adapter_path_setup *setup) {

  path->user = setup->user;
  path->ev_cb = setup->ev_cb;
  path->av_cb = setup->av_cb;
  memcpy(&path->info, &setup->info, sizeof(kr_adapter_path_info));

  printk("ok we are going to create an adapter path");

  switch (path->adapter->info.api) {
    case KR_ADP_WAYLAND:
      wayland_adapter_path_create(path);
      break;
    case KR_ADP_JACK:
      jack_adapter_path_create(path);
      break;
    default:
      break;
  }
}

static void path_destroy(kr_adapter_path *path) {
  switch (path->info.api) {
    case KR_ADP_WAYLAND:
      wayland_adapter_path_destroy(path);
      break;
    case KR_ADP_JACK:
      jack_adapter_path_destroy(path);
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

/* FIXME get rid of this prepare jank */
static int path_prepare(kr_adapter_path *path) {
  switch (path->info.api) {
    case KR_ADP_JACK:
      kr_jack_path_prepare(path->api_path.jack);
      break;
    default:
      break;
  }
  return 0;
}

int kr_adapter_prepare(kr_adapter *adapter) {

  int i;
  int processed;

  processed = 0;

  for (i = 0; i < KR_ADAPTER_PATHS_MAX; i++) {
    if (adapter->path[i] != NULL) {
      path_prepare(adapter->path[i]);
      processed++;
    }
  }
  return processed;
}

int kr_adapter_unlink(kr_adapter_path *path) {

  int i;
  kr_adapter *adapter;

  if (path == NULL) return -1;
  adapter = path->adapter;

  for (i = 0; i < KR_ADAPTER_PATHS_MAX; i++) {
    if (adapter->path[i] == NULL) continue;
    if (adapter->path[i] == path) {
      path_destroy(path);
      free(path);
      adapter->path[i] = NULL;
      return 0;
    }
  }
  return -1;
}

kr_adapter_path *kr_adapter_mkpath(kr_adapter *adapter,
 kr_adapter_path_setup *setup) {

  kr_adapter_path *path;

  path = NULL;

  if (adapter == NULL) return NULL;
  if (setup == NULL) return NULL;
  if (adapter->handle.exists == NULL) return NULL;
  if (adapter->info.api != setup->info.api) return NULL;
  if (path_setup_check(setup)) return NULL;
  path = path_alloc(adapter);
  if (path == NULL) return NULL;
  path_create(path, setup);

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

  printk("Adapter destroy started");

  for (i = 0; i < KR_ADAPTER_PATHS_MAX; i++) {
    if (adapter->path[i] != NULL) {
      kr_adapter_unlink(adapter->path[i]);
    }
  }

  switch (adapter->info.api) {
    case KR_ADP_WAYLAND:
      wayland_adapter_destroy(adapter);
      break;
    case KR_ADP_JACK:
      jack_adapter_destroy(adapter);
      break;
    default:
      break;
  }

  free(adapter);

  printk("Adapter destroy completed");

  return 0;
}

kr_adapter *kr_adapter_create(kr_adapter_setup *setup) {

  kr_adapter *adapter;

  if (setup == NULL) return NULL;

  adapter = calloc(1, sizeof(kr_adapter));
  adapter->user = setup->user;
  adapter->ev_cb = setup->ev_cb;
  memcpy(&adapter->info, &setup->info, sizeof(kr_adapter_info));

  switch (adapter->info.api) {
    case KR_ADP_WAYLAND:
      wayland_adapter_create(adapter);
      return adapter;
    case KR_ADP_JACK:
      jack_adapter_create(adapter);
      return adapter;
    default:
      break;
  }
  /* Should never make it this far. */
  return NULL;
}
