#ifndef KRAD_SFX_COMMON_H
#define KRAD_SFX_COMMON_H

#include <string.h>

#define KR_SFX_MAX 4
#define KR_SFX_MAX_CHANNELS 8
#define KR_EQ_MAX_BANDS 32

typedef enum {
  KR_SFX_ADD,
  KR_SFX_RM,
  KR_SFX_GET_SAMPLERATE,
  KR_SFX_SET_SAMPLERATE,
  KR_SFX_GET_CHANNELS,
  KR_SFX_SET_CHANNELS
} kr_sfx_control;

typedef enum {
  KR_SFX_NONE,
  KR_SFX_EQ,
  KR_SFX_LOWPASS,
  KR_SFX_HIGHPASS,
  KR_SFX_ANALOG
} kr_sfx_effect_type;

typedef enum {
  KR_SFX_DB = 1,
  KR_SFX_BW,
  KR_SFX_HZ,
  KR_SFX_PASSTYPE,
  KR_SFX_DRIVE,
  KR_SFX_BLEND
} kr_sfx_effect_control;

typedef struct kr_eq_info kr_eq_info;
typedef struct kr_eq_band_info kr_eq_band_info;
typedef struct kr_lowpass_info kr_lowpass_info;
typedef struct kr_highpass_info kr_highpass_info;
typedef struct kr_analog_info kr_analog_info;

struct kr_eq_band_info {
  float db;
  float bw;
  float hz;
};

struct kr_eq_info {
  kr_eq_band_info band[KR_EQ_MAX_BANDS];
};

struct kr_lowpass_info {
  float bw;
  float hz;
};

struct kr_highpass_info {
  float bw;
  float hz;
};

struct kr_analog_info {
  float drive;
  float blend;
};

char *kr_strsfxeft(kr_sfx_effect_type type);
char *kr_strsfxeftctl(kr_sfx_effect_control control);
kr_sfx_effect_type kr_sfxeftstr(char *string);

#endif
