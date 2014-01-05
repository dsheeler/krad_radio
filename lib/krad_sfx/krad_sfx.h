#include "krad_sfx_common.h"
#include "krad_eq.h"
#include "krad_pass.h"
#include "krad_analog.h"
#include "krad_hardlimiter.h"
#include "krad_system.h"

#ifndef KRAD_SFX_H
#define KRAD_SFX_H

typedef struct kr_sfx kr_sfx;
typedef struct kr_sfx_setup kr_sfx_setup;
typedef struct kr_sfx_info_cb_arg kr_sfx_info_cb_arg;
typedef void (kr_sfx_info_cb)(kr_sfx_info_cb_arg *);

typedef struct {
  kr_sfx_control control;
  int control_id;
  char *control_str;
  float value;
  int duration;
  kr_easing easing;
  int sample_rate;
  kr_sfx_effect_type effect;
  void *user;
} kr_sfx_cmd;

struct kr_sfx_info_cb_arg {
  void *user;
};

struct kr_sfx_setup {
  int channels;
  int sample_rate;
  void *user;
  kr_sfx_info_cb *cb;
};

kr_sfx *kr_sfx_create(kr_sfx_setup *setup);
void kr_sfx_destroy(kr_sfx *sfx);
void kr_sfx_process(kr_sfx *sfx, float **input, float **output, int nframes);
int kr_sfx_ctl(kr_sfx *sfx, kr_sfx_cmd *cmd);

#endif
