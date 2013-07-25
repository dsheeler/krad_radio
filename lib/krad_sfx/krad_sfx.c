#include "krad_sfx.h"

kr_sfx *kr_sfx_create(int channels, int sample_rate) {

  kr_sfx *sfx;

  sfx = calloc(1, sizeof(kr_sfx));
  sfx->effect = calloc(KR_EFFECTS_MAX, sizeof(kr_effect_t));
  sfx->channels = channels;
  sfx->sample_rate = sample_rate;

  return sfx;
}

void kr_sfx_destroy(kr_sfx *sfx) {

  int e;

  for (e = 0; e < KR_EFFECTS_MAX; e++) {
    if (sfx->effect[e].active == 1) {
      kr_sfx_effect_remove(sfx, e);
    }
  }

  free(sfx->effect);
  free(sfx);
}

void kr_sfx_set_sample_rate(kr_sfx *sfx, uint32_t sample_rate) {

  int e, c;

  sfx->sample_rate = sample_rate;

  for (e = 0; e < KR_EFFECTS_MAX; e++) {
    if (sfx->effect[e].active == 1) {
      for (c = 0; c < sfx->channels; c++) {
        switch (sfx->effect[e].effect_type) {
          case KR_NOFX:
            break;
          case KR_EQ:
            kr_eq_set_sample_rate(sfx->effect[e].effect[c], sfx->sample_rate);
            break;
          case KR_LOWPASS:
          case KR_HIGHPASS:
            kr_pass_set_sample_rate(sfx->effect[e].effect[c], sfx->sample_rate);
            break;
          case KR_ANALOG:
            kr_analog_set_sample_rate(sfx->effect[e].effect[c], sfx->sample_rate);
            break;
        }
      }
    }
  }
}

void kr_sfx_process(kr_sfx *sfx, float **input, float **output, int nframes) {

  int e, c;

  for (e = 0; e < KR_EFFECTS_MAX; e++) {
    if (sfx->effect[e].active == 1) {
      for (c = 0; c < sfx->channels; c++) {
        switch (sfx->effect[e].effect_type) {
          case KR_NOFX:
            break;
          case KR_EQ:
            //kr_eq_process (sfx->effect[e].effect[c], input[c], output[c], num_samples);
            kr_eq_process2(sfx->effect[e].effect[c], input[c], output[c], nframes, c == 0);
            break;
          case KR_LOWPASS:
          case KR_HIGHPASS:
            //kr_pass_process (sfx->effect[e].effect[c], input[c], output[c], num_samples);
            kr_pass_process2(sfx->effect[e].effect[c], input[c], output[c], nframes, c == 0);
            break;
          case KR_ANALOG:
            //kr_analog_process (sfx->effect[e].effect[c], input[c], output[c], num_samples);
            kr_analog_process2(sfx->effect[e].effect[c], input[c], output[c], nframes, c == 0);
            break;
        }
      }
    }
  }
}

void kr_sfx_effect_add(kr_sfx *sfx, kr_effect_type_t effect) {

  int e, c;

  for (e = 0; e < KR_EFFECTS_MAX; e++) {
    if (sfx->effect[e].active == 0) {
      sfx->effect[e].effect_type = effect;
      for (c = 0; c < sfx->channels; c++) {
        switch (sfx->effect[e].effect_type) {
          case KR_NOFX:
            break;
          case KR_EQ:
            sfx->effect[e].effect[c] = kr_eq_create(sfx->sample_rate);
            break;
         case KR_LOWPASS:
           sfx->effect[e].effect[c] = kr_pass_create(sfx->sample_rate, KR_LOWPASS);
           break;
         case KR_HIGHPASS:
           sfx->effect[e].effect[c] = kr_pass_create(sfx->sample_rate, KR_HIGHPASS);
           break;
         case KR_ANALOG:
           sfx->effect[e].effect[c] = kr_analog_create(sfx->sample_rate);
           break;
        }
      }
      sfx->effect[e].active = 1;
      break;
    }
  }
}

