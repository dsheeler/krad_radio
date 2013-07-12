#include "krad_eq.h"

void kr_eq_band_set_db(kr_eq *eq, int band_num, float db, int duration,
 kr_easing easing, void *user) {
  if (band_num >= KRAD_EQ_MAX_BANDS) return;
  db = LIMIT(db, KRAD_EQ_DB_MIN, KRAD_EQ_DB_MAX);
  kr_easer_set(&eq->band[band_num].db_easer, db, duration, easing, user);
}

void kr_eq_band_set_bandwidth(kr_eq *eq, int band_num, float bw, int duration,
 kr_easing easing, void *user) {
  if (band_num >= KRAD_EQ_MAX_BANDS) return;
  bw = LIMIT(bw, KRAD_EQ_BANDWIDTH_MIN, KRAD_EQ_BANDWIDTH_MAX);
  kr_easer_set(&eq->band[band_num].bandwidth_easer, bw, duration, easing, user);
}

void kr_eq_band_set_hz(kr_eq *eq, int band_num, float hz, int duration,
 kr_easing easing, void *user) {
  if (band_num >= KRAD_EQ_MAX_BANDS) return;
  hz = LIMIT(hz, KRAD_EQ_HZ_MIN, KRAD_EQ_HZ_MAX);
  kr_easer_set(&eq->band[band_num].hz_easer, hz, duration, easing, user);
}

//void kr_eq_process (kr_eq_t *kr_eq, float *input, float *output, int num_samples) {

void kr_eq_process2(kr_eq *eq, float *input, float *output, int num_samples,
 int broadcast) {

  int b, s;
  int recompute;
  int recompute_default;
  void *ptr;

	ptr = NULL;

  if (eq->new_sample_rate != eq->sample_rate) {
    eq->sample_rate = eq->new_sample_rate;
    recompute_default = 1;
  } else {
    recompute_default = 0;
  }

  for (b = 0; b < KRAD_EQ_MAX_BANDS; b++) {
    //if ((kr_eq->band[b].db == 0.0f) && (!kr_eq->band[b].krad_easing_db.active)) {
    //  continue;
    //}
    eq->address.sub_id2 = b;
    recompute = recompute_default;
    if (eq->band[b].hz_easer.active) {
      eq->band[b].hz = kr_easer_process(&eq->band[b].hz_easer, eq->band[b].hz, &ptr);
      recompute = 1;
      if (broadcast == 1) {
        krad_radio_broadcast_subunit_control(eq->mixer->broadcaster,
         &eq->address, HZ, eq->band[b].hz, ptr);
      }
    }
    if (eq->band[b].db_easer.active) {
      eq->band[b].db = kr_easer_process(&eq->band[b].db_easer, eq->band[b].db, &ptr);
      recompute = 1;
      if (broadcast == 1) {
        krad_radio_broadcast_subunit_control(eq->mixer->broadcaster, &eq->address, DB, eq->band[b].db, ptr);
      }
    }
    if (eq->band[b].bandwidth_easer.active) {
      eq->band[b].bandwidth = kr_easer_process(&eq->band[b].bandwidth_easer, eq->band[b].bandwidth, &ptr);
      recompute = 1;
      if (broadcast == 1) {
        krad_radio_broadcast_subunit_control(eq->mixer->broadcaster, &eq->address, BANDWIDTH, eq->band[b].bandwidth, ptr);
      }
    }
    if (recompute == 1) {
      eq_set_params(&eq->band[b].filter, eq->band[b].hz, eq->band[b].db,
       eq->band[b].bandwidth, eq->sample_rate);
    }
    if (eq->band[b].db != 0.0f) {
      for (s = 0; s < num_samples; s++) {
        output[s] = biquad_run(&eq->band[b].filter, input[s]);
      }
    }
  }
}

void kr_eq_set_sample_rate(kr_eq *eq, int sample_rate) {
  eq->new_sample_rate = sample_rate;
}

kr_eq *kr_eq_create(int sample_rate) {

  int b;
  float hz;
  kr_eq *eq;

  eq = calloc(1, sizeof(kr_eq));

  eq->new_sample_rate = sample_rate;
  eq->sample_rate = eq->new_sample_rate;

  hz = 30.0;
  for (b = 0; b < KRAD_EQ_MAX_BANDS; b++) {
    eq->band[b].db = 0.0f,
    eq->band[b].bandwidth = 1.0f;
    eq->band[b].hz = floor(hz);
    if (hz < 1000.0f) {
      if (hz < 150.0f) {
        hz = hz + 15.0f;
      } else {
        if (hz < 600.0f) {
          hz = hz + 50.0f;
        } else {
          hz = hz + 200.0f;
        }
      }
    } else {
      hz = hz + 1500.0f;
    }
  }

  return eq;
}

kr_eq *kr_eq_create2(int sample_rate, kr_mixer *mixer, char *name) {

  int b;
  float hz;
  kr_eq *eq;

  eq = calloc(1, sizeof(kr_eq));
  eq->new_sample_rate = sample_rate;
  eq->sample_rate = eq->new_sample_rate;
  eq->mixer = mixer;
  eq->address.path.unit = KR_MIXER;
  eq->address.path.subunit.mixer_subunit = KR_EFFECT;
  strncpy(eq->address.id.name, name, sizeof(eq->address.id.name));
  eq->address.sub_id = 0;

  hz = 30.0;
  for (b = 0; b < KRAD_EQ_MAX_BANDS; b++) {
    eq->band[b].db = 0.0f,
    eq->band[b].bandwidth = 1.0f;
    eq->band[b].hz = floor(hz);
    if (hz < 1000.0f) {
      if (hz < 150.0f) {
        hz = hz + 15.0f;
      } else {
        if (hz < 600.0f) {
          hz = hz + 50.0f;
        } else {
          hz = hz + 200.0f;
        }
      }
    } else {
      hz = hz + 1500.0f;
    }
  }
  return eq;
}

void kr_eq_destroy(kr_eq *eq) {
  free(eq);
}
