#include "krad_sfx.h"

kr_effects *kr_effects_create(int channels, int sample_rate) {

  kr_effects *effects;

  effects = calloc(1, sizeof(kr_effects));
  effects->effect = calloc(KRAD_EFFECTS_MAX, sizeof(kr_effect_t));
  effects->channels = channels;
  effects->sample_rate = sample_rate;

  return effects;
}

void kr_effects_destroy(kr_effects *effects) {

  int e;

  for (e = 0; e < KRAD_EFFECTS_MAX; e++) {
    if (effects->effect[e].active == 1) {
      kr_effects_effect_remove(effects, e);
    }
  }

  free(effects->effect);
  free(effects);
}

void kr_effects_set_sample_rate(kr_effects *effects, uint32_t sample_rate) {

  int e, c;

  effects->sample_rate = sample_rate;

  for (e = 0; e < KRAD_EFFECTS_MAX; e++) {
    if (effects->effect[e].active == 1) {
      for (c = 0; c < effects->channels; c++) {
        switch (effects->effect[e].effect_type) {
          case KRAD_NOFX:
            break;
          case KRAD_EQ:
            kr_eq_set_sample_rate(effects->effect[e].effect[c], effects->sample_rate);
            break;
          case KRAD_LOWPASS:
          case KRAD_HIGHPASS:
            kr_pass_set_sample_rate(effects->effect[e].effect[c], effects->sample_rate);
            break;
          case KRAD_ANALOG:
            kr_analog_set_sample_rate(effects->effect[e].effect[c], effects->sample_rate);
            break;
        }
      }
    }
  }
}

void kr_effects_process(kr_effects *effects, float **input, float **output, int num_samples) {

  int e, c;

  for (e = 0; e < KRAD_EFFECTS_MAX; e++) {
    if (effects->effect[e].active == 1) {
      for (c = 0; c < effects->channels; c++) {
        switch (effects->effect[e].effect_type) {
          case KRAD_NOFX:
            break;
          case KRAD_EQ:
            //kr_eq_process (effects->effect[e].effect[c], input[c], output[c], num_samples);
            kr_eq_process2(effects->effect[e].effect[c], input[c], output[c], num_samples, c == 0);
            break;
          case KRAD_LOWPASS:
          case KRAD_HIGHPASS:
            //kr_pass_process (effects->effect[e].effect[c], input[c], output[c], num_samples);
            kr_pass_process2(effects->effect[e].effect[c], input[c], output[c], num_samples, c == 0);
            break;
          case KRAD_ANALOG:
            //kr_analog_process (effects->effect[e].effect[c], input[c], output[c], num_samples);
            kr_analog_process2(effects->effect[e].effect[c], input[c], output[c], num_samples, c == 0);
            break;
        }
      }
    }
  }
}

void kr_effects_effect_add(kr_effects *effects, kr_effect_type_t effect) {

  int e, c;

  for (e = 0; e < KRAD_EFFECTS_MAX; e++) {
    if (effects->effect[e].active == 0) {
      effects->effect[e].effect_type = effect;
      for (c = 0; c < effects->channels; c++) {
        switch (effects->effect[e].effect_type) {
          case KRAD_NOFX:
            break;
          case KRAD_EQ:
            effects->effect[e].effect[c] = kr_eq_create (effects->sample_rate);
            break;
         case KRAD_LOWPASS:
           effects->effect[e].effect[c] = kr_pass_create (effects->sample_rate, KRAD_LOWPASS);
           break;
         case KRAD_HIGHPASS:
           effects->effect[e].effect[c] = kr_pass_create (effects->sample_rate, KRAD_HIGHPASS);
           break;
         case KRAD_ANALOG:
           effects->effect[e].effect[c] = kr_analog_create (effects->sample_rate);
           break;
        }
      }
      effects->effect[e].active = 1;
      break;
    }
  }
}

void kr_effects_effect_add2(kr_effects *effects, kr_effect_type_t effect,
 kr_mixer *mixer, char *portgroupname) {

  int e, c;

  for (e = 0; e < KRAD_EFFECTS_MAX; e++) {
    if (effects->effect[e].active == 0) {
      effects->effect[e].effect_type = effect;
      for (c = 0; c < effects->channels; c++) {
        switch (effects->effect[e].effect_type) {
          case KRAD_NOFX:
            break;
          case KRAD_EQ:
            //effects->effect[e].effect[c] = kr_eq_create (effects->sample_rate);
            effects->effect[e].effect[c] = kr_eq_create2(effects->sample_rate, mixer, portgroupname);
            break;
         case KRAD_LOWPASS:
           //effects->effect[e].effect[c] = kr_pass_create (effects->sample_rate, KRAD_LOWPASS);
           effects->effect[e].effect[c] = kr_pass_create2(effects->sample_rate, KRAD_LOWPASS, mixer, portgroupname);
           break;
         case KRAD_HIGHPASS:
           //effects->effect[e].effect[c] = kr_pass_create (effects->sample_rate, KRAD_HIGHPASS);
           effects->effect[e].effect[c] = kr_pass_create2(effects->sample_rate, KRAD_HIGHPASS, mixer, portgroupname);
           break;
         case KRAD_ANALOG:
           //effects->effect[e].effect[c] = kr_analog_create (effects->sample_rate);
           effects->effect[e].effect[c] = kr_analog_create2(effects->sample_rate, mixer, portgroupname);
           break;
        }
      }
      effects->effect[e].active = 1;
      break;
    }
  }
}

