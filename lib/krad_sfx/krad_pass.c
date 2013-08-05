#include "krad_pass.h"

/* Controls */
void kr_pass_set_type(kr_pass *pass, kr_sfx_effect_type type) {
  if ((type != KR_LOWPASS) && (type != KR_HIGHPASS)) {
    return;
  }
  pass->new_type = type;
}

void kr_pass_set_bw(kr_pass *pass, float bw, int duration,
 kr_easing easing, void *user) {
  bw = LIMIT(bw, KR_PASS_BW_MIN, KR_PASS_BW_MAX);
  kr_easer_set(&pass->bw_easer, bw, duration, easing, user);
}

void kr_pass_set_hz(kr_pass *pass, float hz, int duration, kr_easing easing,
 void *user) {
  if (pass->type == KR_LOWPASS) {
    hz = LIMIT(hz, KR_PASS_HZ_MIN, KR_LOWPASS_HZ_MAX);
  } else {
    hz = LIMIT(hz, KR_PASS_HZ_MIN, KR_PASS_HZ_MAX);
  }
  kr_easer_set(&pass->hz_easer, hz, duration, easing, user);
}

//void kr_pass_process (kr_pass *kr_pass, float *input, float *output,
//int num_samples) {
void kr_pass_process2(kr_pass *pass, float *input, float *output,
 int num_samples, int broadcast) {

  int s;
  int recompute;
  void *ptr;

	ptr = NULL;
  recompute = 0;

  if (pass->new_sample_rate != pass->sample_rate) {
    pass->sample_rate = pass->new_sample_rate;
    recompute = 1;
  }

  if (pass->new_type != pass->type) {
    pass->type = pass->new_type;
    recompute = 1;
  }

  if (pass->bw_easer.active) {
    pass->bw = kr_easer_process(&pass->bw_easer, pass->bw, &ptr);
    recompute = 1;
    if (broadcast == 1) {
//      krad_radio_broadcast_subunit_control(pass->mixer->broadcaster,
//       &pass->address, BW, pass->bw, ptr);
    }
  }

  if (pass->hz_easer.active) {
    pass->hz = kr_easer_process(&pass->hz_easer, pass->hz, &ptr);
    recompute = 1;
    if (broadcast == 1) {
//      krad_radio_broadcast_subunit_control(pass->mixer->broadcaster,
//       &pass->address, HZ, pass->hz, ptr);
    }
  }

  if (recompute == 1) {
    if (pass->type == KR_LOWPASS) {
      lp_set_params(&pass->filter, pass->hz, pass->bw,
       pass->sample_rate);
    } else {
     hp_set_params(&pass->filter, pass->hz, pass->bw,
      pass->sample_rate);
    }
  }

  if (((pass->type == KR_LOWPASS) && (pass->hz == KR_LOWPASS_HZ_MAX)) ||
      ((pass->type == KR_HIGHPASS) && (pass->hz == KR_PASS_HZ_MIN))) {
    return;
  }

  for (s = 0; s < num_samples; s++) {
    output[s] = biquad_run(&pass->filter, input[s]);
  }
}

void kr_pass_set_sample_rate(kr_pass *pass, int sample_rate) {
  pass->new_sample_rate = sample_rate;
}

kr_pass *kr_pass_create(int sample_rate, kr_sfx_effect_type type) {

  kr_pass *pass;

  if ((type != KR_LOWPASS) && (type != KR_HIGHPASS)) {
    return NULL;
  }

  pass = calloc(1, sizeof(kr_pass));
  pass->new_sample_rate = sample_rate;
  pass->type = type;
  pass->new_type = pass->type;
  pass->bw = 1;
  if (pass->type == KR_LOWPASS) {
    pass->hz = KR_LOWPASS_HZ_MAX;
  }
  if (pass->type == KR_HIGHPASS) {
    pass->hz = KR_PASS_HZ_MIN;
  }
  return pass;
}

void kr_pass_destroy(kr_pass *pass) {
  free(pass);
}
