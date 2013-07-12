#include "krad_mixer.h"

static void ticker_thread_cleanup(void *arg);
static void *ticker_thread(void *arg);
static void mixer_stop_ticker(kr_mixer *mixer);
static int mixer_process(kr_mixer *mixer, uint32_t frames);
static void update_state(kr_mixer *mixer);
static void mark_destroy(kr_mixer *mixer, kr_mxu *unit);
static void limit(kr_mxu *unit, uint32_t nframes);
static void copy_samples(kr_mxu *dest, kr_mxu *src, uint32_t nframes);
static void mix(kr_mxu *dest, kr_mxu *src, uint32_t nframes);
static void update_samples(kr_mxu *unit, uint32_t nframes);
static void get_samples(kr_mxu *unit, uint32_t nframes);
static int handle_delay(kr_mxu *unit, uint32_t nframes);
static void clear_samples(kr_mxu *unit, uint32_t nframes);
static float read_peak(kr_mxu *unit);
static float read_peak_scaled(kr_mxu *unit);
static void update_meter_readings(kr_mxu *unit);
static void compute_meters(kr_mxu *unit, uint32_t nframes);
static void update_volume(kr_mxu *unit);
static void apply_volume(kr_mxu *unit, int nframes);
static void apply_effects(kr_mxu *unit, int nframes);
static float get_crossfade(kr_mxu *unit);
static float get_fade_out(float crossfade_value);
static float get_fade_in(float crossfade_value);
static void crossfader_set_crossfade(kr_mxcf *crossfader, float value);
static void set_channel_volume(kr_mxu *unit, int channel, float value);
static void set_volume(kr_mxu *unit, float value);
static void set_crossfade(kr_mxu *unit, float value);

static float get_fade_out(float crossfade_value) {

  float fade_out;

  fade_out = cos (3.14159f*0.5f*((crossfade_value + 100.0f) + 0.5f)/200.0f);
  fade_out = fade_out * fade_out;

  return fade_out;
}

static float get_fade_in(float crossfade_value) {
  return 1.0f - get_fade_out(crossfade_value);
}

static float get_crossfade(kr_mxu *unit) {

  if (unit->crossfader->unit[0] == unit) {
    return get_fade_out(unit->crossfader->fade);
  }

  if (unit->crossfader->unit[1] == unit) {
    return get_fade_in(unit->crossfader->fade);
  }

  failfast("failed to get unit for crossfade!");

  return 0.0f;
}

static void apply_effects(kr_mxu *port, int nframes) {

  if (port->effects->sample_rate != port->mixer->sample_rate) {
    kr_effects_set_sample_rate(port->effects, port->mixer->sample_rate);
  }
  // FIXME hrm we count on thems being the same btw in them effects lookout
  kr_effects_process(port->effects, port->samples, port->samples, nframes);
}

