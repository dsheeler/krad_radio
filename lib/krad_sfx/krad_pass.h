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

struct kr_pass {

  biquad filter;

  int type;
  float bw;
  float hz;

  int new_type;

  float new_sample_rate;
  float sample_rate;

  kr_easer bw_easer;
  kr_easer hz_easer;

  kr_mixer *mixer;
  kr_address_t address;
};

kr_pass *kr_pass_create2(int sample_rate, kr_sfx_type type, kr_mixer *mixer,
 char *portgroupname);
kr_pass *kr_pass_create(int sample_rate, kr_sfx_type type);
void kr_pass_destroy(kr_pass *pass);

void kr_pass_set_sample_rate(kr_pass *pass, int sample_rate);
//void kr_pass_process (kr_pass_t *kr_pass, float *input, float *output,
//int nsamples);
void kr_pass_process2(kr_pass *pass, float *input, float *output,
 int nsamples, int broadcast);
/* Controls */
void kr_pass_set_type(kr_pass *pass, kr_sfx_type type);
void kr_pass_set_bw(kr_pass *pass, float bw, int duration, kr_easing easing, void *user);
void kr_pass_set_hz(kr_pass *pass, float hz, int duration, kr_easing easing, void *user);

#endif
