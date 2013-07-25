#ifndef KRAD_MIXER_COMMON_H
#define KRAD_MIXER_COMMON_H

#define KR_MXR_MAX_PATHS 12
#define KR_MXR_MAX_CHANNELS 8
#define KR_MXR_DEF_SRATE 48000
#define KR_MXR_DEF_PFRAMES 1024
#define KR_MXR_DEF_MBUS_LVL 75.0f
#define KR_MXR_RMS_WINDOW_MS 100

#include "krad_sfx_common.h"
#include "krad_ebml2.h"
#include "krad_radio_ipc.h"

typedef struct kr_mixer_info kr_mixer_info;
typedef struct kr_mixer_path_info kr_mixer_path_info;
typedef struct kr_mixer_path_info kr_mixer_input_info;
typedef struct kr_mixer_path_info kr_mixer_bus_info;
typedef struct kr_mixer_path_info kr_mixer_aux_info;

typedef enum {
  NIL,
  MONO,
  STEREO,
  THREE,
  QUAD,
  FIVE,
  SIX,
  SEVEN,
  EIGHT
} kr_mixer_channels;

typedef enum {
  KR_VOLUME = 1,
  KR_CROSSFADE,
  KR_PEAK,
  KR_CROSSFADE_GROUP
} kr_mixer_control;

typedef enum {
  KR_MXR_INPUT,
  KR_MXR_BUS,
  KR_MXR_AUX
} kr_mixer_path_type;

struct kr_mixer_info {
  uint32_t period_size;
  uint32_t sample_rate;
  uint32_t inputs;
  uint32_t buses;
  uint32_t auxes;
  uint64_t frames;
  uint64_t timecode;
  char clock[32];
};

struct kr_mixer_path_info {
  char name[64];
  char bus[64];
  char crossfade_group[64];
  kr_mixer_channels channels;
  kr_mixer_path_type type;
  float fade;
  float volume[KR_MXR_MAX_CHANNELS];
  int map[KR_MXR_MAX_CHANNELS];
  int mixmap[KR_MXR_MAX_CHANNELS];
  float rms[KR_MXR_MAX_CHANNELS];
  float peak[KR_MXR_MAX_CHANNELS];
  int delay;
  kr_lowpass_info lowpass;
  kr_highpass_info highpass;
  kr_analog_info analog;
  kr_eq_info eq;
};

char *kr_mixer_channeltostr(int channel);
char *kr_mixer_ctltostr(kr_mixer_control control);
char *kr_mixer_pathtypetostr(kr_mixer_path_type type);
void kr_mixer_path_info_to_ebml(kr_mixer_path_info *pi, kr_ebml *ebml);
#endif
