#include "krad_transponder.h"

#include "adapters/jack.c"

typedef struct kr_adapter kr_adapter;

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

typedef enum {
  KR_XPDR_MIXER,
  KR_XPDR_COMPOSITOR,
  KR_XPDER_ADAPTER
} kr_transponder_path_io_type;

typedef union {
  kr_mixer_path *mixer_path;
	kr_compositor_path *compositor_path;
  kr_adapter_path *adapter_path;
} kr_transponder_path_io_handle;

struct kr_transponder_path_io {
  kr_transponder_path_io_type type;
  kr_transponder_path_io_handle handle;
};

struct kr_transponder_path {
  kr_transponder_path_info info;
  kr_transponder_path_io input;
  kr_transponder_path_io output;
  kr_transponder *xpdr;
};

struct kr_transponder {
  kr_mixer *mixer;
  kr_compositor *compositor;
  kr_transponder_info info;
  kr_adapter *adapter[KR_XPDR_PATHS_MAX];
  kr_transponder_path *path[KR_XPDR_PATHS_MAX];
};

static int path_setup_check(kr_xpdr_path_setup *setup);
static void path_create(kr_xpdr_path *path, kr_xpdr_path_setup *setup);
static void path_destroy(kr_xpdr_path *path);

static int path_setup_check(kr_xpdr_path_setup *setup) {

  if (setup->adapter_api != KR_ADP_JACK) return -1;

  if (memchr(setup->name + 1, '\0', sizeof(setup->name) - 1) == NULL) {
    return -2;
  }

  if (strlen(setup->name) == 0) return -3;

  return 0;
}

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

static void path_adapter_mkpath(kr_xpdr_path *path) {

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

static void path_adapter_setup(kr_xpdr_path *path, kr_xpdr_path_setup *setup) {

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

static void path_create(kr_xpdr_path *path, kr_xpdr_path_setup *setup) {

  int i;

  strncpy(path->info.name, setup->name, sizeof(path->info.name));
  path_adapter_setup(path, setup);
  if (path->adapter == NULL) {
    printke("we failed to create the adapter");
    return;
  }

  path_adapter_mkpath(path);
  if (path->adapter_path.jack == NULL) {
    printke("we failed to create the adapter path");
    return;
  }

  //crate mixer/compositor path
}

static void path_destroy(kr_xpdr_path *path) {

  // destroy compositor/mixer path
  // destroy adapter path

}

int kr_transponder_info_fill(kr_transponder *xpdr, kr_xpdr_info *info) {
  if ((xpdr == NULL) || (info == NULL)) return -1;
  memcpy(info, &xpdr->info, sizeof(kr_transponder_info));
  return 0;
}

int kr_transponder_mkpath(kr_transponder *xpdr, kr_xpdr_path_setup *setup) {

  int i;
  kr_transponder_path *path;

  if ((xpdr == NULL) || (setup == NULL)) return -1;

  path = NULL;

  for (i = 0; i < KR_XPDR_PATHS_MAX; i++) {
    if (xpdr->path[i] == NULL) {
      if (path_setup_check(setup)) {
        return -2;
      }
      path = calloc(1, sizeof(kr_transponder_path));
      xpdr->path[i] = path;
      break;
    }
  }

  if (path == NULL) return -3;

  path->xpdr = xpdr;
  path_create(path, setup);

  xpdr->info.active_paths++;

  return 0;
}

int kr_transponder_unlink(kr_xpdr_path *path) {

  int i;
  kr_transponder *xpdr;

  if (path == NULL) return -1;
  xpdr = path->xpdr;

  for (i = 0; i < KR_XPDR_PATHS_MAX; i++) {
    if (xpdr->path[i] == path) {
      path_destroy(path);
      xpdr->path[i] = NULL;
      xpdr->info.active_paths--;
      return 0;
    }
  }
  return -1;
}

int kr_transponder_destroy(kr_transponder *xpdr) {

  int i;

  if (xpdr == NULL) return -1;

  printk("Krad Transponder: Destroy Started");

  for (i = 0; i < KR_XPDR_PATHS_MAX; i++) {
    if (xpdr->path[i] != NULL) {
      kr_transponder_unlink(xpdr->path[i]);
    }
  }

  free(xpdr);

  printk("Krad Transponder: Destroy Completed");

  return 0;
}

void temp_test(kr_transponder *xpdr) {

  kr_transponder_path_setup setup;

  setup.adapter_api = KR_ADP_JACK;
  snprintf(setup.name, sizeof(setup.name), "Test Path");
  snprintf(setup.adapter_instance, sizeof(setup.adapter_instance), "%s", "");

  kr_transponder_mkpath(xpdr, &setup);
}

kr_transponder *kr_transponder_create(kr_transponder_setup *setup) {

  kr_transponder *xpdr;

  if (setup == NULL) return NULL;

  xpdr = calloc(1, sizeof(kr_transponder));

  xpdr->mixer = setup->mixer;
  xpdr->compositor = setup->compositor;

  temp_test(xpdr);

  return xpdr;
}
