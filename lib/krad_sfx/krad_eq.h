#ifndef KR_EQ_H
#define KR_EQ_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>
#include <time.h>

#include "krad_system.h"
#include "krad_easing.h"
#include "krad_sfx_common.h"
#include "krad_mixer.h"
#include "biquad.h"

#define KR_EQ_DB 666
#define KR_EQ_BW 667
#define KR_EQ_HZ 668

#define KR_EQ_BW_MIN 0.1
#define KR_EQ_BW_MAX 5.0
#define KR_EQ_DB_MIN -50.0
#define KR_EQ_DB_MAX 20.0
#define KR_EQ_HZ_MIN 20.0
#define KR_EQ_HZ_MAX 20000.0

typedef struct kr_eq kr_eq;

kr_eq *kr_eq_create(int sample_rate);
void kr_eq_destroy(kr_eq *eq);

void kr_eq_set_sample_rate(kr_eq *eq, int sample_rate);
//void kr_eq_process (kr_eq_t *kr_eq, float *input, float *output,
//int num_samples);
void kr_eq_process2(kr_eq *eq, float *input, float *output, int nsamples,
 int broadcast);
void kr_eq_band_set_db(kr_eq *eq, int band_num, float db, int duration,
 kr_easing easing, void *user);
void kr_eq_band_set_bw(kr_eq *eq, int band_num, float bw,
 int duration, kr_easing easing, void *user);
void kr_eq_band_set_hz(kr_eq *eq, int band_num, float hz, int duration,
 kr_easing easing, void *user);

int kr_eq_info_get(kr_eq *eq, kr_eq_info *info);

#endif
