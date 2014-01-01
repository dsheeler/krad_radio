#include "krad_sfx.h"

typedef struct kr_sfx_effect kr_sfx_effect;

struct kr_sfx_effect {
  kr_sfx_effect_type type;
  void *effect[KR_SFX_MAX_CHANNELS];
  int active;
};

struct kr_sfx {
  int channels;
  float sample_rate;
  void *user;
  kr_sfx_info_cb *cb;
  kr_sfx_effect *effect;
};

kr_sfx *kr_sfx_create(kr_sfx_setup *setup) {

  kr_sfx *sfx;

  sfx = calloc(1, sizeof(kr_sfx));
  sfx->effect = calloc(KR_SFX_MAX, sizeof(kr_sfx_effect));

  sfx->channels = setup->channels;
  sfx->sample_rate = setup->sample_rate;
  sfx->user = setup->user;
  sfx->cb = setup->cb;

  return sfx;
}

void kr_sfx_destroy(kr_sfx *sfx) {

  int e;

  for (e = 0; e < KR_SFX_MAX; e++) {
    if (sfx->effect[e].active == 1) {
      kr_sfx_remove(sfx, e);
    }
  }

  free(sfx->effect);
  free(sfx);
}

void kr_sfx_sample_rate_set(kr_sfx *sfx, uint32_t sample_rate) {

  int e, c;

  sfx->sample_rate = sample_rate;

  for (e = 0; e < KR_SFX_MAX; e++) {
    if (sfx->effect[e].active == 1) {
      for (c = 0; c < sfx->channels; c++) {
        switch (sfx->effect[e].type) {
          case KR_SFX_NONE:
            break;
          case KR_SFX_EQ:
            kr_eq_set_sample_rate(sfx->effect[e].effect[c], sfx->sample_rate);
            break;
          case KR_SFX_LOWPASS:
          case KR_SFX_HIGHPASS:
            kr_pass_set_sample_rate(sfx->effect[e].effect[c], sfx->sample_rate);
            break;
          case KR_SFX_ANALOG:
            kr_analog_set_sample_rate(sfx->effect[e].effect[c], sfx->sample_rate);
            break;
        }
      }
    }
  }
}

void kr_sfx_process(kr_sfx *sfx, float **input, float **output, int nframes) {

  int e, c;

  for (e = 0; e < KR_SFX_MAX; e++) {
    if (sfx->effect[e].active == 1) {
      for (c = 0; c < sfx->channels; c++) {
        switch (sfx->effect[e].type) {
          case KR_SFX_NONE:
            break;
          case KR_SFX_EQ:
            kr_eq_process2(sfx->effect[e].effect[c], input[c], output[c], nframes, c == 0);
            break;
          case KR_SFX_LOWPASS:
          case KR_SFX_HIGHPASS:
            kr_pass_process2(sfx->effect[e].effect[c], input[c], output[c], nframes, c == 0);
            break;
          case KR_SFX_ANALOG:
            kr_analog_process2(sfx->effect[e].effect[c], input[c], output[c], nframes, c == 0);
            break;
        }
      }
    }
  }
}

void kr_sfx_add(kr_sfx *sfx, kr_sfx_effect_type effect) {

  int e, c;

  for (e = 0; e < KR_SFX_MAX; e++) {
    if (sfx->effect[e].active == 0) {
      sfx->effect[e].type = effect;
      for (c = 0; c < sfx->channels; c++) {
        switch (sfx->effect[e].type) {
          case KR_SFX_NONE:
            break;
          case KR_SFX_EQ:
            sfx->effect[e].effect[c] = kr_eq_create(sfx->sample_rate);
            break;
         case KR_SFX_LOWPASS:
           sfx->effect[e].effect[c] = kr_pass_create(sfx->sample_rate, KR_LOWPASS);
           break;
         case KR_SFX_HIGHPASS:
           sfx->effect[e].effect[c] = kr_pass_create(sfx->sample_rate, KR_HIGHPASS);
           break;
         case KR_SFX_ANALOG:
           sfx->effect[e].effect[c] = kr_analog_create(sfx->sample_rate);
           break;
        }
      }
      sfx->effect[e].active = 1;
      break;
    }
  }
}

void kr_sfx_remove(kr_sfx *sfx, int effect_num) {

  int c;

  for (c = 0; c < sfx->channels; c++) {
    switch (sfx->effect[effect_num].type) {
      case KR_SFX_NONE:
        break;
      case KR_SFX_EQ:
        kr_eq_destroy(sfx->effect[effect_num].effect[c]);
        break;
      case KR_SFX_LOWPASS:
      case KR_SFX_HIGHPASS:
        kr_pass_destroy(sfx->effect[effect_num].effect[c]);
        break;
      case KR_SFX_ANALOG:
        kr_analog_destroy(sfx->effect[effect_num].effect[c]);
        break;
    }
    sfx->effect[effect_num].effect[c] = NULL;
  }
  sfx->effect[effect_num].active = 0;
}

