#ifndef KRAD_EASING_H
#define KRAD_EASING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>

#include "krad_easing_common.h"

typedef struct {
  int updating;
  int active;
  float target;
  float new_target;
  float start_value;
  float change_amount;
  int elapsed_time;
  int duration;
  int new_duration;
  kr_easing easing;
  kr_easing new_easing;
  void *ptr;
  void *new_ptr;
} kr_easer;

kr_easing kr_easing_random();
float kr_ease(kr_easing easing, float now, float start, float amt, float dur);
void kr_easer_set(kr_easer *easer, float target, int dur, kr_easing easing, void *ptr);
float kr_easer_process(kr_easer *easer, float current, void **ptr);
void kr_easer_destroy(kr_easer *easer);
kr_easer *kr_easer_create();

#endif
