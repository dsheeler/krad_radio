#ifndef KRAD_MIXER_H
#define KRAD_MIXER_H

#define KRAD_MIXER_MAX_MINIWINS 192

typedef struct krad_mixer_St krad_mixer_t;
typedef struct krad_mixer_portgroup_St krad_mixer_portgroup_t;
typedef struct krad_mixer_local_portgroup_St krad_mixer_local_portgroup_t;
typedef struct krad_mixer_portgroup_St krad_mixer_mixbus_t;
typedef struct krad_mixer_crossfade_group_St krad_mixer_crossfade_group_t;

#include "krad_mixer_common.h"
#include "krad_radio.h"
#include "krad_mixer_interface.h"
#include "krad_sfx.h"

struct krad_mixer_crossfade_group_St {
  krad_mixer_portgroup_t *portgroup[2];
  float fade;
  krad_easing_t fade_easing;
};

struct krad_mixer_local_portgroup_St {
  int channels;
  int local;
  int shm_sd;
  int msg_sd;
  char *local_buffer;
  int local_buffer_size;
  float *samples[KRAD_MIXER_MAX_CHANNELS];
  krad_mixer_portgroup_direction_t direction;
  krad_mixer_t *krad_mixer;
  int last_wrote;
};

struct krad_mixer_portgroup_St {
  
  char sysname[64];
  kr_address_t address;
  
  krad_mixer_portgroup_direction_t direction;
  krad_mixer_portgroup_io_t io_type;
  krad_mixer_output_t output_type;
  void *io_ptr;
  channels_t channels;
  krad_mixer_mixbus_t *mixbus;
  krad_mixer_crossfade_group_t *crossfade_group;
  
  krad_easing_t volume_easing;
  
  int map[KRAD_MIXER_MAX_CHANNELS];
  int mixmap[KRAD_MIXER_MAX_CHANNELS];  
  
  float volume[KRAD_MIXER_MAX_CHANNELS];
  float volume_actual[KRAD_MIXER_MAX_CHANNELS];
  float new_volume_actual[KRAD_MIXER_MAX_CHANNELS];
  int last_sign[KRAD_MIXER_MAX_CHANNELS];
  int win;
  int winss[KRAD_MIXER_MAX_MINIWINS];
  float wins[KRAD_MIXER_MAX_CHANNELS][KRAD_MIXER_MAX_MINIWINS];
  float avg[KRAD_MIXER_MAX_CHANNELS];
  float peak[KRAD_MIXER_MAX_CHANNELS];
  float peak_last[KRAD_MIXER_MAX_CHANNELS];
  float *samples[KRAD_MIXER_MAX_CHANNELS];

  float **mapped_samples[KRAD_MIXER_MAX_CHANNELS];

  int delay;
  int delay_actual;

  int destroy_mark;
  int active;
  
  krad_mixer_t *krad_mixer;
  krad_tags_t *krad_tags;

  krad_xmms_t *krad_xmms;

  kr_effects_t *effects;
};

struct krad_mixer_St {

  kr_address_t address;

  krad_audio_t *krad_audio;

  krad_audio_api_t pusher;
  krad_ticker_t *krad_ticker;
  int ticker_running;
  int ticker_period;
  pthread_t ticker_thread;
    
  char *name;
  uint32_t period_size;
  uint32_t sample_rate;
    
  int avg_window_size;

  krad_mixer_mixbus_t *master_mix;

  krad_mixer_portgroup_t *tone_port;
  char push_tone;  
    
  krad_mixer_portgroup_t *portgroup[KRAD_MIXER_MAX_PORTGROUPS];
  krad_mixer_crossfade_group_t *crossfade_group;

  int frames_since_peak_read;
  int frames_per_peak_broadcast;

  struct timespec start_time;

  krad_app_broadcaster_t *broadcaster;

  krad_app_server_t *app;

  int destroying;
};

krad_mixer_portgroup_t *krad_mixer_local_portgroup_create (krad_mixer_t *krad_mixer, char *sysname,
                               int direction, int shm_sd, int msg_sd);
krad_mixer_portgroup_t *krad_mixer_portgroup_create (krad_mixer_t *krad_mixer, char *sysname, int direction, krad_mixer_output_t output_type,
                                                     int channels, float volume, krad_mixer_mixbus_t *mixbus, krad_mixer_portgroup_io_t io_type, void *io_ptr, krad_audio_api_t api);
void krad_mixer_portgroup_destroy (krad_mixer_t *krad_mixer, krad_mixer_portgroup_t *portgroup);
krad_mixer_portgroup_t *krad_mixer_get_portgroup_from_sysname (krad_mixer_t *krad_mixer, char *sysname);

void krad_mixer_crossfade_group_create (krad_mixer_t *krad_mixer, krad_mixer_portgroup_t *portgroup1, krad_mixer_portgroup_t *portgroup2);
void krad_mixer_crossfade_group_destroy (krad_mixer_t *krad_mixer, krad_mixer_crossfade_group_t *crossfade_group);

void krad_mixer_start_ticker_at (krad_mixer_t *krad_mixer, struct timespec start_time);
void krad_mixer_start_ticker (krad_mixer_t *krad_mixer);

krad_audio_api_t krad_mixer_get_pusher (krad_mixer_t *krad_mixer);
int krad_mixer_has_pusher (krad_mixer_t *krad_mixer);
void krad_mixer_set_pusher (krad_mixer_t *krad_mixer, krad_audio_api_t pusher);
void krad_mixer_unset_pusher (krad_mixer_t *krad_mixer);

int krad_mixer_mix (uint32_t nframes, krad_mixer_t *krad_mixer);

uint32_t krad_mixer_get_sample_rate (krad_mixer_t *krad_mixer);
void krad_mixer_set_sample_rate (krad_mixer_t *krad_mixer, uint32_t sample_rate);

uint32_t krad_mixer_get_period_size (krad_mixer_t *krad_mixer);
void krad_mixer_set_period_size (krad_mixer_t *krad_mixer, uint32_t period_size);

krad_mixer_t *krad_mixer_create (char *name);
void krad_mixer_destroy (krad_mixer_t *krad_mixer);
void krad_mixer_set_app (krad_mixer_t *krad_mixer, krad_app_server_t *app);

int krad_mixer_set_portgroup_control (krad_mixer_t *krad_mixer, char *sysname, char *control, float value, int duration, void *ptr);

void krad_mixer_portgroup_mixmap_channel (krad_mixer_portgroup_t *portgroup, int in_channel, int out_channel);
void krad_mixer_portgroup_map_channel (krad_mixer_portgroup_t *portgroup, int in_channel, int out_channel);

void krad_mixer_unplug_portgroup (krad_mixer_t *krad_mixer, char *name, char *remote_name);
void krad_mixer_plug_portgroup (krad_mixer_t *krad_mixer, char *name, char *remote_name);
                               
void krad_mixer_portgroup_xmms2_cmd (krad_mixer_t *krad_mixer, char *portgroupname, char *xmms2_cmd);
void krad_mixer_portgroup_bind_xmms2 (krad_mixer_t *krad_mixer, char *portgroupname, char *ipc_path);
void krad_mixer_portgroup_unbind_xmms2 (krad_mixer_t *krad_mixer, char *portgroupname);

                               
#endif