/*void kr_sfx_effect_ctl(kr_sfx *sfx, int effect_num, int control_id,
 int control, float value, int duration, kr_easing easing, void *user) {*/
void kr_sfx_effect_ctl(kr_sfx *sfx, int effect_num, int control_id,
 char *control_str, float value, int duration, kr_easing easing, void *user) {

  int c;
  int control;
  control = kr_sfxeftctlstr(sfx->effect[effect_num].type, control_str);

  for (c = 0; c < sfx->channels; c++) {
    switch (sfx->effect[effect_num].type) {
      case KR_SFX_NONE:
        break;
      case KR_SFX_EQ:
        switch (control) {
          case KR_EQ_DB:
            kr_eq_band_set_db(sfx->effect[effect_num].effect[c],
             control_id, value, duration, easing, user);
            break;
          case KR_EQ_BW:
            kr_eq_band_set_bw(sfx->effect[effect_num].effect[c],
             control_id, value, duration, easing, user);
            break;
          case KR_EQ_HZ:
            kr_eq_band_set_hz(sfx->effect[effect_num].effect[c],
             control_id, value, duration, easing, user);
            break;
        }
        break;
      case KR_SFX_LOWPASS:
      case KR_SFX_HIGHPASS:
        switch (control) {
          case KR_PASS_BW:
            kr_pass_set_bw(sfx->effect[effect_num].effect[c],
             value, duration, easing, user);
            break;
          case KR_PASS_HZ:
            kr_pass_set_hz(sfx->effect[effect_num].effect[c], value,
             duration, easing, user);
            break;
        }
        break;
      case KR_SFX_ANALOG:
        switch (control) {
          case KR_ANALOG_BLEND:
            kr_analog_set_blend(sfx->effect[effect_num].effect[c],
             value, duration, easing, user);
            break;
          case KR_ANALOG_DRIVE:
            kr_analog_set_drive(sfx->effect[effect_num].effect[c],
             value, duration, easing, user);
            break;
        }
        break;
    }
  }
}

int kr_sfx_effect_info(kr_sfx *sfx, int num, void *effect_info) {

  int i;
  kr_eq_info *eq_info;
  kr_lowpass_info *lp_info;
  kr_highpass_info *hp_info;
  kr_analog_info *analog_info;
  kr_eq *eq;
  kr_lowpass *lowpass;
  kr_highpass *highpass;
  kr_analog *analog;

  switch (num) {
    case 0:
      eq_info = (kr_eq_info *)effect_info;
      eq = (kr_eq *)sfx->effect[0].effect[0];
      for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
        eq_info->band[i].db = eq->band[i].db;
        eq_info->band[i].bw = eq->band[i].bw;
        eq_info->band[i].hz = eq->band[i].hz;
      }
      return 0;
    case 1:
      lp_info = (kr_lowpass_info *)effect_info;
      lowpass = (kr_lowpass *)sfx->effect[1].effect[0];
      lp_info->hz = lowpass->hz;
      lp_info->bw = lowpass->bw;
      return 0;
    case 2:
      hp_info = (kr_highpass_info *)effect_info;
      highpass = (kr_highpass *)sfx->effect[2].effect[0];
      hp_info->hz = highpass->hz;
      hp_info->bw = highpass->bw;
      return 0;
    case 3:
      analog_info = (kr_analog_info *)effect_info;
      analog = (kr_analog *)sfx->effect[3].effect[0];
      analog_info->drive = analog->drive;
      analog_info->blend = analog->blend;
      return 0;
   default:
      break;
  }

  return -1;
}

int kr_sfxeftctlstr(kr_sfx_effect_type type, char *string) {
  if (type == KR_SFX_EQ) {
    if ((strlen(string) == 2) && (strncmp(string, "db", 2) == 0)) {
      return KR_EQ_DB;
    }
    if ((strlen(string) == 2) && (strncmp(string, "hz", 2) == 0)) {
      return KR_EQ_HZ;
    }
    if ((strlen(string) == 2) && (strncmp(string, "bw", 2) == 0)) {
      return KR_EQ_BW;
    }
    if ((strlen(string) == 9) && (strncmp(string, "bandwidth", 9) == 0)) {
      return KR_EQ_BW;
    }
  }
  if (((type == KR_SFX_LOWPASS) || (type == KR_SFX_HIGHPASS))) {
    if ((strlen(string) == 2) && (strncmp(string, "hz", 2) == 0)) {
      return KR_PASS_HZ;
    }
    if ((strlen(string) == 2) && (strncmp(string, "bw", 2) == 0)) {
      return KR_PASS_BW;
    }
    if ((strlen(string) == 9) && (strncmp(string, "bandwidth", 9) == 0)) {
      return KR_PASS_BW;
    }
  }
  if (type == KR_SFX_ANALOG) {
    if ((strlen(string) == 5) && (strncmp(string, "blend", 5) == 0)) {
      return KR_ANALOG_BLEND;
    }
    if ((strlen(string) == 5) && (strncmp(string, "drive", 5) == 0)) {
      return KR_ANALOG_DRIVE;
    }
  }
  return 0;
}
