#ifndef KRAD_EASING_H
#define KRAD_EASING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>

#include "krad_system.h"
#include "krad_easing_common.h"

typedef struct {
  float target;
  int duration;
  kr_easing easing;
  void *ptr;
  int rw;
} kr_easer_update;


typedef struct {
  int newest;
  int last;
  kr_easer_update update[2];

//  int updating;
  float new_target;
  int new_duration;
  kr_easing new_easing;
  void *new_ptr;

  int active;
  float target;
  float start_value;
  float change_amount;
  int elapsed_time;
  int duration;
  kr_easing easing;
  void *ptr;
} kr_easer;

kr_easing kr_easing_random();
float kr_ease(kr_easing easing, float now, float start, float amt, float dur);
void kr_easer_set(kr_easer *easer, float target, int dur, kr_easing easing, void *ptr);
float kr_easer_process(kr_easer *easer, float current, void **ptr);
void kr_easer_destroy(kr_easer *easer);
kr_easer *kr_easer_create();

#endif
