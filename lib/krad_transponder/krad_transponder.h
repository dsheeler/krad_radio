#ifndef KRAD_TRANSPONDER_H
#define KRAD_TRANSPONDER_H

#define KR_XPDR_PATHS_MAX 32

typedef struct kr_transponder kr_transponder;
typedef struct kr_transponder_setup kr_transponder_setup;
typedef struct kr_transponder_path kr_transponder_path;
typedef struct kr_transponder_path_setup kr_transponder_path_setup;
typedef struct kr_transponder_event_cb_arg kr_transponder_event_cb_arg;
typedef void (kr_transponder_event_cb)(kr_transponder_event_cb_arg *);
typedef struct kr_transponder_path_event_cb_arg kr_transponder_path_event_cb_arg;
typedef void (kr_transponder_path_event_cb)(kr_transponder_path_event_cb_arg *);

typedef struct kr_transponder kr_xpdr;
typedef struct kr_transponder_setup kr_xpdr_setup;
typedef struct kr_transponder_path kr_xpdr_path;
typedef struct kr_transponder_path_setup kr_xpdr_path_setup;
typedef struct kr_transponder_event_cb_arg kr_xpdr_event_cb_arg;
typedef void (kr_xpdr_event_cb)(kr_xpdr_event_cb_arg *);
typedef struct kr_transponder_path_event_cb_arg kr_xpdr_path_event_cb_arg;
typedef void (kr_xpdr_path_event_cb)(kr_xpdr_path_event_cb_arg *);

#include "krad_transponder_common.h"
#include "krad_adapter.h"
#include "krad_mixer.h"
#include "krad_compositor.h"

struct kr_transponder_event_cb_arg {
  void *user;
  /* EVENT INFO */
};

struct kr_transponder_path_event_cb_arg {
  void *user;
  kr_transponder_path *path;
  /* PATH EVENT INFO */
};

struct kr_transponder_setup {
  kr_mixer *mixer;
  kr_compositor *compositor;
  void *user;
  kr_transponder_event_cb *ev_cb;
};

struct kr_transponder_path_setup {
  kr_transponder_path_info info;
  void *user;
  kr_transponder_path_event_cb *ev_cb;
};

int kr_transponder_get_info(kr_transponder *xpdr, kr_xpdr_info *info);

kr_xpdr_path *kr_transponder_mkpath(kr_xpdr *xpdr, kr_xpdr_path_setup *setup);
int kr_transponder_unlink(kr_xpdr_path *path);
kr_transponder *kr_transponder_create(kr_transponder_setup *setup);
int kr_transponder_destroy(kr_transponder *transponder);

#endif
