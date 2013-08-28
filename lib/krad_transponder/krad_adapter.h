#ifndef KRAD_ADAPTER_H
#define KRAD_ADAPTER_H

#define KR_ADAPTER_PATHS_MAX 32

typedef struct kr_adapter kr_adapter;
typedef struct kr_adapter_setup kr_adapter_setup;
typedef struct kr_adapter_path kr_adapter_path;
typedef struct kr_adapter_path_setup kr_adapter_path_setup;

#include "krad_adapter_common.h"
#include "krad_av.h"

#include "krad_jack.h"
#include "krad_wayland.h"
#include "krad_v4l2.h"

typedef struct {
  kr_adapter *adapter;
  void *user;
  /* ADAPTER EVENT INFO */
} kr_adapter_event_cb_arg;

typedef struct {
  kr_adapter *adapter;
  void *user;
} kr_adapter_av_cb_arg;

typedef struct {
  kr_adapter_path *path;
  void *user;
  /* ADAPTER PATH EVENT INFO */
} kr_adapter_path_event_cb_arg;

typedef struct {
  kr_adapter_path *path;
  kr_image image;
  kr_audio audio;
  void *user;
} kr_adapter_path_av_cb_arg;

typedef void (kr_adapter_event_cb)(kr_adapter_event_cb_arg *);
typedef void (kr_adapter_path_event_cb)(kr_adapter_path_event_cb_arg *);
typedef void (kr_adapter_av_cb)(kr_adapter_av_cb_arg *);
typedef void (kr_adapter_path_av_cb)(kr_adapter_path_av_cb_arg *);

struct kr_adapter_path_setup {
  kr_adapter_path_info info;
  void *user;
  kr_adapter_path_event_cb *ev_cb;
  kr_adapter_path_av_cb *av_cb;
};

struct kr_adapter_setup {
  kr_adapter_info info;
  void *user;
  kr_adapter_event_cb *ev_cb;
  kr_adapter_av_cb *av_cb;
};

int kr_adapter_prepare(kr_adapter *adapter);

int kr_adapter_unlink(kr_adapter_path *path);
kr_adapter_path *kr_adapter_mkpath(kr_adapter *adapter,
 kr_adapter_path_setup *setup);

int kr_adapter_get_info(kr_adapter *adapter, kr_adapter_info *info);
int kr_adapter_destroy(kr_adapter *adapter);
kr_adapter *kr_adapter_create(kr_adapter_setup *setup);


#endif
