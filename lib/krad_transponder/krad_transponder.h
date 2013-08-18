#ifndef KRAD_TRANSPONDER_H
#define KRAD_TRANSPONDER_H

#define KR_XPDR_PATHS_MAX 32

typedef struct kr_transponder kr_transponder;
typedef struct kr_transponder kr_xpdr;
typedef struct kr_transponder_setup kr_transponder_setup;
typedef struct kr_transponder_info_cb_arg kr_transponder_info_cb_arg;
typedef void (kr_transponder_info_cb)(kr_transponder_info_cb_arg *);

typedef struct kr_transponder_path kr_xpdr_path;
typedef struct kr_transponder_path_setup kr_xpdr_path_setup;

typedef struct kr_transponder_path kr_transponder_path;
typedef struct kr_transponder_path_setup kr_transponder_path_setup;

#include "krad_transponder_common.h"
#include "krad_adapter.h"
#include "krad_mixer.h"
#include "krad_compositor.h"

struct kr_transponder_setup {
  kr_mixer *mixer;
  kr_compositor *compositor;
  void *user;
  kr_transponder_info_cb *cb;
};

struct kr_transponder_path_setup {
  kr_transponder_path_info info;
  void *user;
  void *cb;
};

int kr_transponder_info_fill(kr_transponder *xpdr, kr_xpdr_info *info);

int kr_transponder_mkpath(kr_transponder *xpdr, kr_xpdr_path_setup *setup);
int kr_transponder_unlink(kr_xpdr_path *path);
kr_transponder *kr_transponder_create(kr_transponder_setup *setup);
int kr_transponder_destroy(kr_transponder *transponder);

#endif
