#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <math.h>

#ifndef KRAD_TONE_H
#define KRAD_TONE_H

#define MAX_TONES 8
#define KR_TONE_DEFAULT_VOLUME 65

#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

typedef struct {
  float frequency;
  float delta;
  float angle;
  int active;
  int duration;
  int last_sign;
} tone_t;

typedef struct {

  tone_t tones[MAX_TONES];
  int active_tones;
  int duration;
  float sample_rate;
  int volume;
  float volume_actual;

} krad_tone_t;

krad_tone_t *krad_tone_create(float sample_rate);
void krad_tone_set_sample_rate(krad_tone_t *krad_tone, float sample_rate);
void krad_tone_add(krad_tone_t *krad_tone, float frequency);
void krad_tone_add_preset(krad_tone_t *krad_tone, char *preset);
void krad_tone_clear(krad_tone_t *krad_tone);
void krad_tone_set_volume(krad_tone_t *krad_tone, int volume);
void krad_tone_remove(krad_tone_t *krad_tone, float frequency);
void krad_tone_run(krad_tone_t *krad_tone, float *buffer, int samples);
void krad_tone_destroy(krad_tone_t *krad_tone);

#endif
