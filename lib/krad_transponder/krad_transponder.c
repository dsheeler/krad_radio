#include "krad_transponder.h"

typedef union {
  void *exists;
  kr_mixer_path *mixer_path;
	kr_compositor_path *compositor_path;
  kr_adapter_path *adapter_path;
} kr_xpdr_path_io;

struct kr_transponder_path {
  kr_transponder_path_info info;
  kr_xpdr_path_io input;
  kr_xpdr_path_io output;
  void *user;
  void *cb;
  kr_transponder *xpdr;
};

struct kr_transponder {
  kr_mixer *mixer;
  kr_compositor *compositor;
  kr_transponder_info info;
  kr_adapter *adapter[KR_XPDR_PATHS_MAX * 2];
  kr_transponder_path *path[KR_XPDR_PATHS_MAX];
};

static int path_io_info_check(kr_xpdr_path_io_info *info);
static int path_info_check(kr_xpdr_path_info *info);
static int path_setup_check(kr_xpdr_path_setup *setup);
static void path_create(kr_xpdr_path *path, kr_xpdr_path_setup *setup);
static void path_destroy(kr_xpdr_path *path);
static kr_adapter *find_adapter(kr_xpdr *xpdr, kr_adapter_path_setup *setup);
static kr_adapter *get_adapter(kr_xpdr *xpdr, kr_adapter_path_setup *setup);

static kr_adapter *find_adapter(kr_xpdr *xpdr, kr_adapter_path_setup *setup) {

  int i;
  kr_adapter *adapter;
  kr_adapter_info info;

  adapter = NULL;

  //find adapter instance
  for (i = 0; i < KR_XPDR_PATHS_MAX * 2; i++) {
    if (xpdr->adapter[i] != NULL) {
      adapter = xpdr->adapter[i];
      if (kr_adapter_get_info(adapter, &info)) return NULL;
      if (info.api == setup->info.api) {
        //compare instance string
        //&& (strncmp(xpdr->adapter))) {
        //if we find it return
        return adapter;
      }
    }
  }
  return NULL;
}

static kr_adapter *get_adapter(kr_xpdr *xpdr, kr_adapter_path_setup *setup) {

  int i;
  kr_adapter *adapter;
  kr_adapter_setup adapter_setup;

  /* Find adapter instance */
  adapter = find_adapter(xpdr, setup);
  if (adapter) return adapter;

  /* Otherwise create it */
  memset(&adapter_setup, 0, sizeof(kr_adapter_setup));
  for (i = 0; i < KR_XPDR_PATHS_MAX; i++) {
    if (xpdr->adapter[i] == NULL) {
      adapter_setup.info.api = setup->info.api;
      /* FIXME need pointers  */
      adapter_setup.user = NULL;
      adapter_setup.cb = NULL;
      xpdr->adapter[i] = kr_adapter_create(&adapter_setup);
      return xpdr->adapter[i];
    }
  }
  return NULL;
}

static int path_io_info_check(kr_transponder_path_io_info *info) {

  if ((info->type != KR_XPDR_ADAPTER)
      && (info->type != KR_XPDR_MIXER)
      && (info->type != KR_XPDR_COMPOSITOR)) {
    return -1;
  }

  switch (info->type) {
    case KR_XPDR_ADAPTER:
      break;
    case KR_XPDR_MIXER:
      break;
    case KR_XPDR_COMPOSITOR:
      break;
  }

  return 0;
}

static int path_info_check(kr_xpdr_path_info *info) {

  if (memchr(info->name + 1, '\0', sizeof(info->name) - 1) == NULL) {
    return -2;
  }
  if (strlen(info->name) == 0) return -3;
  if (((info->input.type == KR_XPDR_MIXER)
      && (info->output.type == KR_XPDR_COMPOSITOR))
      || ((info->input.type == KR_XPDR_COMPOSITOR)
      && (info->output.type == KR_XPDR_MIXER))) {
    return -4;
  }
  if (path_io_info_check(&info->input)) return -5;
  if (path_io_info_check(&info->output)) return -6;
  return 0;
}