void kr_effects_effect_remove(kr_effects *effects, int effect_num) {

  int c;

  for (c = 0; c < effects->channels; c++) {
    switch (effects->effect[effect_num].effect_type) {
      case KRAD_NOFX:
        break;
      case KRAD_EQ:
        kr_eq_destroy(effects->effect[effect_num].effect[c]);
        break;
      case KRAD_LOWPASS:
      case KRAD_HIGHPASS:
        kr_pass_destroy(effects->effect[effect_num].effect[c]);
        break;
      case KRAD_ANALOG:
        kr_analog_destroy(effects->effect[effect_num].effect[c]);
        break;
    }
    effects->effect[effect_num].effect[c] = NULL;
  }

  effects->effect[effect_num].active = 0;
}


void kr_effects_control(kr_effects *effects, int effect_num, int control_id,
 int control, float value, int duration, kr_easing easing, void *user) {

  int c;

  for (c = 0; c < effects->channels; c++) {
    switch (effects->effect[effect_num].effect_type) {
      case KRAD_NOFX:
        break;
      case KRAD_EQ:
        switch (control) {
          case KRAD_EQ_CONTROL_DB:
            kr_eq_band_set_db(effects->effect[effect_num].effect[c],
             control_id, value, duration, easing, user);
            break;
          case KRAD_EQ_CONTROL_BANDWIDTH:
            kr_eq_band_set_bandwidth(effects->effect[effect_num].effect[c],
             control_id, value, duration, easing, user);
            break;
          case KRAD_EQ_CONTROL_HZ:
            kr_eq_band_set_hz(effects->effect[effect_num].effect[c],
             control_id, value, duration, easing, user);
            break;
        }
        break;
      case KRAD_LOWPASS:
      case KRAD_HIGHPASS:
        switch (control) {
          case KRAD_PASS_CONTROL_BANDWIDTH:
            kr_pass_set_bandwidth(effects->effect[effect_num].effect[c],
             value, duration, easing, user);
            break;
          case KRAD_PASS_CONTROL_HZ:
            kr_pass_set_hz(effects->effect[effect_num].effect[c], value,
             duration, easing, user);
            break;
        }
        break;
      case KRAD_ANALOG:
        switch (control) {
          case KRAD_ANALOG_CONTROL_BLEND:
            kr_analog_set_blend(effects->effect[effect_num].effect[c],
             value, duration, easing, user);
            break;
          case KRAD_ANALOG_CONTROL_DRIVE:
            kr_analog_set_drive(effects->effect[effect_num].effect[c],
             value, duration, easing, user);
            break;
        }
        break;
    }
  }
}

kr_effect_type_t kr_effects_strtoeffect(char *string) {
	if (((strlen(string) == 2) && (strncmp(string, "lp", 2) == 0)) ||
	    ((strlen(string) == 7) && (strncmp(string, "lowpass", 7) == 0))) {
		return KRAD_LOWPASS;
	}
	if (((strlen(string) == 2) && (strncmp(string, "hp", 2) == 0)) ||
	    ((strlen(string) == 8) && (strncmp(string, "highpass", 8) == 0))) {
		return KRAD_HIGHPASS;
	}
	if ((strlen(string) == 2) && (strncmp(string, "eq", 2) == 0)) {
		return KRAD_EQ;
	}
	if ((strlen(string) == 6) && (strncmp(string, "analog", 6) == 0)) {
		return KRAD_ANALOG;
	}
	return KRAD_NOFX;
}

int kr_effects_strtoctrl(kr_effect_type_t effect_type,
 char *string) {

  if (effect_type == KRAD_EQ) {
	  if ((strlen(string) == 2) && (strncmp(string, "db", 2) == 0)) {
		  return KRAD_EQ_CONTROL_DB;
	  }
	  if ((strlen(string) == 2) && (strncmp(string, "hz", 2) == 0)) {
		  return KRAD_EQ_CONTROL_HZ;
	  }
	  if ((strlen(string) == 2) && (strncmp(string, "bw", 2) == 0)) {
		  return KRAD_EQ_CONTROL_BANDWIDTH;
	  }
	  if ((strlen(string) == 9) && (strncmp(string, "bandwidth", 9) == 0)) {
		  return KRAD_EQ_CONTROL_BANDWIDTH;
	  }
  }

  if (((effect_type == KRAD_LOWPASS) || (effect_type == KRAD_HIGHPASS))) {
	  if ((strlen(string) == 2) && (strncmp(string, "hz", 2) == 0)) {
		  return KRAD_PASS_CONTROL_HZ;
	  }
	  if ((strlen(string) == 2) && (strncmp(string, "bw", 2) == 0)) {
		  return KRAD_PASS_CONTROL_BANDWIDTH;
	  }
	  if ((strlen(string) == 9) && (strncmp(string, "bandwidth", 9) == 0)) {
		  return KRAD_PASS_CONTROL_BANDWIDTH;
	  }
  }

  if (effect_type == KRAD_ANALOG) {
	  if ((strlen(string) == 5) && (strncmp(string, "blend", 5) == 0)) {
		  return KRAD_ANALOG_CONTROL_BLEND;
	  }
	  if ((strlen(string) == 5) && (strncmp(string, "drive", 5) == 0)) {
		  return KRAD_ANALOG_CONTROL_DRIVE;
	  }
  }

	return 0;
}
