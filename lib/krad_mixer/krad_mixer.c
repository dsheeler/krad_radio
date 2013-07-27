#include "krad_mixer.h"

static int mixer_process(kr_mixer *mixer, uint32_t frames);
static void update_state(kr_mixer *mixer);
static void mark_destroy(kr_mixer *mixer, kr_mixer_path *unit);
static void limit(kr_mixer_path *unit, uint32_t nframes);
static void copy_frames(kr_mixer_path *d, kr_mixer_path *s, uint32_t nframes);
static void mix(kr_mixer_path *dest, kr_mixer_path *src, uint32_t nframes);
static void update_frames(kr_mixer_path *unit, uint32_t nframes);
static void get_frames(kr_mixer_path *unit, uint32_t nframes);
static int handle_delay(kr_mixer_path *unit, uint32_t nframes);
static void clear_frames(kr_mixer_path *unit, uint32_t nframes);
static float read_peak(kr_mixer_path *unit);
static float read_peak_scaled(kr_mixer_path *unit);
static void update_meter_readings(kr_mixer_path *unit);
static void compute_meters(kr_mixer_path *unit, uint32_t nframes);
static void update_volume(kr_mixer_path *unit);
static void apply_volume(kr_mixer_path *unit, int nframes);
static void apply_effects(kr_mixer_path *unit, int nframes);
static float get_crossfade(kr_mixer_path *unit);
static float get_fade_out(float crossfade_value);
static float get_fade_in(float crossfade_value);
static void set_channel_volume(kr_mixer_path *unit, int channel, float value);
static void set_volume(kr_mixer_path *unit, float value);
static void set_crossfade(kr_mixer_path *unit, float value);

static float get_fade_out(float crossfade_value) {

  float fade_out;

  fade_out = cos (3.14159f*0.5f*((crossfade_value + 100.0f) + 0.5f)/200.0f);
  fade_out = fade_out * fade_out;

  return fade_out;
}

static float get_fade_in(float crossfade_value) {
  return 1.0f - get_fade_out(crossfade_value);
}

static float get_crossfade(kr_mixer_path *unit) {

  if (unit->crossfader->unit[0] == unit) {
    return get_fade_out(unit->crossfader->fade);
  }

  if (unit->crossfader->unit[1] == unit) {
    return get_fade_in(unit->crossfader->fade);
  }

  failfast("failed to get unit for crossfade!");

  return 0.0f;
}

static void apply_effects(kr_mixer_path *port, int nframes) {

  if (port->sfx->sample_rate != port->mixer->sample_rate) {
    kr_sfx_set_sample_rate(port->sfx, port->mixer->sample_rate);
  }
  // FIXME hrm we count on thems being the same btw in them effects lookout
  kr_sfx_process(port->sfx, port->samples, port->samples, nframes);
}

static void apply_volume(kr_mixer_path *unit, int nframes) {

  int c;
  int s;
  int sign;

  sign = 0;

  for (c = 0; c < unit->channels; c++) {
    if (unit->new_volume_actual[c] == unit->volume_actual[c]) {
      for (s = 0; s < nframes; s++) {
        unit->samples[c][s] = unit->samples[c][s] * unit->volume_actual[c];
      }
    } else {
      /* The way the volume change is set up here, the volume can only
       * change once per callback, but thats allways plenty
       * last_sign: 0 = unset, -1 neg, +1 pos */
      if (unit->last_sign[c] == 0) {
        if (unit->samples[c][0] > 0.0f) {
          unit->last_sign[c] = 1;
        } else {
          /* Zero counts as negative here, but its moot */
          unit->last_sign[c] = -1;
        }
      }
      for (s = 0; s < nframes; s++) {
        if (unit->last_sign[c] != 0) {
          if (unit->samples[c][s] > 0.0f) {
            sign = 1;
          } else {
            sign = -1;
          }
          if ((sign != unit->last_sign[c]) || (unit->samples[c][s] == 0.0f)) {
            unit->volume_actual[c] = unit->new_volume_actual[c];
            unit->last_sign[c] = 0;
          }
        }
        unit->samples[c][s] = (unit->samples[c][s] * unit->volume_actual[c]);
      }
      if (unit->last_sign[c] != 0) {
        unit->last_sign[c] = sign;
      }
    }
  }
}