static void apply_volume(kr_mxu *unit, int nframes) {

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

static float read_peak_scaled(kr_mxu *unit) {

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

static float read_peak(kr_mxu *unit) {

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

static void compute_meters(kr_mxu *unit, uint32_t nframes) {

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
    unit->win = unit->win % KRAD_MIXER_MAX_MINIWINS;
  }
  for (c = 0; c < unit->channels; c++) {
    level = 0;
    s = 0;
    w = unit->win;
    mw = unit->win + KRAD_MIXER_MAX_MINIWINS;
    while (s < unit->mixer->avg_window_size) {
      level += unit->wins[c][w % KRAD_MIXER_MAX_MINIWINS];
      s += unit->winss[w % KRAD_MIXER_MAX_MINIWINS];
      w++;
      if (w == mw) break;
    }
    unit->avg[c] = level / (float)(w - unit->win);
  }
}

static void clear_samples(kr_mxu *unit, uint32_t nframes) {

  int c;
  int s;

  for (c = 0; c < unit->channels; c++) {
    for (s = 0; s < nframes; s++) {
      unit->samples[c][s] = 0.0f;
    }
  }
}

static int handle_delay(kr_mxu *unit, uint32_t nframes) {

  int delay_frames;

  if (unit->delay > unit->delay_actual) {
    delay_frames = unit->delay - unit->delay_actual;
    if (delay_frames > nframes) {
      delay_frames = nframes;
    }
    clear_samples(unit, delay_frames);
    unit->delay += delay_frames;
    return nframes - delay_frames;
  }

  return nframes;
}

static void get_samples(kr_mxu *unit, uint32_t nframes) {

  int c;
  float *samples[KRAD_MIXER_MAX_CHANNELS];

  switch (unit->io_type) {
    case KRAD_TONE:
      krad_tone_run(unit->io_ptr, *unit->mapped_samples[0], nframes);
      break;
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      krad_audio_unit_samples_callback(nframes, unit->io_ptr, samples);
      for (c = 0; c < KRAD_MIXER_MAX_CHANNELS; c++ ) {
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
}

static void update_samples(kr_mxu *unit, uint32_t nframes) {
  if (unit->delay != unit->delay_actual) {
    nframes = handle_delay(unit, nframes);
  }
  get_samples(unit, nframes);
}

static void mix(kr_mxu *dest, kr_mxu *src, uint32_t nframes) {

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

static void copy_samples(kr_mxu *dest, kr_mxu *src, uint32_t nframes) {

  int c;
  int s;

  // Do mixdown here?

  switch (dest->io_type) {
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
}

static void limit(kr_mxu *unit, uint32_t nframes) {

  int c;

  for (c = 0; c < unit->channels; c++) {
    krad_hardlimit(unit->samples[c], nframes);
  }
}

static void update_state(kr_mixer *mixer) {

  int p;
  kr_mxu *unit;

  p = 0;
  unit = NULL;

  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 3)) {
      unit->active = 4;
    }
    if ((unit != NULL) && (unit->active == 1)) {
      unit->active = 2;
    }
  }
}

static void update_volume(kr_mxu *unit) {

  int c;

  for (c = 0; c < unit->channels; c++) {
    set_channel_volume(unit, c, unit->volume[c]);
  }
}

static void ticker_thread_cleanup(void *arg) {

  kr_mixer *mixer = (kr_mixer *)arg;

  if (mixer->ticker != NULL) {
    krad_ticker_destroy(mixer->ticker);
    mixer->ticker = NULL;
    printk("Krad Mixer: Synthetic Timer Destroyed");
  }
}

static void *ticker_thread(void *arg) {

  kr_mixer *mixer = (kr_mixer *)arg;

  krad_system_set_thread_name("kr_mixer");
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  mixer->ticker = krad_ticker_create(mixer->sample_rate, mixer->period_size);
  pthread_cleanup_push(ticker_thread_cleanup, mixer);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  krad_ticker_start_at(mixer->ticker, mixer->start_time);
  while (mixer->ticker_running == 1) {
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    mixer_process(mixer, mixer->period_size);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    krad_ticker_wait(mixer->ticker);
  }
  pthread_cleanup_pop(1);
  return NULL;
}

static void mixer_stop_ticker(kr_mixer *mixer) {
  if (mixer->ticker_running == 1) {
    mixer->ticker_running = 2;
    pthread_cancel(mixer->ticker_thread);
    pthread_join(mixer->ticker_thread, NULL);
    mixer->ticker_running = 0;
  }
}

static void mark_destroy(kr_mixer *mixer, kr_mxu *unit) {
  if (mixer->destroying == 2) {
    unit->active = 4;
  } else {
    if (unit->destroy_mark != 1) {
      unit->destroy_mark = 1;
      unit->active = 3;
    }
  }
}

static void set_crossfade(kr_mxu *unit, float value) {
  if (unit->crossfader != NULL) {
    crossfader_set_crossfade(unit->crossfader, value);
  }
}

static void crossfader_set_crossfade(kr_mxcf *crossfader, float value) {
  if (crossfader != NULL) {
    crossfader->fade = value;
    if ((crossfader->unit[0] != NULL) && (crossfader->unit[1] != NULL)) {
      update_volume(crossfader->unit[0]);
      update_volume(crossfader->unit[1]);
    }
  }
}

static void set_volume(kr_mxu *unit, float value) {

  int c;
  float volume_temp;

  volume_temp = (value/100.0f);
  volume_temp *= volume_temp;

  if (unit->direction == INPUT) {
    volume_temp = volume_temp * unit->bus->volume_actual[0];

    if (unit->crossfader != NULL) {
      volume_temp = volume_temp * get_crossfade(unit);
    }
  }

  for (c = 0; c < unit->channels; c++) {
    unit->volume[c] = value;
    if (unit->direction == MIX) {
      unit->volume_actual[c] = volume_temp;
    } else {
      unit->new_volume_actual[c] = volume_temp;
    }
  }

  if (unit->direction == MIX) {

    int p;
    kr_mxu *pg;

    for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
      pg = unit->mixer->unit[p];
      if ((pg != NULL) && (pg->active == 2) && (pg->direction == INPUT)) {
        if (pg->bus == unit) {
          update_volume(pg);
        }
      }
    }
  }
}

static void set_channel_volume(kr_mxu *unit, int channel, float value) {

  float volume_temp;

  if (unit->direction == INPUT) {
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

static void update_meter_readings(kr_mxu *unit) {

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
  char tone_str[2];

  client = NULL;
  kr_mxu *unit = NULL;
  kr_mxu *bus = NULL;
  kr_mixer_crossfader *crossfader = NULL;

  update_state(mixer);

  if (mixer->push_tone != -1) {
    tone_str[0] = mixer->push_tone;
    tone_str[1] = '\0';
    krad_tone_add_preset(mixer->tone->io_ptr, tone_str);
    mixer->push_tone = -1;
  }

  // Gets input/output port buffers
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 2) &&
       ((unit->direction == INPUT) || (unit->direction == OUTPUT))) {
      update_samples(unit, nframes);
    }
  }

  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS / 2; p++) {
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

  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
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
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 2) &&
        (unit->direction == INPUT)) {
      apply_volume(unit, nframes);
      //experiment
      if (unit->volume_actual[0] != 0.0f) {
        apply_effects(unit, nframes);
      }
      compute_meters(unit, nframes);
    }
  }

  // Clear Mixes
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 2) &&
        (unit->io_type == MIXBUS)) {
      clear_samples(unit, nframes);
    }
  }

  // Mix
  for (m = 0; m < KRAD_MIXER_MAX_PORTGROUPS; m++) {
    bus = mixer->unit[m];
    if ((bus != NULL) && (bus->active) && (bus->io_type == MIXBUS)) {
      for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
        unit = mixer->unit[p];
        if ((unit != NULL) && (unit->active == 2) &&
            (unit->bus == bus) && (unit->direction == INPUT)) {
          mix(bus, unit, nframes);
        }
      }
    }
  }

  // copy to outputs, limit all outputs
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && (unit->active == 2) &&
        (unit->direction == OUTPUT)) {
      copy_samples(unit, unit->bus, nframes);
      if (unit->output_type == AUX) {
        apply_volume(unit, nframes);
      }
      limit(unit, nframes);
    }
  }

  if (mixer->master != NULL) {
    compute_meters(mixer->master, nframes);
  }

  mixer->frames_since_peak_read += nframes;

  if (mixer->frames_since_peak_read >= mixer->frames_per_peak_broadcast) {
    mixer->frames_since_peak_read = 0;
    for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
      unit = mixer->unit[p];
      if ((unit != NULL) && (unit->active == 2) &&
          (unit->direction == INPUT)) {
        update_meter_readings(unit);
      }
    }
    if (mixer->master != NULL) {
      update_meter_readings(mixer->master);
    }
  }
  return 0;
}

