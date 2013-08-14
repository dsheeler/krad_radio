#include "krad_transponder.h"

#include "adapter.c"

typedef union {
  kr_mixer_path *mixer_path;
	kr_compositor_path *compositor_path;
  kr_adapter_path *adapter_path;
} kr_transponder_path_io_handle;

typedef struct {
  kr_transponder_path_io_type type;
  kr_transponder_path_io_handle handle;
} kr_transponder_path_io;

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
  kr_adapter *adapter[KR_XPDR_PATHS_MAX * 2];
  kr_transponder_path *path[KR_XPDR_PATHS_MAX];
};

static int path_setup_io_info_check(kr_xpdr_path_io_info *info);
static int path_setup_info_check(kr_xpdr_path_info *info);
static void path_create(kr_xpdr_path *path, kr_xpdr_path_setup *setup);
static void path_destroy(kr_xpdr_path *path);

static int path_setup_io_info_check(kr_transponder_path_io_info *info) {

  //if (info->adapter_api != KR_ADP_JACK) return -1;
  // FIXME do check ;P

  return 0;
}

static int path_setup_info_check(kr_xpdr_path_info *info) {

  if (memchr(info->name + 1, '\0', sizeof(info->name) - 1) == NULL) {
    return -2;
  }
  if (strlen(info->name) == 0) return -3;

  if (path_setup_io_info_check(&info->input)) return -4;
  if (path_setup_io_info_check(&info->output)) return -5;

  /* FIXME check that we are not connecting whatever
   * we decide to be invalid such as mixer to mixer
   * or w/e, if anything  */

  return 0;
}

static void path_create(kr_xpdr_path *path, kr_xpdr_path_setup *setup) {

//create adapter/mixer/compositor path
/*
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
*/
}

static void path_destroy(kr_xpdr_path *path) {
  // destroy compositor/mixer/adapter paths
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
      if (path_setup_info_check(&setup->info)) {
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
/*
  kr_transponder_path_setup setup;

  setup.adapter_api = KR_ADP_JACK;
  snprintf(setup.name, sizeof(setup.name), "Test Path");

  kr_transponder_mkpath(xpdr, &setup);
*/
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