static float read_peak_scaled(kr_mixer_path *unit) {

  float db;
  float def;

  db = 20.0f * log10f(read_peak(unit) * 1.0f);
  db = 20.0f * log10f(unit->avg[0] * 1.0f);
  if (db < -70.0f) {
      def = 0.0f;
    } else if (db < -60.0f) {
      def = (db + 70.0f) * 0.25f;
    } else if (db < -50.0f) {
      def = (db + 60.0f) * 0.5f + 2.5f;
    } else if (db < -40.0f) {
      def = (db + 50.0f) * 0.75f + 7.5;
    } else if (db < -30.0f) {
      def = (db + 40.0f) * 1.5f + 15.0f;
    } else if (db < -20.0f) {
      def = (db + 30.0f) * 2.0f + 30.0f;
    } else if (db < 0.0f) {
      def = (db + 20.0f) * 2.5f + 50.0f;
    } else {
      def = 100.0f;
  }

  return def;
}

static float read_peak(kr_mixer_path *unit) {

  //FIXME N channels

  float tmp = unit->peak[0];
  unit->peak[0] = 0.0f;

  float tmp2 = unit->peak[1];
  unit->peak[1] = 0.0f;
  if (tmp > tmp2) {
    return tmp;
  } else {
    return tmp2;
  }
}

static void compute_meters(kr_mixer_path *unit, uint32_t nframes) {

  int c;
  int s;
  int w;
  int mw;
  float sample;
  float level;
  int cur_frame;
  int cframes;

  cur_frame = 0;
  cframes = MIN(32, nframes);
  while (cur_frame < nframes) {
    for (c = 0; c < unit->channels; c++) {
      level = 0;
      for(s = cur_frame; s < cur_frame + cframes; s++) {
        sample = fabs(unit->samples[c][s]);
        level += sample;
        if (sample > unit->peak[c]) {
          unit->peak[c] = sample;
        }
      }
      unit->wins[c][unit->win] = level / (float)cframes;
    }
    unit->winss[unit->win] = cframes;
    cur_frame += cframes;
    cframes = MIN(32, nframes - cur_frame);
    unit->win++;
    unit->win = unit->win % KR_MXR_MAX_MINIWINS;
  }
  for (c = 0; c < unit->channels; c++) {
    level = 0;
    s = 0;
    w = unit->win;
    mw = unit->win + KR_MXR_MAX_MINIWINS;
    while (s < unit->mixer->avg_window_size) {
      level += unit->wins[c][w % KR_MXR_MAX_MINIWINS];
      s += unit->winss[w % KR_MXR_MAX_MINIWINS];
      w++;
      if (w == mw) break;
    }
    unit->avg[c] = level / (float)(w - unit->win);
  }
}

static void clear_frames(kr_mixer_path *unit, uint32_t nframes) {

  int c;
  int s;

  for (c = 0; c < unit->channels; c++) {
    for (s = 0; s < nframes; s++) {
      unit->samples[c][s] = 0.0f;
    }
  }
}

static int handle_delay(kr_mixer_path *unit, uint32_t nframes) {

  int delay_frames;

  if (unit->delay > unit->delay_actual) {
    delay_frames = unit->delay - unit->delay_actual;
    if (delay_frames > nframes) {
      delay_frames = nframes;
    }
    clear_frames(unit, delay_frames);
    unit->delay += delay_frames;
    return nframes - delay_frames;
  }

  return nframes;
}

static void get_frames(kr_mixer_path *unit, uint32_t nframes) {

  int c;
  float *samples[KR_MXR_MAX_CHANNELS];
/*
  switch (unit->type) {
    case KRAD_TONE:
      //krad_tone_run(unit->io_ptr, *unit->mapped_samples[0], nframes);
      break;
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      krad_audio_unit_samples_callback(nframes, unit->io_ptr, samples);
      for (c = 0; c < KR_MXR_MAX_CHANNELS; c++ ) {
        unit->samples[c] = samples[unit->map[c]];
      }
      break;
    case KRAD_LINK:
      if (unit->direction == INPUT) {
        krad_link_audio_samples_callback(nframes, unit->io_ptr,
         unit->mapped_samples[0]);
      }
      break;
    case KLOCALSHM:
      if (unit->direction == INPUT) {
        kr_mixer_local_audio_samples_callback(nframes, unit->io_ptr,
         unit->mapped_samples[0]);
      }
      break;
  }
  */
}

