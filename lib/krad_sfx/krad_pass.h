#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>
#include <time.h>

#include "biquad.h"

#include "krad_system.h"
#include "krad_easing.h"
#include "krad_sfx_common.h"
#include "krad_mixer.h"

#ifndef KR_PASS_H
#define KR_PASS_H

#define KR_LOWPASS KR_SFX_LOWPASS
#define KR_HIGHPASS KR_SFX_HIGHPASS

#define KR_PASS_TYPE 788
#define KR_PASS_BW 789
#define KR_PASS_HZ 780

#define KR_PASS_BW_MIN 0.1
#define KR_PASS_BW_MAX 5.0
#define KR_PASS_HZ_MIN 20.0
#define KR_PASS_HZ_MAX 20000.0
#define KR_LOWPASS_HZ_MAX 12000.0

typedef struct kr_pass kr_pass;
typedef struct kr_pass kr_lowpass;
typedef struct kr_pass kr_highpass;

typedef struct kr_pass_info kr_pass_info;
typedef struct kr_pass_setup kr_pass_setup;
typedef struct kr_pass_cmd kr_pass_cmd;

struct kr_pass_info {
  float bw;
  float hz;
  kr_sfx_effect_type type;
};

struct kr_pass_setup {
  int sample_rate;
  kr_pass_info info;
};

struct kr_pass_cmd {
  kr_sfx_effect_control control;
  float value;
  int duration;
  kr_easing easing;
  void *user;
};

kr_pass *kr_pass_create(int sample_rate, kr_sfx_effect_type type);
kr_pass *kr_pass_create2(kr_pass_setup *setup);
void kr_pass_destroy(kr_pass *pass);

void kr_pass_set_sample_rate(kr_pass *pass, int sample_rate);
//void kr_pass_process (kr_pass_t *kr_pass, float *input, float *output,
//int nsamples);
void kr_pass_process2(kr_pass *pass, float *input, float *output,
 int nsamples, int broadcast);
/* Controls */
void kr_pass_set_type(kr_pass *pass, kr_sfx_effect_type type);
void kr_pass_set_bw(kr_pass *pass, float bw, int duration, kr_easing easing, void *user);
void kr_pass_set_hz(kr_pass *pass, float hz, int duration, kr_easing easing, void *user);

int kr_pass_ctl(kr_pass *pass, kr_pass_cmd *cmd);
int kr_lowpass_info_get(kr_pass *pass, kr_lowpass_info *info);
int kr_highpass_info_get(kr_pass *pass, kr_highpass_info *info);

#endif