void kr_mixer_cf_attatch(kr_mixer *mixer, kr_mxu *unit1, kr_mxu *unit2) {

  int i;
  kr_mixer_crossfader *crossfader;

  if ((unit1 == NULL) || ((unit1->active != 1) && (unit1->active != 2)) ||
      (unit2 == NULL) || ((unit2->active != 1) && (unit2->active != 2))) {
    printke("Invalid unit for crossfade!");
    return;
  }
  if (!(((unit1->direction == INPUT) || (unit1->direction == MIX)) &&
      ((unit1->direction == INPUT) || (unit2->direction == MIX)) &&
      (((unit1->io_type == MIXBUS) && (unit2->io_type == MIXBUS)) ||
       ((unit1->io_type != MIXBUS) && (unit2->io_type != MIXBUS))))) {
    printke("Invalid crossfade group!");
    return;
  }
  if (unit1->crossfader != NULL) {
    kr_mixer_cf_detatch(mixer, unit1->crossfader);
  }
  if (unit2->crossfader != NULL) {
    kr_mixer_cf_detatch(mixer, unit2->crossfader);
  }
  for (i = 0; i < KRAD_MIXER_MAX_PORTGROUPS / 2; i++) {
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
   &unit1->address, KR_CROSSFADE_GROUP, KR_STRING, unit2->name, NULL );

  kr_mixer_control(mixer, unit1->name, "crossfade", -100.0f, 0, NULL);
}