static void update_frames(kr_mixer_path *unit, uint32_t nframes) {
  if (unit->delay != unit->delay_actual) {
    nframes = handle_delay(unit, nframes);
  }
  get_frames(unit, nframes);
}

static void mix(kr_mixer_path *dest, kr_mixer_path *src, uint32_t nframes) {

  int c;
  int s;

  if (dest->channels == src->channels) {
    for (c = 0; c < dest->channels; c++) {
      for (s = 0; s < nframes; s++) {
        dest->samples[c][s] += src->samples[c][s];
      }
    }
  } else {
    //up mix
    for (c = 0; c < dest->channels; c++) {
      if (src->mixmap[c] != -1) {
        for (s = 0; s < nframes; s++) {
          dest->samples[c][s] += src->samples[src->mixmap[c]][s];
        }
      }
    }
  }
}

static void copy_frames(kr_mixer_path *d, kr_mixer_path *s, uint32_t nframes) {

  /*
  int c;
  int s;

  // Do mixdown here?
  switch (dest->type) {
    case KRAD_TONE:
      break;
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      for (c = 0; c < dest->channels; c++) {
        for (s = 0; s < nframes; s++) {
          dest->samples[c][s] = src->samples[c][s];
        }
      }
      break;
    case KRAD_LINK:
      if (dest->direction == OUTPUT) {
        krad_link_audio_samples_callback(nframes, dest->io_ptr, src->samples);
      }
      break;
    case KLOCALSHM:
      if (dest->direction == OUTPUT) {
        kr_mixer_local_audio_samples_callback(nframes, dest->io_ptr,
         src->samples);
      }
      break;
  }
  */
}

static void limit(kr_mixer_path *unit, uint32_t nframes) {

  int c;

  for (c = 0; c < unit->channels; c++) {
    krad_hardlimit(unit->samples[c], nframes);
  }
}

static void update_state(kr_mixer *mixer) {

  int p;
  kr_mixer_path *unit;

  p = 0;
  unit = NULL;

  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 3)) {
      unit->active = 4;
    }
    if ((unit != NULL) && (unit->active == 1)) {
      unit->active = 2;
    }
  }
}

static void update_volume(kr_mixer_path *unit) {

  int c;

  for (c = 0; c < unit->channels; c++) {
    set_channel_volume(unit, c, unit->volume[c]);
  }
}

static void mark_destroy(kr_mixer *mixer, kr_mixer_path *unit) {
  if (mixer->destroying == 2) {
    unit->active = 4;
  } else {
    if (unit->destroy_mark != 1) {
      unit->destroy_mark = 1;
      unit->active = 3;
    }
  }
}

static void set_crossfade(kr_mixer_path *unit, float value) {

  kr_mixer_crossfader *crossfader;

  if (unit->crossfader != NULL) {
    crossfader = unit->crossfader;
    if ((crossfader->unit[0] != NULL) && (crossfader->unit[1] != NULL)) {
      crossfader->fade = value;
      update_volume(crossfader->unit[0]);
      update_volume(crossfader->unit[1]);
    }
  }
}

static void set_volume(kr_mixer_path *unit, float value) {

  int c;
  float volume_temp;

  volume_temp = (value/100.0f);
  volume_temp *= volume_temp;
  //FIXME input gets bus volume, bus gets higher level bus vol
  if ((unit->type == KR_MXR_INPUT) || (1 == 2)) {
    volume_temp = volume_temp * unit->bus->volume_actual[0];

    if (unit->crossfader != NULL) {
      volume_temp = volume_temp * get_crossfade(unit);
    }
  }

  for (c = 0; c < unit->channels; c++) {
    unit->volume[c] = value;
    if (unit->type == KR_MXR_BUS) {
      unit->volume_actual[c] = volume_temp;
    } else {
      unit->new_volume_actual[c] = volume_temp;
    }
  }

  if (unit->type == KR_MXR_BUS) {

    int p;
    kr_mixer_path *pg;

    for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
      pg = unit->mixer->unit[p];
      if ((pg != NULL) && (pg->active == 2) && (pg->type == KR_MXR_INPUT)) {
        if (pg->bus == unit) {
          update_volume(pg);
        }
      }
    }
  }
}

