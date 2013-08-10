#include "krad_transponder.h"

struct kr_transponder {
  kr_mixer *mixer;
  kr_compositor *compositor;
  kr_transponder_path *path[KR_XPDR_PATHS_MAX];
  kr_transponder_info info;
};

struct kr_transponder_path {
  kr_transponder_path_info info;
  krad_tags_t *krad_tags;
  kr_mixer_path *mixer_path;
  kr_transponder *xpdr;
  //krad_link_av_mode_t av_mode;
	//krad_compositor_port_t *krad_compositor_port;
};

/*
#include "adapters/wayland.c"
#include "adapters/v4l2.c"
#include "adapters/x11.c"
#include "adapters/flycap.c"
#include "adapters/decklink.c"
#include "adapters/video_encoder.c"
#include "adapters/audio_encoder.c"
*/

static void path_destroy(kr_xpdr_path *path);

static void path_destroy(kr_xpdr_path *path) {

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
      path = calloc(1, sizeof(kr_transponder_path));
      xpdr->path[i] = path;
      break;
    }
  }

  path->xpdr = xpdr;

  //look at direction and adapter api

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

kr_transponder *kr_transponder_create(kr_transponder_setup *setup) {

  kr_transponder *xpdr;

  if (setup == NULL) return NULL;

  xpdr = calloc(1, sizeof(kr_transponder));

  xpdr->mixer = setup->mixer;
  xpdr->compositor = setup->compositor;

  return xpdr;
}