void kr_sfx_effect_add2(kr_sfx *sfx, kr_sfx_type effect, kr_mixer *mixer,
 char *portgroupname) {

  int e, c;

  for (e = 0; e < KR_EFFECTS_MAX; e++) {
    if (sfx->effect[e].active == 0) {
      sfx->effect[e].effect_type = effect;
      for (c = 0; c < sfx->channels; c++) {
        switch (sfx->effect[e].effect_type) {
          case KR_NOFX:
            break;
          case KR_EQ:
            //sfx->effect[e].effect[c] = kr_eq_create (sfx->sample_rate);
            sfx->effect[e].effect[c] = kr_eq_create2(sfx->sample_rate, mixer, portgroupname);
            break;
         case KR_LOWPASS:
           //sfx->effect[e].effect[c] = kr_pass_create (sfx->sample_rate, KR_LOWPASS);
           sfx->effect[e].effect[c] = kr_pass_create2(sfx->sample_rate, KR_LOWPASS, mixer, portgroupname);
           break;
         case KR_HIGHPASS:
           //sfx->effect[e].effect[c] = kr_pass_create (sfx->sample_rate, KR_HIGHPASS);
           sfx->effect[e].effect[c] = kr_pass_create2(sfx->sample_rate, KR_HIGHPASS, mixer, portgroupname);
           break;
         case KR_ANALOG:
           //sfx->effect[e].effect[c] = kr_analog_create (sfx->sample_rate);
           sfx->effect[e].effect[c] = kr_analog_create2(sfx->sample_rate, mixer, portgroupname);
           break;
        }
      }
      sfx->effect[e].active = 1;
      break;
    }
  }
}

void kr_sfx_effect_remove(kr_sfx *sfx, int effect_num) {

  int c;

  for (c = 0; c < sfx->channels; c++) {
    switch (sfx->effect[effect_num].effect_type) {
      case KR_NOFX:
        break;
      case KR_EQ:
        kr_eq_destroy(sfx->effect[effect_num].effect[c]);
        break;
      case KR_LOWPASS:
      case KR_HIGHPASS:
        kr_pass_destroy(sfx->effect[effect_num].effect[c]);
        break;
      case KR_ANALOG:
        kr_analog_destroy(sfx->effect[effect_num].effect[c]);
        break;
    }
    sfx->effect[effect_num].effect[c] = NULL;
  }
  sfx->effect[effect_num].active = 0;
}

void kr_sfx_control(kr_sfx *sfx, int effect_num, int control_id,
 int control, float value, int duration, kr_easing easing, void *user) {

  int c;

  for (c = 0; c < sfx->channels; c++) {
    switch (sfx->effect[effect_num].effect_type) {
      case KR_NOFX:
        break;
      case KR_EQ:
        switch (control) {
          case KR_EQ_DB:
            kr_eq_band_set_db(sfx->effect[effect_num].effect[c],
             control_id, value, duration, easing, user);
            break;
          case KR_EQ_BANDWIDTH:
            kr_eq_band_set_bandwidth(sfx->effect[effect_num].effect[c],
             control_id, value, duration, easing, user);
            break;
          case KR_EQ_HZ:
            kr_eq_band_set_hz(sfx->effect[effect_num].effect[c],
             control_id, value, duration, easing, user);
            break;
        }
        break;
      case KR_LOWPASS:
      case KR_HIGHPASS:
        switch (control) {
          case KR_PASS_BW:
            kr_pass_set_bandwidth(sfx->effect[effect_num].effect[c],
             value, duration, easing, user);
            break;
          case KR_PASS_HZ:
            kr_pass_set_hz(sfx->effect[effect_num].effect[c], value,
             duration, easing, user);
            break;
        }
        break;
      case KR_ANALOG:
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

kr_sfx_type kr_sfx_strtosfx(char *string) {
	if (((strlen(string) == 2) && (strncmp(string, "lp", 2) == 0)) ||
	    ((strlen(string) == 7) && (strncmp(string, "lowpass", 7) == 0))) {
		return KR_LOWPASS;
	}
	if (((strlen(string) == 2) && (strncmp(string, "hp", 2) == 0)) ||
	    ((strlen(string) == 8) && (strncmp(string, "highpass", 8) == 0))) {
		return KR_HIGHPASS;
	}
	if ((strlen(string) == 2) && (strncmp(string, "eq", 2) == 0)) {
		return KR_EQ;
	}
	if ((strlen(string) == 6) && (strncmp(string, "analog", 6) == 0)) {
		return KR_ANALOG;
	}
	return KR_NOFX;
}

int kr_sfx_strtoctl(kr_sfx_type type, char *string) {
  if (type == KR_EQ) {
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
  if (((type == KR_LOWPASS) || (type == KR_HIGHPASS))) {
	  if ((strlen(string) == 2) && (strncmp(string, "hz", 2) == 0)) {
		  return KR_PASS_HZ;
	  }
	  if ((strlen(string) == 2) && (strncmp(string, "bw", 2) == 0)) {
		  return KR_PASS_BANDWIDTH;
	  }
	  if ((strlen(string) == 9) && (strncmp(string, "bandwidth", 9) == 0)) {
		  return KR_PASS_BANDWIDTH;
	  }
  }
  if (type == KR_ANALOG) {
	  if ((strlen(string) == 5) && (strncmp(string, "blend", 5) == 0)) {
		  return KR_ANALOG_BLEND;
	  }
	  if ((strlen(string) == 5) && (strncmp(string, "drive", 5) == 0)) {
		  return KR_ANALOG_DRIVE;
	  }
  }
	return 0;
}