static void set_channel_volume(kr_mixer_path *unit, int channel, float value) {

  float volume_temp;

  if (unit->type == KR_MXR_INPUT) {
    unit->volume[channel] = value;
    volume_temp = (unit->volume[channel]/100.0f);
    volume_temp *= volume_temp;
    volume_temp = volume_temp * unit->bus->volume_actual[0];
    if (unit->crossfader != NULL) {
      volume_temp = volume_temp * get_crossfade(unit);
    }
    unit->new_volume_actual[channel] = volume_temp;
  }
}

static void update_meter_readings(kr_mixer_path *unit) {

  float peak;

  peak = read_peak_scaled(unit);
  if (peak != unit->peak_last[0]) {
    unit->peak_last[0] = peak;
    krad_radio_broadcast_subunit_control(unit->mixer->broadcaster,
     &unit->address, KR_PEAK, peak, NULL);
  }
}

static int mixer_process(kr_mixer *mixer, uint32_t nframes) {

  int p, m;
  void *client;

  client = NULL;
  kr_mixer_path *unit = NULL;
  kr_mixer_path *bus = NULL;
  kr_mixer_crossfader *crossfader = NULL;

  update_state(mixer);
  // Gets input/output port buffers
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 2) &&
       //((unit->type == KR_MXR_INPUT) || (unit->type == KR_MXR_AUX))) {
       ((unit->type == KR_MXR_INPUT))) {
      update_frames(unit, nframes);
    }
  }

  for (p = 0; p < KR_MXR_MAX_PATHS / 2; p++) {
    crossfader = &mixer->crossfader[p];
    if ((crossfader != NULL) &&
        ((crossfader->unit[0] != NULL) &&
        (crossfader->unit[1] != NULL))) {
      if (crossfader->easer.active) {
        set_crossfade(crossfader->unit[0],
         kr_easer_process(&crossfader->easer,
          crossfader->fade, &client));
        krad_radio_broadcast_subunit_control(mixer->broadcaster,
         &crossfader->unit[0]->address, KR_CROSSFADE,
         crossfader->fade, client);
      }
    }
  }

  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 2) &&
        (unit->volume_easer.active)) {
      set_volume(unit,
       kr_easer_process(&unit->volume_easer, unit->volume[0],
       &client));
      krad_radio_broadcast_subunit_control(mixer->broadcaster,
       &unit->address, KR_VOLUME, unit->volume[0], client);
    }
  }

  // apply volume, effects and calc peaks on inputs
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 2) &&
        (unit->type == KR_MXR_INPUT)) {
      apply_volume(unit, nframes);
      //experiment
      if (unit->volume_actual[0] != 0.0f) {
        apply_effects(unit, nframes);
      }
      compute_meters(unit, nframes);
    }
  }

  // Clear Mixes
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 2) &&
        (unit->type == KR_MXR_BUS)) {
      clear_frames(unit, nframes);
    }
  }

  // Mix
  for (m = 0; m < KR_MXR_MAX_PATHS; m++) {
    bus = mixer->unit[m];
    if ((bus != NULL) && (bus->active) && (bus->type == KR_MXR_BUS)) {
      for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
        unit = mixer->unit[p];
        if ((unit != NULL) && (unit->active == 2) &&
            (unit->bus == bus) && (unit->type == KR_MXR_INPUT)) {
          mix(bus, unit, nframes);
        }
      }
    }
  }
/*
  // copy to outputs, limit all outputs
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 2) &&
        (unit->direction == OUTPUT)) {
      copy_frames(unit, unit->bus, nframes);
      if (unit->output_type == AUX) {
        apply_volume(unit, nframes);
      }
      limit(unit, nframes);
    }
  }
*/
  if (mixer->master != NULL) {
    compute_meters(mixer->master, nframes);
  }

  mixer->frames_since_peak_read += nframes;

  if (mixer->frames_since_peak_read >= mixer->frames_per_peak_broadcast) {
    mixer->frames_since_peak_read = 0;
    for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
      unit = mixer->unit[p];
      if ((unit != NULL) && (unit->active == 2)) {
        update_meter_readings(unit);
      }
    }
    if (mixer->master != NULL) {
      update_meter_readings(mixer->master);
    }
  }
  return 0;
}

