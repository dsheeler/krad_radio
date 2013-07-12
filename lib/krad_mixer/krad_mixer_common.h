#ifndef KRAD_MIXER_COMMON_H
#define KRAD_MIXER_COMMON_H

typedef struct krad_mixer_rep_St krad_mixer_rep_t;
typedef struct krad_mixer_rep_St kr_mixer_t;
typedef struct krad_mixer_portgroup_rep_St kr_portgroup_t;
typedef struct krad_mixer_portgroup_rep_St kr_mxr_unit_rep;
typedef struct krad_mixer_portgroup_rep_St kr_mxr_unit;
typedef struct krad_mixer_portgroup_rep_St kr_mixer_portgroup_t;
typedef struct krad_mixer_portgroup_rep_St krad_mixer_portgroup_rep_t;

#define KRAD_MIXER_MAX_PORTGROUPS 12
#define KRAD_MIXER_MAX_CHANNELS 8
#define KRAD_MIXER_DEFAULT_SAMPLE_RATE 48000
#define KRAD_MIXER_DEFAULT_PERIOD_SIZE 1024
#define DEFAULT_MASTERBUS_LEVEL 75.0f
#define KRAD_MIXER_RMS_WINDOW_SIZE_MS 100

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
} kr_mixer_unit_control;

typedef enum {
  DB = 1,
  BANDWIDTH,
  HZ,
  TYPE,
  DRIVE,
  BLEND,
} kr_mixer_effect_control;

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
  INPUT,
  BUS,
  DIRECTOUT,
  AUXOUT,
} kr_mixer_unit_type;

struct krad_mixer_portgroup_rep_St {

  char name[64];
  channels_t channels;
  kr_mixer_unit_type type;

  char bus[64];
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

char *kr_mixer_channel_to_str(int channel);
char *effect_type_to_str(kr_effect_type effect_type);
char *effect_control_to_str(kr_mixer_effect_control control);
char *unit_control_to_str(kr_mixer_unit_control control);
char *kr_mixer_unit_type_to_str(kr_mixer_unit_type type);
void kr_mixer_unit_rep_to_ebml(kr_mxr_unit_rep *rep, kr_ebml2_t *ebml);
#endif