void kr_mixer_cf_detatch(kr_mixer *mixer, kr_mixer_crossfader *crossfader) {

  kr_mxu *unit[2];

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
    kr_mixer_control(mixer, unit[0]->name, "volume",
     unit[0]->volume[0], 0, NULL);
    kr_mixer_control(mixer, unit[1]->name, "volume",
     unit[1]->volume[0], 0, NULL);

    krad_radio_broadcast_subunit_update(mixer->as->app_broadcaster,
     &unit[0]->address, KR_CROSSFADE_GROUP, KR_STRING, "", NULL);
  }
}

kr_mxu *kr_mixer_unit_create (kr_mixer *mixer, char *name, int direction,
 kr_mixer_output_t output_type, int channels, float volume,
 kr_mixer_bus *bus, kr_mixer_unit_io_t io_type, void *io_ptr,
 krad_audio_api_t api) {

  int p;
  int c;
  kr_mxu *unit;

  unit = NULL;

  /* prevent dupe names */
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if (mixer->unit[p]->active != 0) {
      if (strncmp(name, mixer->unit[p]->name, strlen(name)) == 0) {
        return NULL;
      }
    }
  }

  //FIXME race here if unit being created via app and
  // transponder at same moment
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if (mixer->unit[p]->active == 0) {
      unit = mixer->unit[p];
      break;
    }
  }

  if (unit == NULL) {
    return NULL;
  }

  unit->mixer = mixer;
  strcpy(unit->name, name);
  unit->channels = channels;
  unit->io_type = io_type;
  unit->output_type = output_type;
  unit->bus = bus;
  unit->direction = direction;
  unit->address.path.unit = KR_MIXER;
  unit->address.path.subunit.mixer_subunit = KR_PORTGROUP;
  strcpy (unit->address.id.name, unit->name);

  for (c = 0; c < KRAD_MIXER_MAX_CHANNELS; c++) {
    if (c < unit->channels) {
      unit->mixmap[c] = c;
    } else {
      unit->mixmap[c] = -1;
    }
    unit->map[c] = c;
    unit->mapped_samples[c] = &unit->samples[c];
    if ((unit->direction != OUTPUT) || (unit->output_type == AUX)) {
      unit->volume[c] = volume;
    } else {
      unit->volume[c] = 100.0f;
    }
    unit->volume_actual[c] = (float)(unit->volume[c]/100.0f);
    unit->volume_actual[c] *= unit->volume_actual[c];
    unit->new_volume_actual[c] = unit->volume_actual[c];

    switch (unit->io_type) {
      case KRAD_TONE:
        unit->samples[c] = calloc (1, 16384);
        break;
      case MIXBUS:
        unit->samples[c] = calloc (1, 16384);
        break;
      case KRAD_AUDIO:
        break;
      case KRAD_LINK:
        unit->samples[c] = calloc (1, 16384);
        break;
      case KLOCALSHM:
        break;
    }
  }

  switch (unit->io_type) {
    case KRAD_TONE:
      unit->io_ptr = krad_tone_create(mixer->sample_rate);
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      unit->io_ptr = krad_audio_unit_create(mixer->krad_audio,
       unit->name, kr_mixer_dir_to_kr_audio_dir(unit->direction),
       unit->channels, api);
      break;
    case KRAD_LINK:
      unit->io_ptr = io_ptr;
      break;
    case KLOCALSHM:
      unit->io_ptr = io_ptr;
      break;
  }

  if (unit->io_type != KRAD_LINK) {
    unit->tags = krad_tags_create(unit->name);
    //if ((unit->krad_tags != NULL) && (mixer->app != NULL)) {
    //  krad_tags_set_set_tag_callback (unit->krad_tags, mixer->app,
    //  (void (*)(void *, char *, char *, char *, int))
    //  krad_app_server_broadcast_tag);
    //}
  } else {
    unit->tags = krad_link_get_tags(unit->io_ptr);
  }

  if (unit->tags == NULL) {
    failfast ("Oh I couldn't find me tags");
  }

  unit->effects = kr_effects_create(unit->channels, unit->mixer->sample_rate);

  if (unit->effects == NULL) {
    failfast ("Oh I couldn't make effects");
  }

  if (unit->direction == INPUT) {
    kr_effects_effect_add2(unit->effects, kr_effects_string_to_effect("eq"),
    unit->mixer unit->name);
    kr_effects_effect_add2(unit->effects,
     kr_effects_string_to_effect("lowpass"), unit->mixer, unit->name);
    kr_effects_effect_add2(unit->effects,
     kr_effects_string_to_effect("highpass"), unit->mixer, unit->name);
    kr_effects_effect_add2(unit->effects,
     kr_effects_string_to_effect("analog"), unit->mixer, unit->name);
  }

  if (unit->io_type != KLOCALSHM) {
    unit->active = 1;
  }

  return unit;
}