void kr_mixer_xf_couple(kr_mixer *mixer, kr_mixer_path *unit1,
 kr_mixer_path *unit2) {

  int i;
  kr_mixer_crossfader *crossfader;

  if ((unit1 == NULL) || ((unit1->active != 1) && (unit1->active != 2)) ||
      (unit2 == NULL) || ((unit2->active != 1) && (unit2->active != 2))) {
    printke("Invalid unit for crossfade!");
    return;
  }
  if (!(((unit1->type == KR_MXR_INPUT) || (unit1->type == KR_MXR_BUS)) &&
      ((unit1->type == KR_MXR_INPUT) || (unit2->type == KR_MXR_BUS)) &&
      (((unit1->type == KR_MXR_BUS) && (unit2->type == KR_MXR_BUS)) ||
       ((unit1->type != KR_MXR_BUS) && (unit2->type != KR_MXR_BUS))))) {
    printke("Invalid crossfade group!");
    return;
  }
  if (unit1->crossfader != NULL) {
    kr_mixer_xf_decouple(mixer, unit1->crossfader);
  }
  if (unit2->crossfader != NULL) {
    kr_mixer_xf_decouple(mixer, unit2->crossfader);
  }
  for (i = 0; i < KR_MXR_MAX_PATHS / 2; i++) {
    crossfader = &mixer->crossfader[i];
    if ((crossfader != NULL) && ((crossfader->unit[0] == NULL) &&
        (crossfader->unit[1] == NULL))) {
      break;
    }
  }

  crossfader->unit[0] = unit1;
  crossfader->unit[1] = unit2;
  unit1->crossfader = crossfader;
  unit2->crossfader = crossfader;

  krad_radio_broadcast_subunit_update(mixer->as->app_broadcaster,
   &unit1->address, KR_CROSSFADE_GROUP, KR_STRING, unit2->name, NULL);

  kr_mixer_ctl(mixer, unit1->name, "crossfade", -100.0f, 0, NULL);
}

void kr_mixer_xf_decouple(kr_mixer *mixer, kr_mixer_crossfader *crossfader) {

  kr_mixer_path *unit[2];

  unit[0] = NULL;
  unit[1] = NULL;

  if ((crossfader != NULL) && (crossfader->unit[0] != NULL) &&
      (crossfader->unit[1] != NULL)) {

    unit[0] = crossfader->unit[0];
    unit[1] = crossfader->unit[1];
    crossfader->unit[0]->crossfader = NULL;
    crossfader->unit[1]->crossfader = NULL;

    crossfader->unit[0] = NULL;
    crossfader->unit[1] = NULL;
    crossfader->fade = -100.0f;

    /* Hrm */
    kr_mixer_ctl(mixer, unit[0]->name, "volume",
     unit[0]->volume[0], 0, NULL);
    kr_mixer_ctl(mixer, unit[1]->name, "volume",
     unit[1]->volume[0], 0, NULL);

    krad_radio_broadcast_subunit_update(mixer->as->app_broadcaster,
     &unit[0]->address, KR_CROSSFADE_GROUP, KR_STRING, "", NULL);
  }
}