static int path_setup_check(kr_xpdr_path_setup *setup) {
  /*
  if (setup->user == NULL) return -1;
  if (setup->cb == NULL) return -2;
  */
  if (path_info_check(&setup->info)) return -3;
  return 0;
}

static void path_io_create(kr_xpdr_path *path, kr_xpdr_path_io_info *info) {

  kr_xpdr_path_io *io;
  kr_mixer *mixer;
  //kr_compositor *compositor;
  kr_adapter *adapter;
  kr_mixer_path_setup mp_setup;
  //kr_compositor_path_setup compositor_path_setup;
  kr_adapter_path_setup ap_setup;

  mixer = path->xpdr->mixer;
  //compositor = path->xpdr->compositor;

  if (path->output.exists) {
    io = &path->input;
  } else {
    io = &path->output;
  }

  switch (info->type) {
    case KR_XPDR_ADAPTER:
      memcpy(&ap_setup.info, &info->info.adapter_path_info,
       sizeof(kr_adapter_path_info));
      ap_setup.cb = NULL;
      ap_setup.user = path;
      adapter = get_adapter(path->xpdr, &ap_setup);
      if (adapter) {
        io->adapter_path = kr_adapter_mkpath(adapter, &ap_setup);
      }
      break;
    case KR_XPDR_MIXER:
      memcpy(&mp_setup.info, &info->info.mixer_path_info,
       sizeof(kr_mixer_path_info));
      mp_setup.audio_cb = NULL;
      mp_setup.user = path;
      io->mixer_path = kr_mixer_mkpath(mixer, &mp_setup);
      break;
    case KR_XPDR_COMPOSITOR:
      break;
  }
}

static void path_create(kr_xpdr_path *path, kr_xpdr_path_setup *setup) {

  memcpy(&path->info, &setup->info, sizeof(kr_transponder_path_info));
  path->user = setup->user;
  path->cb = setup->cb;

  path_io_create(path, &path->info.output);
  path_io_create(path, &path->info.input);
}

static void path_io_destroy(kr_xpdr_path_io *io, kr_xpdr_path_io_type type) {
  switch (type) {
    case KR_XPDR_ADAPTER:
      //kr_adapter_unlink(path->input.handle.adapter_path);
      break;
    case KR_XPDR_MIXER:
      kr_mixer_unlink(io->mixer_path);
      break;
    case KR_XPDR_COMPOSITOR:
      //kr_compositor_unlink(path->input.handle.compositor_path);
      break;
  }
}

static void path_destroy(kr_xpdr_path *path) {
  path_io_destroy(&path->input, path->info.input.type);
  path_io_destroy(&path->output, path->info.output.type);
}

int kr_transponder_info_fill(kr_transponder *xpdr, kr_xpdr_info *info) {
  if ((xpdr == NULL) || (info == NULL)) return -1;
  memcpy(info, &xpdr->info, sizeof(kr_transponder_info));
  return 0;
}

static kr_xpdr_path *path_alloc(kr_transponder *xpdr) {

  int i;

  for (i = 0; i < KR_XPDR_PATHS_MAX; i++) {
    if (xpdr->path[i] == NULL) {
      xpdr->path[i] = calloc(1, sizeof(kr_transponder_path));
      xpdr->path[i]->xpdr = xpdr;
      return xpdr->path[i];
    }
  }
  return NULL;
}

int kr_transponder_mkpath(kr_transponder *xpdr, kr_xpdr_path_setup *setup) {

  kr_transponder_path *path;

  if ((xpdr == NULL) || (setup == NULL)) return -1;
  if (path_setup_check(setup)) return -2;
  path = path_alloc(xpdr);
  if (path == NULL) return -3;
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

kr_transponder *kr_transponder_create(kr_transponder_setup *setup) {

  kr_transponder *xpdr;

  if (setup == NULL) return NULL;

  xpdr = calloc(1, sizeof(kr_transponder));

  xpdr->mixer = setup->mixer;
  xpdr->compositor = setup->compositor;

  return xpdr;
}
