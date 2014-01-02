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

#ifndef KR_ANALOG_H
#define KR_ANALOG_H

#define KR_ANALOG_DRIVE 671
#define KR_ANALOG_BLEND 672
#define KR_ANALOG_DRIVE_MIN_OFF 0.0f
#define KR_ANALOG_DRIVE_MIN 0.1f
#define KR_ANALOG_DRIVE_MAX 10.0f
#define KR_ANALOG_BLEND_MIN -10.0f
#define KR_ANALOG_BLEND_MAX 10.0f

typedef struct kr_analog kr_analog;

kr_analog *kr_analog_create(int sample_rate);
void kr_analog_destroy(kr_analog *analog);

void kr_analog_set_sample_rate(kr_analog *analog, int sample_rate);
//void kr_analog_process(kr_analog_t *kr_analog, float *input, float *output,
//int nsamples);
void kr_analog_process2(kr_analog *analog, float *input, float *output,
 int nsamples, int broadcast);

/* Controls */
void kr_analog_set_drive(kr_analog *analog, float drive, int duration,
 kr_easing easing, void *user);
void kr_analog_set_blend(kr_analog *analog, float blend, int duration,
 kr_easing easing, void *user);

int kr_analog_info_get(kr_analog *analog, kr_analog_info *info);

#endif