kr_mixer_path *kr_mixer_mkpath(kr_mixer *mixer, kr_mixer_path_setup *np) {

  int p;
  int c;
  int len;
  kr_mixer_path *path;

  path = NULL;
  len = strlen(np->info.name);
  /* prevent dupe names */
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    if (mixer->unit[p]->active != 0) {
      if (strncmp(np->info.name, mixer->unit[p]->name, len) == 0) {
        return NULL;
      }
    }
  }
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    if (mixer->unit[p]->active == 0) {
      path = mixer->unit[p];
      break;
    }
  }
  if (path == NULL) {
    return NULL;
  }
  path->mixer = mixer;
  strncpy(path->name, np->info.name, sizeof(path->name));
  path->channels = np->info.channels;
  path->type = np->info.type;
  //FIXME find my bus by name
  path->bus = mixer->master;
  path->address.path.unit = KR_MIXER;
  path->address.path.subunit.mixer_subunit = KR_PORTGROUP;
  strcpy (path->address.id.name, path->name);
  for (c = 0; c < KR_MXR_MAX_CHANNELS; c++) {
    if (c < path->channels) {
      //FIXME take mapping from np
      path->mixmap[c] = c;
    } else {
      path->mixmap[c] = -1;
    }
    path->map[c] = c;
    path->mapped_samples[c] = &path->samples[c];
    path->volume[c] = np->info.volume[c];
    path->volume_actual[c] = (float)(path->volume[c]/100.0f);
    path->volume_actual[c] *= path->volume_actual[c];
    path->new_volume_actual[c] = path->volume_actual[c];
    path->samples[c] = calloc(1, 4096);
  }
  path->tags = krad_tags_create(path->name);
  if (path->tags == NULL) {
    failfast ("Oh I couldn't find me tags");
  }
  path->sfx = kr_sfx_create(path->channels, path->mixer->sample_rate);
  if (path->sfx == NULL) {
    failfast ("Oh I couldn't make effects");
  }
  kr_sfx_effect_add2(path->sfx, KR_EQ, path->mixer, path->name);
  kr_sfx_effect_add2(path->sfx, KR_LOWPASS, path->mixer, path->name);
  kr_sfx_effect_add2(path->sfx, KR_HIGHPASS, path->mixer, path->name);
  kr_sfx_effect_add2(path->sfx, KR_ANALOG, path->mixer, path->name);
  //FIXME
  //set sfx params from np
  return path;
}

void kr_mixer_path_unlink(kr_mixer *mixer, kr_mixer_path *unit) {

  int c;

  if (unit == NULL) {
    return;
  }

  if (unit->crossfader != NULL) {
    kr_mixer_xf_decouple(mixer, unit->crossfader);
  }

  mark_destroy(mixer, unit);

  while (unit->active != 4) {
    usleep(1000);
  }

  unit->delay = 0;
  unit->delay_actual = 0;

  printk("Krad Mixer: Removing %s", unit->name);

  for (c = 0; c < KR_MXR_MAX_CHANNELS; c++) {
    free(unit->samples[c]);
  }

  //if (unit->type != KRAD_LINK) {
    krad_tags_destroy(unit->tags);
    unit->tags = NULL;
  //}

  if (unit->sfx != NULL) {
    kr_sfx_destroy(unit->sfx);
    unit->sfx = NULL;
  }

  unit->destroy_mark = 0;
  unit->active = 0;
}

kr_mixer_path *kr_mixer_path_from_name(kr_mixer *mixer, char *name) {

  int p;
  int len;
  kr_mixer_path *unit;

  len = strlen(name);

  if (len > 0) {
    for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
      unit = mixer->unit[p];
      if ((unit != NULL) && ((unit->active == 1) || (unit->active == 2))) {
        if ((strlen(unit->name) == len) &&
            (strncmp(name, unit->name, len) == 0)) {
          return unit;
        }
      }
    }
  }

  printke("Krad Mixer: Could not find unit called %s", name);

  return NULL;
}

int kr_mixer_ctl(kr_mixer *mixer, char *name, char *control, float value,
 int duration, void *ptr) {

  kr_mixer_path *unit;

  unit = kr_mixer_path_from_name(mixer, name);

  if (unit != NULL) {
    //if ((unit->direction == OUTPUT) &&
    //    (unit->output_type == DIRECT)) {
    //  return 1;
   // }
    if ((strncmp(control, "volume", 6) == 0) && (strlen(control) == 6)) {
      //unit_set_volume (unit, value);
      kr_easer_set(&unit->volume_easer, value, duration, EASEINOUTSINE, ptr);
      return 1;
    }
    //Fixme add bus to bus crossfadin
    if ((unit->type == KR_MXR_INPUT) && (unit->type != KR_MXR_BUS)) {
      if ((strncmp(control, "crossfade", 9) == 0) && (strlen(control) == 9)) {
        if (unit->crossfader != NULL) {
          //unit_set_crossfade (unit, value);
          kr_easer_set(&unit->crossfader->easer, value, duration,
           EASEINOUTSINE, ptr);
        }
        return 1;
      }
      if (strncmp(control, "volume_left", 11) == 0) {
        set_channel_volume(unit, 0, value);
        return 1;
      }
      if (strncmp(control, "volume_right", 12) == 0) {
        set_channel_volume(unit, 1, value);
        return 1;
      }
    }
  }
  return 0;
}