void kr_mixer_unit_destroy(kr_mixer *mixer, kr_mxu *unit) {

  int c;

  if (unit == NULL) {
    return;
  }

  if (unit->crossfader != NULL) {
    kr_mixer_cf_detatch(mixer, unit->crossfader);
  }

  mark_destroy(mixer, unit);

  while (unit->active != 4) {
    usleep(1000);
  }

  unit->delay = 0;
  unit->delay_actual = 0;

  printk("Krad Mixer: Removing %s", unit->name);

  for (c = 0; c < KRAD_MIXER_MAX_CHANNELS; c++) {
    switch (unit->io_type) {
      case KRAD_TONE:
        free(unit->samples[c]);
        break;
      case MIXBUS:
        free(unit->samples[c]);
        break;
      case KRAD_AUDIO:
        break;
      case KRAD_LINK:
        free(unit->samples[c]);
        break;
      case KLOCALSHM:
        break;
    }
  }

  switch (unit->io_type) {
    case KRAD_TONE:
      krad_tone_destroy(unit->io_ptr);
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      krad_audio_portgroup_destroy(unit->io_ptr);
      break;
    case KRAD_LINK:
      break;
    case KLOCALSHM:
      kr_mixer_local_portgroup_destroy(unit->io_ptr);
      break;
  }

  if (unit->xmms != NULL) {
    krad_xmms_destroy(unit->xmms);
    unit->xmms = NULL;
  }

  if (unit->io_type != KRAD_LINK) {
    krad_tags_destroy(unit->tags);
    unit->tags = NULL;
  }

  if (unit->effects != NULL) {
    kr_effects_destroy(unit->effects);
    unit->effects = NULL;
  }

  unit->destroy_mark = 0;
  unit->active = 0;
}

