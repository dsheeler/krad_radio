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

#define KR_MXR_MAX_MINIWINS 192

#include "krad_mixer_common.h"
#include "krad_radio.h"
#include "krad_mixer_interface.h"
#include "krad_sfx.h"

struct kr_mixer_path_setup {
  kr_mixer_path_info info;
  void *user;
  void *cb;
};

struct kr_mixer_crossfader {
  kr_mixer_path *unit[2];
  float fade;
  kr_easer easer;
};

struct kr_mixer_path {
  kr_mixer_path_type type;
  kr_mixer_bus *bus;
  void *user;
  char name[64];
  kr_address_t address;
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
  kr_mixer *mixer;
  krad_tags_t *tags; //prolly better off in the txpnder?
  kr_sfx *sfx;
};

struct kr_mixer {
  kr_address_t address;
  uint32_t period_size;
  uint32_t sample_rate;
  int avg_window_size;
  kr_mixer_bus *master;
  kr_mixer_path *unit[KR_MXR_MAX_PATHS];
  kr_mixer_crossfader *crossfader;
  int frames_since_peak_read;
  int frames_per_peak_broadcast;
  krad_app_broadcaster_t *broadcaster;
  kr_app_server *as;
  int pusher;
  int destroying;
};

kr_mixer_path *kr_mixer_mkpath(kr_mixer *mixer, kr_mixer_path_setup *np);
void kr_mixer_path_unlink(kr_mixer *mixer, kr_mixer_path *path);
kr_mixer_path *kr_mixer_path_from_name(kr_mixer *mixer, char *name);
int kr_mixer_ctl(kr_mixer *mx, char *n, char *ctl, float val, int dr, void *p);
void kr_mixer_xf_couple(kr_mixer *mixer, kr_mixer_path *l, kr_mixer_path *r);
void kr_mixer_xf_decouple(kr_mixer *mixer, kr_mixer_crossfader *crossfader);
void kr_mixer_channel_copy(kr_mixer_path *unit, int in_chan, int out_chan);
void kr_mixer_channel_move(kr_mixer_path *unit, int in_chan, int out_chan);

// hrm perhaps we have two ctl functions one for mixer as a whole and
// one for paths, makes sense to mee

// ok so what we need to do is setup two cb types
// one is for pushing/pulling samples
// the other is for volume/peaks/level up
// ... perhaps a 3rd type for path create/destroy?
// this will remove an un-needed mixer/appserver dep
// I should note that the address_t is only used for the broadcasts

/* Mixer as a whole funcs */
kr_mixer *kr_mixer_create();//FIXME max paths?
int kr_mixer_destroy(kr_mixer *mixer);
int kr_mixer_mix(kr_mixer *mixer);
uint32_t kr_mixer_sample_rate(kr_mixer *mixer);
int32_t kr_mixer_sample_rate_set(kr_mixer *mixer, uint32_t sample_rate);
uint32_t kr_mixer_period(kr_mixer *mixer);
int32_t kr_mixer_period_set(kr_mixer *mixer, uint32_t period_sz);


//FIXME replace with cb
//void kr_mixer_appserver_set(kr_mixer *mixer, kr_as *as);
//

#endif