void kr_mixer_channel_move(kr_mixer_path *unit, int in_chan, int out_chan) {
  unit->map[in_chan] = out_chan;
  unit->mapped_samples[in_chan] = &unit->samples[out_chan];
}

void kr_mixer_channel_copy(kr_mixer_path *unit, int in_chan, int out_chan) {
  unit->mixmap[out_chan] = in_chan;
}

uint32_t kr_mixer_period(kr_mixer *mixer) {
  return mixer->period_size;
}

int kr_mixer_period_set(kr_mixer *mixer, uint32_t period_size) {
  mixer->period_size = period_size;
  return mixer->period_size;
}

uint32_t kr_mixer_sample_rate(kr_mixer *mixer) {
  return mixer->sample_rate;
}

int kr_mixer_sample_rate_set(kr_mixer *mixer, uint32_t sample_rate) {
  mixer->sample_rate = sample_rate;
  return mixer->sample_rate;
}

int kr_mixer_mix(kr_mixer *mixer, uint32_t frames) {
  return mixer_process(mixer, frames);
}

void kr_mixer_appserver_set(kr_mixer *mixer, kr_app_server *as) {
  mixer->as = as;
  mixer->broadcaster = krad_app_server_broadcaster_register(mixer->as);
}

int kr_mixer_destroy(kr_mixer *mixer) {

  int p;

  printk("Krad Mixer shutdown started");

  mixer->destroying = 1;
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    if ((mixer->unit[p]->active == 2) &&
        (mixer->unit[p]->type != KR_MXR_BUS)) {
      mark_destroy(mixer, mixer->unit[p]);
    }
  }
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    if ((mixer->unit[p]->active != 0) &&
        (mixer->unit[p]->type != KR_MXR_BUS)) {
      kr_mixer_path_unlink(mixer, mixer->unit[p]);
    }
  }
  mixer->destroying = 2;
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    if (mixer->unit[p]->active != 0) {
      kr_mixer_path_unlink(mixer, mixer->unit[p]);
    }
  }
  free(mixer->crossfader);
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    free(mixer->unit[p]);
  }
  free(mixer);
  printk("Krad Mixer shutdown complete");
  return 0;
}

static void kr_mixer_masterbus_setup(kr_mixer *mixer) {

  kr_mixer_path_setup mbs;

  memset(&mbs, 0, sizeof(kr_mixer_path_setup));
  mbs.info.type = KR_MXR_BUS;
  strncpy(mbs.info.name, "Master", sizeof(mbs.info.name));
  mbs.info.channels = 2;
  mbs.info.volume[0] = KR_MXR_DEF_MBUS_LVL;
  mbs.info.volume[1] = KR_MXR_DEF_MBUS_LVL;
  mixer->master = kr_mixer_mkpath(mixer, &mbs);
}

kr_mixer *kr_mixer_create() {

  int p;
  kr_mixer *mixer;

  if ((mixer = calloc(1, sizeof(kr_mixer))) == NULL) {
    failfast("Krad Mixer memory alloc failure");
  }

  mixer->address.path.unit = KR_MIXER;
  mixer->address.path.subunit.mixer_subunit = KR_UNIT;
  mixer->sample_rate = KR_MXR_DEF_SRATE;
  mixer->avg_window_size = (mixer->sample_rate / 1000) * KR_MXR_RMS_WINDOW_MS;
  mixer->period_size = KR_MXR_DEF_PFRAMES;
  mixer->frames_per_peak_broadcast = 1536;
  mixer->crossfader = calloc(KR_MXR_MAX_PATHS / 2,
   sizeof(kr_mixer_crossfader));
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    mixer->unit[p] = calloc(1, sizeof(kr_mixer_path));
  }
  kr_mixer_masterbus_setup(mixer);
  return mixer;
}