kr_mxu *kr_mixer_get_unit_from_name(kr_mixer *mixer, char *name) {

  int p;
  int len;
  kr_mxu *unit;

  len = strlen(name);

  if (len > 0) {
    for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
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

int kr_mixer_control(kr_mixer *mixer, char *name, char *control, float value,
 int duration, void *ptr) {

  kr_mxu *unit;

  unit = kr_mixer_unit_from_name(mixer, name);

  if (unit != NULL) {
    if ((unit->direction == OUTPUT) &&
        (unit->output_type == DIRECT)) {
      return 1;
    }
    if ((strncmp(control, "volume", 6) == 0) && (strlen(control) == 6)) {
      //unit_set_volume (unit, value);
      kr_easer_set(&unit->volume_easer, value, duration, EASEINOUTSINE, ptr);
      return 1;
    }
    if ((unit->direction == INPUT) && (unit->io_type != MIXBUS)) {
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

void kr_mixer_channel_move(kr_mixer_unit *unit, int in_chan, int out_chan) {
  unit->map[in_chan] = out_chan;
  unit->mapped_samples[in_chan] = &unit->samples[out_chan];
}

void kr_mixer_channel_copy(kr_mixer_unit *unit, int in_chan, int out_chan) {
  unit->mixmap[out_chan] = in_chan;
}

void kr_mixer_start_ticker(kr_mixer *mixer) {
  if (mixer->ticker_running == 1) {
    mixer_stop_ticker(mixer);
  }
  if (mixer->destroying == 0) {
    clock_gettime(CLOCK_MONOTONIC, &mixer->start_time);
    mixer->ticker_running = 1;
    pthread_create(&mixer->ticker_thread, NULL, ticker_thread, (void *)mixer);
  }
}

void kr_mixer_start_ticker_at(kr_mixer *mixer, struct timespec start_time) {
  if (mixer->ticker_running == 1) {
    mixer_stop_ticker(mixer);
  }
  if (mixer->destroying == 0) {
    memcpy(&mixer->start_time, &start_time, sizeof(struct timespec));
    mixer->ticker_running = 1;
    pthread_create(&mixer->ticker_thread, NULL, ticker_thread, (void *)mixer);
  }
}

void kr_mixer_unset_pusher(kr_mixer *mixer) {
  if (mixer->ticker_running == 1) {
    mixer_stop_ticker(mixer);
  }
  if (kr_mixer_period(mixer) != KRAD_MIXER_DEFAULT_PERIOD_SIZE) {
    kr_mixer_period_set(mixer, KRAD_MIXER_DEFAULT_PERIOD_SIZE);
  }
  kr_mixer_start_ticker(mixer);
  mixer->pusher = 0;
}

void kr_mixer_set_pusher(kr_mixer *mixer, krad_audio_api_t pusher) {
  if (mixer->ticker_running == 1) {
    mixer_stop_ticker(mixer);
  }
  mixer->pusher = pusher;
}

int kr_mixer_has_pusher(kr_mixer *mixer) {
  if (mixer->pusher == 0) {
    return 0;
  } else {
    return 1;
  }
}

krad_audio_api_t kr_mixer_get_pusher(kr_mixer *mixer) {
  return mixer->pusher;
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
  krad_tone_set_sample_rate(mixer->tone->io_ptr, mixer->sample_rate);
  if (mixer->ticker_running == 1) {
    mixer_stop_ticker(mixer);
    kr_mixer_start_ticker(mixer);
  }
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
  if (mixer->pusher != JACK) {
    mixer_stop_ticker(mixer);
    mixer->destroying = 2;
  }
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if ((mixer->unit[p]->active == 2) &&
        (mixer->unit[p]->io_type != MIXBUS)) {
      mark_destroy(mixer, mixer->unit[p]);
    }
  }
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if ((mixer->unit[p]->active != 0) &&
        (mixer->unit[p]->io_type != MIXBUS)) {
      kr_mixer_unit_destroy(mixer, mixer->unit[p]);
    }
  }
  mixer->destroying = 2;
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if (mixer->unit[p]->active != 0) {
      kr_mixer_unit_destroy(mixer, mixer->unit[p]);
    }
  }
  free(mixer->crossfader);
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    free(mixer->unit[p]);
  }
  free(mixer);
  printk("Krad Mixer shutdown complete");
  return 0;
}

kr_mixer *kr_mixer_create() {

  int p;
  kr_mixer *mixer;

  if ((mixer = calloc(1, sizeof(kr_mixer_t))) == NULL) {
    failfast("Krad Mixer memory alloc failure");
  }

  mixer->address.path.unit = KR_MIXER;
  mixer->address.path.subunit.mixer_subunit = KR_UNIT;
  mixer->sample_rate = KRAD_MIXER_DEFAULT_SAMPLE_RATE;
  mixer->avg_window_size = (mixer->sample_rate / 1000) *
   KRAD_MIXER_RMS_WINDOW_SIZE_MS;
  mixer->period_size = KRAD_MIXER_DEFAULT_PERIOD_SIZE;
  mixer->frames_per_peak_broadcast = 1536;
  mixer->crossfader = calloc(KRAD_MIXER_MAX_PORTGROUPS / 2,
   sizeof(kr_mixer_crossfader));
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    mixer->unit[p] = calloc(1, sizeof(kr_mixer_unit));
  }
  mixer->master = kr_mixer_unit_create(mixer, "MasterBUS", MIX, NOTOUTPUT, 2,
   DEFAULT_MASTERBUS_LEVEL, NULL, MIXBUS, NULL, 0);
  return mixer;
}
