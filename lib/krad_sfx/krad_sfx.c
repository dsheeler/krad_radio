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

static void set_sample_rate(kr_sfx *sfx, uint32_t sample_rate);
static void effect_add(kr_sfx *sfx, kr_sfx_effect_type effect);
static void effect_remove(kr_sfx *sfx, kr_sfx_effect_type effect);
static void effect_control(kr_sfx *sfx, kr_sfx_effect_type effect, int control_id,
 char *control_str, float value, int duration, kr_easing easing, void *user);
static int effect_info(kr_sfx *sfx, kr_sfx_effect_type effect, void *effect_info);

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
      effect_remove(sfx, e);
    }
  }
  free(sfx->effect);
  free(sfx);
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

static void set_sample_rate(kr_sfx *sfx, uint32_t sample_rate) {
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

static void effect_add(kr_sfx *sfx, kr_sfx_effect_type effect) {
  int e;
  int c;
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

void effect_remove(kr_sfx *sfx, kr_sfx_effect_type effect) {
  int e;
  int c;
  for (e = 0; e < KR_SFX_MAX; e++) {
    if ((sfx->effect[e].active == 1) && (sfx->effect[e].type == effect)) {
      for (c = 0; c < sfx->channels; c++) {
        switch (sfx->effect[e].type) {
          case KR_SFX_NONE:
            break;
          case KR_SFX_EQ:
            kr_eq_destroy(sfx->effect[e].effect[c]);
            break;
          case KR_SFX_LOWPASS:
          case KR_SFX_HIGHPASS:
            kr_pass_destroy(sfx->effect[e].effect[c]);
            break;
          case KR_SFX_ANALOG:
            kr_analog_destroy(sfx->effect[e].effect[c]);
            break;
        }
        sfx->effect[e].effect[c] = NULL;
      }
      sfx->effect[e].active = 0;
      break;
    }
  }
}

static void effect_control(kr_sfx *sfx, kr_sfx_effect_type effect, int control_id,
 char *control_str, float value, int duration, kr_easing easing, void *user) {
  int e;
  int c;
  int control;
  for (e = 0; e < KR_SFX_MAX; e++) {
    if ((sfx->effect[e].active == 1) && (sfx->effect[e].type == effect)) {
      control = kr_sfxeftctlstr(sfx->effect[e].type, control_str);
      for (c = 0; c < sfx->channels; c++) {
        switch (sfx->effect[e].type) {
          case KR_SFX_NONE:
            break;
          case KR_SFX_EQ:
            switch (control) {
              case KR_SFX_DB:
                kr_eq_band_set_db(sfx->effect[e].effect[c],
                 control_id, value, duration, easing, user);
                break;
              case KR_SFX_BW:
                kr_eq_band_set_bw(sfx->effect[e].effect[c],
                 control_id, value, duration, easing, user);
                break;
              case KR_SFX_HZ:
                kr_eq_band_set_hz(sfx->effect[e].effect[c],
                 control_id, value, duration, easing, user);
                break;
             }
             break;
          case KR_SFX_LOWPASS:
          case KR_SFX_HIGHPASS:
            switch (control) {
             case KR_SFX_BW:
                kr_pass_set_bw(sfx->effect[e].effect[c],
                 value, duration, easing, user);
                break;
             case KR_SFX_HZ:
                kr_pass_set_hz(sfx->effect[e].effect[c], value,
                 duration, easing, user);
                 break;
              }
            break;
          case KR_SFX_ANALOG:
            switch (control) {
              case KR_SFX_BLEND:
                kr_analog_set_blend(sfx->effect[e].effect[c],
                 value, duration, easing, user);
                break;
              case KR_SFX_DRIVE:
                kr_analog_set_drive(sfx->effect[e].effect[c],
                 value, duration, easing, user);
                break;
            }
            break;
        }
      }
    }
  }
}

static int effect_info(kr_sfx *sfx, kr_sfx_effect_type effect, void *info) {
  int i;
  kr_eq_info *eq_info;
  kr_lowpass_info *lp_info;
  kr_highpass_info *hp_info;
  kr_analog_info *analog_info;
  kr_eq *eq;
  kr_lowpass *lowpass;
  kr_highpass *highpass;
  kr_analog *analog;
  switch (effect) {
    case KR_SFX_EQ:
      eq_info = (kr_eq_info *)info;
      eq = (kr_eq *)sfx->effect[0].effect[0];
      for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
        eq_info->band[i].db = eq->band[i].db;
        eq_info->band[i].bw = eq->band[i].bw;
        eq_info->band[i].hz = eq->band[i].hz;
      }
      return 0;
    case KR_SFX_LOWPASS:
      lp_info = (kr_lowpass_info *)info;
      lowpass = (kr_lowpass *)sfx->effect[1].effect[0];
      lp_info->hz = lowpass->hz;
      lp_info->bw = lowpass->bw;
      return 0;
    case KR_SFX_HIGHPASS:
      hp_info = (kr_highpass_info *)info;
      highpass = (kr_highpass *)sfx->effect[2].effect[0];
      hp_info->hz = highpass->hz;
      hp_info->bw = highpass->bw;
      return 0;
    case KR_SFX_ANALOG:
      analog_info = (kr_analog_info *)info;
      analog = (kr_analog *)sfx->effect[3].effect[0];
      analog_info->drive = analog->drive;
      analog_info->blend = analog->blend;
      return 0;
   default:
      break;
  }
  return -1;
}

int kr_sfx_ctl(kr_sfx *sfx, kr_sfx_cmd *cmd) {
  if ((sfx == NULL) || (cmd == NULL)) return -1;
  switch (cmd->control) {
    case KR_SFX_SET_SAMPLERATE:
      set_sample_rate(sfx, cmd->sample_rate);
      break;
    case KR_SFX_EFFECT_ADD:
      effect_add(sfx, cmd->effect);
      break;
    case KR_SFX_EFFECT_RM:
      effect_remove(sfx, cmd->effect);
      break;
    case KR_SFX_EFFECT_CONTROL:
      effect_control(sfx, cmd->effect, cmd->control_id, cmd->control_str,
       cmd->value, cmd->duration, cmd->easing, cmd->user);
      break;
    case KR_SFX_GET_INFO:
      effect_info(sfx, cmd->effect, cmd->user);
      break;
    default:
      return -2;
  }
  return 0;
}
