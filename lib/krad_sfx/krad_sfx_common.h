#ifndef KRAD_SFX_COMMON_H
#define KRAD_SFX_COMMON_H

#ifndef LIMIT
#define LIMIT(v,l,u) ((v)<(l)?(l):((v)>(u)?(u):(v)))
#endif

#define KR_SFX_MAX 4
#define KR_SFX_MAX_CHANNELS 8
#define KR_EQ_MAX_BANDS 32

typedef enum {
  KR_NOFX,
  KR_EQ,
  KR_LOWPASS,
  KR_HIGHPASS,
  KR_ANALOG,
} kr_sfx_type;

typedef enum {
  DB = 1,
  BW,
  HZ,
  TYPE,
  DRIVE,
  BLEND
} kr_sfx_control;

typedef struct kr_eq_info kr_eq_info;
typedef struct kr_eq_band_info kr_eq_band_info;
typedef struct kr_pass_info kr_lowpass_info;
typedef struct kr_pass_info kr_highpass_info;
typedef struct kr_analog_info kr_analog_info;

struct kr_eq_band_info {
  float db;
  float bw;
  float hz;
};

struct kr_eq_info {
  kr_eq_band_info band[KR_EQ_MAX_BANDS];
};

struct kr_pass_info {
  float bw;
  float hz;
};

struct kr_analog_info {
  float drive;
  float blend;
};

char *sfxtypetostr(kr_sfx_type type);
char *sfxctltostr(kr_sfx_control control);

#endif
