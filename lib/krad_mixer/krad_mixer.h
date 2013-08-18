#ifndef KRAD_MIXER_H
#define KRAD_MIXER_H

typedef struct kr_mixer kr_mixer;
typedef struct kr_mixer_path kr_mixer_path;
typedef struct kr_mixer_path kr_mixer_input;
typedef struct kr_mixer_path kr_mixer_bus;
typedef struct kr_mixer_path kr_mixer_aux;
typedef struct kr_mixer_crossfader kr_mixer_crossfader;
typedef struct kr_mixer_path_setup kr_mixer_path_setup;
typedef struct kr_mixer_path_setup kr_mixer_input_setup;
typedef struct kr_mixer_path_setup kr_mixer_aux_setup;
typedef struct kr_mixer_path_audio_cb_arg kr_mixer_path_audio_cb_arg;
typedef struct kr_mixer_path_info_cb_arg kr_mixer_path_info_cb_arg;
typedef struct kr_mixer_info_cb_arg kr_mixer_info_cb_arg;
typedef struct kr_mixer_setup kr_mixer_setup;

#define KR_MXR_MAX_MINIWINS 192

#include "krad_mixer_common.h"
#include "krad_mixer_interface.h"
#include "krad_sfx.h"

typedef void (kr_mixer_info_cb)(kr_mixer_info_cb_arg *);
typedef void (kr_mixer_path_info_cb)(kr_mixer_path_info_cb_arg *);
typedef void (kr_mixer_path_audio_cb)(kr_mixer_path_audio_cb_arg *);

struct kr_mixer_info_cb_arg {
  void *user;
};

struct kr_mixer_path_info_cb_arg {
  void *user;
};

struct kr_mixer_path_audio_cb_arg {
  uint32_t channels;
  uint32_t nframes;
  float **samples;
  void *user;
};

struct kr_mixer_path_setup {
  kr_mixer_path_info info;
  void *user;
  kr_mixer_path_audio_cb *cb;
};

/* FIXME add hard mode/maxpaths */
struct kr_mixer_setup {
  uint32_t period_size;
  uint32_t sample_rate;
  void *user;
  kr_mixer_info_cb *cb;
};

//FIXME the below strucst should be opauqe
struct kr_mixer_crossfader {
  kr_mixer_path *unit[2];
  float fade;
  kr_easer easer;
};

struct kr_mixer_path {
  kr_mixer_path_type type;
  kr_mixer_bus *bus;
  char name[64];
  kr_mixer_channels channels;
  kr_mixer_crossfader *crossfader;
  kr_easer volume_easer;
  int map[KR_MXR_MAX_CHANNELS];
  int mixmap[KR_MXR_MAX_CHANNELS];
  float volume[KR_MXR_MAX_CHANNELS];
  float volume_actual[KR_MXR_MAX_CHANNELS];
  float new_volume_actual[KR_MXR_MAX_CHANNELS];
  int last_sign[KR_MXR_MAX_CHANNELS];
  int win;
  int winss[KR_MXR_MAX_MINIWINS];
  float wins[KR_MXR_MAX_CHANNELS][KR_MXR_MAX_MINIWINS];
  float avg[KR_MXR_MAX_CHANNELS];
  float peak[KR_MXR_MAX_CHANNELS];
  float peak_last[KR_MXR_MAX_CHANNELS];
  float *samples[KR_MXR_MAX_CHANNELS];
  float **mapped_samples[KR_MXR_MAX_CHANNELS];
  int delay;
  int delay_actual;
  int destroy_mark;
  int active;
  kr_mixer_path_info_cb *info_cb;
  void *info_cb_user;
  kr_mixer_path_audio_cb *audio_cb;
  void *audio_cb_user;
  kr_mixer *mixer;
  kr_sfx *sfx;
};

struct kr_mixer {
  uint32_t period_size;
  uint32_t sample_rate;
  uint32_t new_sample_rate;
  int avg_window_size;
  kr_mixer_bus *master;
  kr_mixer_path *unit[KR_MXR_MAX_PATHS];
  kr_mixer_crossfader *crossfader;
  int frames_since_peak_read;
  int frames_per_peak_broadcast;
  kr_mixer_info_cb *info_cb;
  void *info_cb_user;
  int pusher;
  int destroying;
};

kr_mixer_path *kr_mixer_mkpath(kr_mixer *mixer, kr_mixer_path_setup *setup);
int kr_mixer_unlink(kr_mixer_path *path);
kr_mixer_path *kr_mixer_path_from_name(kr_mixer *mixer, char *name);

//FIXME replace with kr_mixer_path_ctl and union type arg
int kr_mixer_ctl(kr_mixer *mx, char *n, char *ctl, float val, int dr, void *p);
void kr_mixer_xf_couple(kr_mixer *mixer, kr_mixer_path *l, kr_mixer_path *r);
void kr_mixer_xf_decouple(kr_mixer *mixer, kr_mixer_crossfader *crossfader);
void kr_mixer_channel_copy(kr_mixer_path *unit, int in_chan, int out_chan);
void kr_mixer_channel_move(kr_mixer_path *unit, int in_chan, int out_chan);

void kr_mixer_setup_init(kr_mixer_setup *setup);

/* Mixer as a whole funcs */
kr_mixer *kr_mixer_create(kr_mixer_setup *setup);
int kr_mixer_destroy(kr_mixer *mixer);
int kr_mixer_mix(kr_mixer *mixer);

//FIXME replace with kr_mixer_ctl
uint32_t kr_mixer_sample_rate(kr_mixer *mixer);
int32_t kr_mixer_sample_rate_set(kr_mixer *mixer, uint32_t sample_rate);
uint32_t kr_mixer_period(kr_mixer *mixer);
int32_t kr_mixer_period_set(kr_mixer *mixer, uint32_t period_sz);

#endif
