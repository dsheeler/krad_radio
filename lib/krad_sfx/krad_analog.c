#include "krad_analog.h"

#define EPS 0.000000001f

struct kr_analog {
  float sample_rate;
  float drive;
  float blend;
  float prev_drive;
  float prev_blend;
  kr_easer drive_easer;
  kr_easer blend_easer;
  float prev_med;
  float prev_out;
  float rdrive;
  float rbdr;
  float kpa;
  float kpb;
  float kna;
  float knb;
  float ap;
  float an;
  float imr;
  float kc;
  float srct;
  float sq;
  float pwrq;
};

static inline float M(float x) {
  if ((x > EPS) || (x < -EPS))
    return x;
  else
    return 0.0f;
}

static inline float D(float x) {
  if (x > EPS)
    return sqrt(x);
  else if (x < -EPS)
    return sqrt(-x);
  else
    return 0.0f;
}

void kr_analog_set_blend(kr_analog *analog, float blend, int duration,
 kr_easing easing, void *user) {
  blend = LIMIT(blend, KR_ANALOG_BLEND_MIN, KR_ANALOG_BLEND_MAX);
  kr_easer_set(&analog->blend_easer, blend, duration, easing, user);
}

void kr_analog_set_drive(kr_analog *analog, float drive, int duration,
 kr_easing easing, void *user) {
  drive = LIMIT(drive, KR_ANALOG_DRIVE_MIN_OFF, KR_ANALOG_DRIVE_MAX);
  kr_easer_set(&analog->drive_easer, drive, duration, easing, user);
}

void kr_analog_set_sample_rate(kr_analog *analog, int sample_rate) {
  analog->sample_rate = sample_rate;
}

kr_analog *kr_analog_create(int sample_rate) {

  kr_analog *analog;

  analog = calloc(1, sizeof(kr_analog));
  analog->sample_rate = sample_rate;
  analog->prev_drive = -1.0f;
  analog->prev_blend = -11.0f;
  return analog;
}

void kr_analog_destroy(kr_analog *analog) {
  free(analog);
}

//void kr_analog_process (kr_analog_t *kr_analog, float *input, float *output,
//int num_samples) {
void kr_analog_process2(kr_analog *analog, float *input, float *output,
 int num_samples, int broadcast) {

  int s;
  void *ptr;
  float drive;
  float blend;
  unsigned long sample_rate;

  float rdrive = analog->rdrive;
  float rbdr = analog->rbdr;
  float kpa = analog->kpa;
  float kpb = analog->kpb;
  float kna = analog->kna;
  float knb = analog->knb;
  float ap = analog->ap;
  float an = analog->an;
  float imr = analog->imr;
  float kc = analog->kc;
  float srct = analog->srct;
  float sq = analog->sq;
  float pwrq = analog->pwrq;

  float prev_med;
  float prev_out;
  float in;
  float med;
  float out;

  ptr = NULL;

  if (kr_easer_active(&analog->blend_easer)) {
    analog->blend = kr_easer_process(&analog->blend_easer, analog->blend,
     &ptr);
    if (broadcast == 1) {
//      krad_radio_broadcast_subunit_control(analog->mixer->broadcaster,
//       &analog->address, BLEND, analog->blend, ptr);
    }
  }
  if (kr_easer_active(&analog->drive_easer)) {
    analog->drive = kr_easer_process(&analog->drive_easer, analog->drive,
     &ptr);
    if (broadcast == 1) {
//      krad_radio_broadcast_subunit_control(analog->mixer->broadcaster,
//       &analog->address, DRIVE, analog->drive, ptr);
    }
  }

  if (analog->drive < KR_ANALOG_DRIVE_MIN) {
    return;
  }

  drive = LIMIT(analog->drive, KR_ANALOG_DRIVE_MIN, KR_ANALOG_DRIVE_MAX);
  blend = LIMIT(analog->blend, KR_ANALOG_BLEND_MIN, KR_ANALOG_BLEND_MAX);

  sample_rate = analog->sample_rate;

  if ((analog->prev_drive != drive) || (analog->prev_blend != blend)) {

    rdrive = 12.0f / drive;
    rbdr = rdrive / (10.5f - blend) * 780.0f / 33.0f;
    kpa = D(2.0f * (rdrive*rdrive) - 1.0f) + 1.0f;
    kpb = (2.0f - kpa) / 2.0f;
    ap = ((rdrive*rdrive) - kpa + 1.0f) / 2.0f;
    kc = kpa / D(2.0f * D(2.0f * (rdrive*rdrive) - 1.0f) - 2.0f * rdrive*rdrive);

    srct = (0.1f * sample_rate) / (0.1f * sample_rate + 1.0f);
    sq = kc*kc + 1.0f;
    knb = -1.0f * rbdr / D(sq);
    kna = 2.0f * kc * rbdr / D(sq);
    an = rbdr*rbdr / sq;
    imr = 2.0f * knb + D(2.0f * kna + 4.0f * an - 1.0f);
    pwrq = 2.0f / (imr + 1.0f);

    analog->prev_drive = drive;
    analog->prev_blend = blend;
  }

  for (s = 0; s < num_samples; s++) {

    in = *(input++);
    prev_med = analog->prev_med;
    prev_out = analog->prev_out;

    if (in >= 0.0f) {
      med = (D(ap + in * (kpa - in)) + kpb) * pwrq;
    } else {
      med = (D(an - in * (kna + in)) + knb) * pwrq * -1.0f;
    }

    out = srct * (med - prev_med + prev_out);

    if (out < -1.0f)
      out = -1.0f;

    *(output++) = out;

    analog->prev_med = M(med);
    analog->prev_out = M(out);
  }

  analog->rdrive = rdrive;
  analog->rbdr = rbdr;
  analog->kpa = kpa;
  analog->kpb = kpb;
  analog->kna = kna;
  analog->knb = knb;
  analog->ap = ap;
  analog->an = an;
  analog->imr = imr;
  analog->kc = kc;
  analog->srct = srct;
  analog->sq = sq;
  analog->pwrq = pwrq;
}

int kr_analog_info_get(kr_analog *analog, kr_analog_info *info) {
  if ((analog == NULL) || (info == NULL)) return -1;
  info->drive = analog->drive;
  info->blend = analog->blend;
  return 0;
}
