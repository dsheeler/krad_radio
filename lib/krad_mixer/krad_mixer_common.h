#ifndef KRAD_MIXER_COMMON_H
#define KRAD_MIXER_COMMON_H

typedef struct krad_mixer_rep_St krad_mixer_rep_t;
typedef struct krad_mixer_rep_St kr_mixer_t;
typedef struct krad_mixer_portgroup_rep_St kr_portgroup_t;
typedef struct krad_mixer_portgroup_rep_St kr_mixer_portgroup_t;
typedef struct krad_mixer_portgroup_rep_St krad_mixer_portgroup_rep_t;

#define KRAD_MIXER_MAX_PORTGROUPS 12
#define KRAD_MIXER_MAX_CHANNELS 8
#define KRAD_MIXER_DEFAULT_SAMPLE_RATE 48000
#define KRAD_MIXER_DEFAULT_PERIOD_SIZE 1024
#define DEFAULT_MASTERBUS_LEVEL 75.0f
#define KRAD_MIXER_RMS_WINDOW_SIZE_MS 125

#include "krad_sfx_common.h"
#include "krad_ebml2.h"
#include "krad_radio_ipc.h"

typedef enum {
  KR_VOLUME = 1,
  KR_CROSSFADE,
  KR_PEAK,
  KR_CROSSFADE_GROUP,
  KR_XMMS2_IPC_PATH,
  KR_DTMF,
} kr_mixer_portgroup_control_t;

typedef enum {
  DB = 1,
  BANDWIDTH,
  HZ,  
  TYPE,
  DRIVE,
  BLEND,
} kr_mixer_effect_control_t;

typedef enum {
  KRAD_TONE = 1,
  KLOCALSHM,
  KRAD_AUDIO, /* i.e local audio i/o */
  KRAD_LINK, /* i.e. remote audio i/o */
  MIXBUS,  /* i.e. mixer internal i/o */
} krad_mixer_portgroup_io_t;

typedef enum {
  NOTOUTPUT,
  DIRECT,
  AUX,
} krad_mixer_output_t;

typedef enum {
  NIL,
  MONO,
  STEREO,
  THREE,
  QUAD,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
} channels_t;

typedef enum {
  OUTPUT,
  INPUT,
  MIX,
} krad_mixer_portgroup_direction_t;

struct krad_mixer_portgroup_rep_St {

  char sysname[64];
  channels_t channels;
  int io_type;
  krad_mixer_portgroup_direction_t direction;
  krad_mixer_output_t output_type;
  
  char mixbus[64];
  char crossfade_group[64];
  float fade;
  
  float volume[KRAD_MIXER_MAX_CHANNELS];
  
  int map[KRAD_MIXER_MAX_CHANNELS];
  int mixmap[KRAD_MIXER_MAX_CHANNELS];  
  
  float rms[KRAD_MIXER_MAX_CHANNELS];
  float peak[KRAD_MIXER_MAX_CHANNELS];
  int delay;

  kr_eq_rep_t eq;
  kr_lowpass_rep_t lowpass;
  kr_highpass_rep_t highpass;
  kr_analog_rep_t analog;

  uint32_t has_xmms2;
  char xmms2_ipc_path[128];

};

struct krad_mixer_rep_St {
  uint32_t period_size;
  uint32_t sample_rate;
  uint32_t inputs;
  uint32_t outputs;
  uint32_t buses;    
  char time_source[32];
//  uint64_t frames;
//  uint64_t timecode;
};

struct krad_effects_rep_St {
  kr_effect_type_t effect_typ;
  void *effect[KRAD_EFFECTS_MAX_CHANNELS];
};

char *krad_mixer_channel_number_to_string (int channel);
char *effect_type_to_string (kr_effect_type_t effect_type);
char *effect_control_to_string (kr_mixer_effect_control_t effect_control);
char *portgroup_control_to_string (kr_mixer_portgroup_control_t portgroup_control);
char *portgroup_direction_to_string (krad_mixer_portgroup_direction_t direction);
char *portgroup_output_type_to_string (krad_mixer_output_t output_type);

void krad_mixer_portgroup_rep_to_ebml2 (krad_mixer_portgroup_rep_t *krad_mixer_portgroup_rep, kr_ebml2_t *ebml);
#endif
