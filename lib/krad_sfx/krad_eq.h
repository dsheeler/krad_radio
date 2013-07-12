#ifndef KRAD_EQ_H
#define KRAD_EQ_H

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

#define KRAD_EQ_CONTROL_DB 666
#define KRAD_EQ_CONTROL_BANDWIDTH 667
#define KRAD_EQ_CONTROL_HZ 668

#define KRAD_EQ_BANDWIDTH_MIN 0.1
#define KRAD_EQ_BANDWIDTH_MAX 5.0
#define KRAD_EQ_DB_MIN -50.0
#define KRAD_EQ_DB_MAX 20.0
#define KRAD_EQ_HZ_MIN 20.0
#define KRAD_EQ_HZ_MAX 20000.0

typedef struct {
  biquad filter;
  float db;
  float bandwidth;
  float hz;
  kr_easer db_easer;
  kr_easer bandwidth_easer;
  kr_easer hz_easer;
} kr_eq_band;

typedef struct {
  float new_sample_rate;
  float sample_rate;
  kr_eq_band band[KRAD_EQ_MAX_BANDS];
  kr_mixer *mixer;
  kr_address_t address;
} kr_eq;

kr_eq *kr_eq_create2(int sample_rate, kr_mixer *mixer, char *name);
kr_eq *kr_eq_create(int sample_rate);
void kr_eq_destroy(kr_eq *eq);

void kr_eq_set_sample_rate(kr_eq *eq, int sample_rate);
//void kr_eq_process (kr_eq_t *kr_eq, float *input, float *output,
//int num_samples);
void kr_eq_process2(kr_eq *eq, float *input, float *output, int num_samples,
 int broadcast);
void kr_eq_band_set_db(kr_eq *eq, int band_num, float db, int duration,
 kr_easing easing, void *user);
void kr_eq_band_set_bandwidth(kr_eq *eq, int band_num, float bandwidth,
 int duration, kr_easing easing, void *user);
void kr_eq_band_set_hz(kr_eq *eq, int band_num, float hz, int duration,
 kr_easing easing, void *user);

#endif
