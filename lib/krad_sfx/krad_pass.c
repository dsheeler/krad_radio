#include "krad_pass.h"

struct kr_pass {
  biquad filter;
  kr_pass_info info;
  int new_type;
  float sample_rate;
  float new_sample_rate;
  kr_easer bw_easer;
  kr_easer hz_easer;
};

static float pass_clamp_bw(kr_pass *pass, float bw);
static float pass_clamp_hz(kr_pass *pass, float hz);

void kr_pass_set_type(kr_pass *pass, kr_sfx_effect_type type) {
  if ((type != KR_LOWPASS) && (type != KR_HIGHPASS)) {
    return;
  }
  pass->new_type = type;
}

static float pass_clamp_bw(kr_pass *pass, float bw) {
  bw = LIMIT(bw, KR_PASS_BW_MIN, KR_PASS_BW_MAX);
  return bw;
}

void kr_pass_set_bw(kr_pass *pass, float bw, int duration,
 kr_easing easing, void *user) {
  bw = pass_clamp_bw(pass, bw);
  kr_easer_set(&pass->bw_easer, bw, duration, easing, user);
}

static float pass_clamp_hz(kr_pass *pass, float hz) {
  if (pass->info.type == KR_LOWPASS) {
    hz = LIMIT(hz, KR_PASS_HZ_MIN, KR_LOWPASS_HZ_MAX);
  } else {
    hz = LIMIT(hz, KR_PASS_HZ_MIN, KR_PASS_HZ_MAX);
  }
  return hz;
}

void kr_pass_set_hz(kr_pass *pass, float hz, int duration, kr_easing easing,
 void *user) {
  hz = pass_clamp_hz(pass, hz);
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
  if (pass->new_type != pass->info.type) {
    pass->info.type = pass->new_type;
    recompute = 1;
  }
  if (kr_easer_active(&pass->bw_easer)) {
    pass->info.bw = kr_easer_process(&pass->bw_easer, pass->info.bw, &ptr);
    recompute = 1;
    if (broadcast == 1) {
//      krad_radio_broadcast_subunit_control(pass->mixer->broadcaster,
//       &pass->address, BW, pass->bw, ptr);
    }
  }
  if (kr_easer_active(&pass->hz_easer)) {
    pass->info.hz = kr_easer_process(&pass->hz_easer, pass->info.hz, &ptr);
    recompute = 1;
    if (broadcast == 1) {
//      krad_radio_broadcast_subunit_control(pass->mixer->broadcaster,
//       &pass->address, HZ, pass->hz, ptr);
    }
  }
  if (recompute == 1) {
    if (pass->info.type == KR_LOWPASS) {
      lp_set_params(&pass->filter, pass->info.hz, pass->info.bw,
       pass->sample_rate);
    } else {
     hp_set_params(&pass->filter, pass->info.hz, pass->info.bw,
      pass->sample_rate);
    }
  }
  if (((pass->info.type == KR_LOWPASS)
   && (pass->info.hz == KR_LOWPASS_HZ_MAX))
   || ((pass->info.type == KR_HIGHPASS)
   && (pass->info.hz == KR_PASS_HZ_MIN))) {
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
  pass->info.type = type;
  pass->new_type = pass->info.type;
  pass->info.bw = 1;
  if (pass->info.type == KR_LOWPASS) {
    pass->info.hz = KR_LOWPASS_HZ_MAX;
  }
  if (pass->info.type == KR_HIGHPASS) {
    pass->info.hz = KR_PASS_HZ_MIN;
  }
  return pass;
}

kr_pass *kr_pass_create2(kr_pass_setup *setup) {
  kr_pass *pass;
  if (setup == NULL) return NULL;
  pass = kr_pass_create(setup->sample_rate, setup->info.type);
  pass->info.bw = pass_clamp_bw(pass, setup->info.bw);
  pass->info.hz = pass_clamp_hz(pass, setup->info.hz);
  return pass;
}

void kr_pass_destroy(kr_pass *pass) {
  free(pass);
}

int kr_pass_ctl(kr_pass *pass, kr_pass_cmd *cmd) {
  if ((pass == NULL) || (cmd == NULL)) return -1;
  switch (cmd->control) {
    case KR_SFX_HZ:
      kr_pass_set_hz(pass, cmd->value, cmd->duration, cmd->easing, cmd->user);
      break;
    case KR_SFX_BW:
      kr_pass_set_bw(pass, cmd->value, cmd->duration, cmd->easing, cmd->user);
      break;
    case KR_SFX_PASSTYPE:
      break;
    default:
      return -1;
  }
  return 0;
}

int kr_lowpass_info_get(kr_pass *pass, kr_lowpass_info *info) {
  if ((pass == NULL) || (info == NULL)) return -1;
  info->bw = pass->info.bw;
  info->hz = pass->info.hz;
  return 0;
}

int kr_highpass_info_get(kr_pass *pass, kr_highpass_info *info) {
  if ((pass == NULL) || (info == NULL)) return -1;
  info->bw = pass->info.bw;
  info->hz = pass->info.hz;
  return 0;
}
