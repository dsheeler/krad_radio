#include "krad_transponder.h"

/*
#include "adapters/alsa.c"
instance per card, can hold and set rate/ duplex,period etc
create options: card, samplerate, duplex, period,

#include "adapters/wayland.c"
instance per compositor, multiple windows
create options: compositor path, window size

#include "adapters/x11.c"
instance per path..
create options: path, res, x/y pos

#include "adapters/flycap.c"

#include "adapters/v4l2.c"
instance per camera, can hold and open in frame_size/frame_rate
create options: device/fps/frame size

#include "adapters/decklink.c"
instance per card, can set * and open
// create options: card, res/fps a+v, connector

#include "adapters/video_encoder.c"
#include "adapters/audio_encoder.c"
allways unique instance

#include "adapters/krshmapi.c"
//create options: name .. unique adapter instance per path?
*/
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
} adapter_handle;

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
} adapter_path;

struct kr_transponder_path {
  kr_transponder_path_info info;
  adapter_path *adapter_path;
  kr_mixer_path *mixer_path;
	kr_compositor_path *compositor_path;
  kr_adapter *adapter;
  kr_transponder *xpdr;
};

struct kr_adapter {
  kr_adapter_api api;
  kr_adapter_info info;
  adapter_handle adapter;
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

  if ((setup->direction != KR_XPDR_INPUT)
      && (setup->direction != KR_XPDR_OUTPUT)) {
    return -1;
  }
  if (setup->adapter != KR_ADP_JACK) return -1;

  if (memchr(setup->name + 1, '\0', sizeof(setup->name) - 1) == NULL) {
    return -2;
  }

  if (strlen(setup->name) == 0) return -3;

  return 0;
}

static void path_create(kr_xpdr_path *path, kr_xpdr_path_setup *setup) {
  //find or create adapter instance
  //
  //create adapter path
  //
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

  setup.direction = KR_XPDR_INPUT;
  setup.adapter = KR_ADP_JACK;
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
