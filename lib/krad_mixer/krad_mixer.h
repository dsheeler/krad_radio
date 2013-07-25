#ifndef KRAD_MIXER_H
#define KRAD_MIXER_H

typedef struct kr_mixer kr_mixer;
typedef struct kr_mixer_path kr_mixer_path;
typedef struct kr_mixer_path kr_mixer_input;
typedef struct kr_mixer_path kr_mixer_bus;
typedef struct kr_mixer_path kr_mixer_aux;
typedef struct kr_mixer_crossfader kr_mixer_crossfader;

#define KRAD_MIXER_MAX_MINIWINS 192

#include "krad_mixer_common.h"
#include "krad_radio.h"
#include "krad_mixer_interface.h"
#include "krad_sfx.h"

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
  int winss[KRAD_MIXER_MAX_MINIWINS];
  float wins[KR_MXR_MAX_CHANNELS][KRAD_MIXER_MAX_MINIWINS];
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
  krad_tags_t *tags;
  krad_xmms_t *xmms;
  kr_sfx *sfx;
};

struct kr_mixer {
  kr_address_t address;
  krad_audio_api_t pusher;
  krad_ticker_t *ticker;
  int ticker_running;
  int ticker_period;
  pthread_t ticker_thread;
  uint32_t period_size;
  uint32_t sample_rate;
  int avg_window_size;
  kr_mixer_bus *master;
  kr_mixer_path *unit[KR_MXR_MAX_PATHS];
  kr_mixer_crossfader *crossfader;
  int frames_since_peak_read;
  int frames_per_peak_broadcast;
  struct timespec start_time;
  krad_app_broadcaster_t *broadcaster;
  kr_app_server *as;
  int destroying;
};

kr_mixer_path *kr_mixer_unit_create(kr_mixer *mixer, char *name,
 int direction, kr_mixer_path_type output_type, int channels,
 float volume, kr_mixer_bus *bus, int io_type,
 void *io_ptr, krad_audio_api_t api);

void kr_mixer_unit_destroy(kr_mixer *mixer, kr_mixer_path *unit);

int kr_mixer_ctl(kr_mixer *mixer, char *name, char *control, float value,
 int duration, void *ptr);

kr_mixer_path *kr_mixer_unit_from_name(kr_mixer *mixer, char *name);

/* Unit operation controls */
void kr_mixer_cf_attach(kr_mixer *mixer, kr_mixer_path *left, kr_mixer_path *right);
void kr_mixer_cf_detatch(kr_mixer *mixer, kr_mixer_crossfader *crossfader);
void kr_mixer_channel_copy(kr_mixer_path *unit, int in_chan, int out_chan);
void kr_mixer_channel_move(kr_mixer_path *unit, int in_chan, int out_chan);

/* Mixer as a whole funcs */
uint32_t kr_mixer_sample_rate(kr_mixer *mixer);
int32_t kr_mixer_sample_rate_set(kr_mixer *mixer, uint32_t sample_rate);
uint32_t kr_mixer_period(kr_mixer *mixer);
int32_t kr_mixer_period_set(kr_mixer *mixer, uint32_t period_sz);
krad_audio_api_t kr_mixer_get_pusher(kr_mixer *mixer);
int32_t kr_mixer_has_pusher(kr_mixer *mixer);
void kr_mixer_set_pusher(kr_mixer *mixer, krad_audio_api_t pusher);
void kr_mixer_unset_pusher(kr_mixer *mixer);
void kr_mixer_start_ticker_at(kr_mixer *mixer, struct timespec start_time);
void kr_mixer_start_ticker(kr_mixer *mixer);
int kr_mixer_mix(kr_mixer *mixer, uint32_t frames);
kr_mixer *kr_mixer_create();
int kr_mixer_destroy(kr_mixer *mixer);
void kr_mixer_appserver_set(kr_mixer *mixer, kr_as *as);

#endif
