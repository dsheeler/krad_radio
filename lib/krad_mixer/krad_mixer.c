#include "krad_mixer.h"

static int mixer_process(kr_mixer *mixer, uint32_t frames);
static void update_state(kr_mixer *mixer);
static void mark_destroy(kr_mixer *mixer, kr_mixer_path *path);
static void limit(kr_mixer_path *path, uint32_t nframes);
static void copy_frames(kr_mixer_path *d, kr_mixer_path *s, uint32_t nframes);
static void mix(kr_mixer_path *dest, kr_mixer_path *src, uint32_t nframes);
static void update_frames(kr_mixer_path *path, uint32_t nframes);
static void get_frames(kr_mixer_path *path, uint32_t nframes);
static int handle_delay(kr_mixer_path *path, uint32_t nframes);
static void clear_frames(kr_mixer_path *path, uint32_t nframes);
static float read_peak(kr_mixer_path *path);
static float read_peak_scaled(kr_mixer_path *path);
static void update_meter_readings(kr_mixer_path *path);
static void compute_meters(kr_mixer_path *path, uint32_t nframes);
static void update_volume(kr_mixer_path *path);
static void apply_volume(kr_mixer_path *path, int nframes);
static void apply_effects(kr_mixer_path *path, int nframes);
static float get_crossfade(kr_mixer_path *path);
static float get_fade_out(float crossfade_value);
static float get_fade_in(float crossfade_value);
static void set_channel_volume(kr_mixer_path *path, int channel, float value);
static void set_volume(kr_mixer_path *path, float value);
static void set_crossfade(kr_mixer_path *path, float value);

static float get_fade_out(float crossfade_value) {

  float fade_out;

  fade_out = cos (3.14159f*0.5f*((crossfade_value + 100.0f) + 0.5f)/200.0f);
  fade_out = fade_out * fade_out;

  return fade_out;
}

static float get_fade_in(float crossfade_value) {
  return 1.0f - get_fade_out(crossfade_value);
}

static float get_crossfade(kr_mixer_path *path) {

  if (path->crossfader->path[0] == path) {
    return get_fade_out(path->crossfader->fade);
  }

  if (path->crossfader->path[1] == path) {
    return get_fade_in(path->crossfader->fade);
  }

  failfast("failed to get path for crossfade!");

  return 0.0f;
}

static void apply_effects(kr_mixer_path *port, int nframes) {
  // FIXME hrm we count on thems being the same btw in them effects lookout
  kr_sfx_process(port->sfx, port->samples, port->samples, nframes);
}

static void apply_volume(kr_mixer_path *path, int nframes) {

  int c;
  int s;
  int sign;

  sign = 0;

  for (c = 0; c < path->channels; c++) {
    if (path->new_volume_actual[c] == path->volume_actual[c]) {
      for (s = 0; s < nframes; s++) {
        path->samples[c][s] = path->samples[c][s] * path->volume_actual[c];
      }
    } else {
      /* The way the volume change is set up here, the volume can only
       * change once per callback, but thats allways plenty
       * last_sign: 0 = unset, -1 neg, +1 pos */
      if (path->last_sign[c] == 0) {
        if (path->samples[c][0] > 0.0f) {
          path->last_sign[c] = 1;
        } else {
          /* Zero counts as negative here, but its moot */
          path->last_sign[c] = -1;
        }
      }
      for (s = 0; s < nframes; s++) {
        if (path->last_sign[c] != 0) {
          if (path->samples[c][s] > 0.0f) {
            sign = 1;
          } else {
            sign = -1;
          }
          if ((sign != path->last_sign[c]) || (path->samples[c][s] == 0.0f)) {
            path->volume_actual[c] = path->new_volume_actual[c];
            path->last_sign[c] = 0;
          }
        }
        path->samples[c][s] = (path->samples[c][s] * path->volume_actual[c]);
      }
      if (path->last_sign[c] != 0) {
        path->last_sign[c] = sign;
      }
    }
  }
}

static float read_peak_scaled(kr_mixer_path *path) {

  float db;
  float def;

  db = 20.0f * log10f(read_peak(path) * 1.0f);
  db = 20.0f * log10f(path->avg[0] * 1.0f);
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

static float read_peak(kr_mixer_path *path) {

  //FIXME N channels

  float tmp = path->peak[0];
  path->peak[0] = 0.0f;

  float tmp2 = path->peak[1];
  path->peak[1] = 0.0f;
  if (tmp > tmp2) {
    return tmp;
  } else {
    return tmp2;
  }
}

static void compute_meters(kr_mixer_path *path, uint32_t nframes) {

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
    for (c = 0; c < path->channels; c++) {
      level = 0;
      for(s = cur_frame; s < cur_frame + cframes; s++) {
        sample = fabs(path->samples[c][s]);
        level += sample;
        if (sample > path->peak[c]) {
          path->peak[c] = sample;
        }
      }
      path->wins[c][path->win] = level / (float)cframes;
    }
    path->winss[path->win] = cframes;
    cur_frame += cframes;
    cframes = MIN(32, nframes - cur_frame);
    path->win++;
    path->win = path->win % KR_MXR_MAX_MINIWINS;
  }
  for (c = 0; c < path->channels; c++) {
    level = 0;
    s = 0;
    w = path->win;
    mw = path->win + KR_MXR_MAX_MINIWINS;
    while (s < path->mixer->avg_window_size) {
      level += path->wins[c][w % KR_MXR_MAX_MINIWINS];
      s += path->winss[w % KR_MXR_MAX_MINIWINS];
      w++;
      if (w == mw) break;
    }
    path->avg[c] = level / (float)(w - path->win);
  }
}

static void clear_frames(kr_mixer_path *path, uint32_t nframes) {

  int c;
  int s;

  for (c = 0; c < path->channels; c++) {
    for (s = 0; s < nframes; s++) {
      path->samples[c][s] = 0.0f;
    }
  }
}

static int handle_delay(kr_mixer_path *path, uint32_t nframes) {

  int delay_frames;

  if (path->delay > path->delay_actual) {
    delay_frames = path->delay - path->delay_actual;
    if (delay_frames > nframes) {
      delay_frames = nframes;
    }
    clear_frames(path, delay_frames);
    path->delay += delay_frames;
    return nframes - delay_frames;
  }

  return nframes;
}

static void get_frames(kr_mixer_path *path, uint32_t nframes) {

  int c;
  float *samples[KR_MXR_MAX_CHANNELS];
/*
  switch (path->type) {
    case KRAD_TONE:
      //krad_tone_run(path->io_ptr, *path->mapped_samples[0], nframes);
      break;
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      krad_audio_path_samples_callback(nframes, path->io_ptr, samples);
      for (c = 0; c < KR_MXR_MAX_CHANNELS; c++ ) {
        path->samples[c] = samples[path->map[c]];
      }
      break;
    case KRAD_LINK:
      if (path->direction == INPUT) {
        krad_link_audio_samples_callback(nframes, path->io_ptr,
         path->mapped_samples[0]);
      }
      break;
    case KLOCALSHM:
      if (path->direction == INPUT) {
        kr_mixer_local_audio_samples_callback(nframes, path->io_ptr,
         path->mapped_samples[0]);
      }
      break;
  }
  */
}

static void update_frames(kr_mixer_path *path, uint32_t nframes) {
  if (path->delay != path->delay_actual) {
    nframes = handle_delay(path, nframes);
  }
  get_frames(path, nframes);
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

static void limit(kr_mixer_path *path, uint32_t nframes) {

  int c;

  for (c = 0; c < path->channels; c++) {
    krad_hardlimit(path->samples[c], nframes);
  }
}

static void update_state(kr_mixer *mixer) {

  int p;
  kr_mixer_path *path;

  p = 0;
  path = NULL;

  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->active == 3)) {
      path->active = 4;
    }
    if ((path != NULL) && (path->active == 1)) {
      path->active = 2;
    }
  }
  if (mixer->new_sample_rate != mixer->sample_rate) {
    mixer->sample_rate = mixer->new_sample_rate;
    for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
      path = mixer->path[p];
      kr_sfx_sample_rate_set(path->sfx, mixer->sample_rate);
    }
  }
}

static void update_volume(kr_mixer_path *path) {

  int c;

  for (c = 0; c < path->channels; c++) {
    set_channel_volume(path, c, path->volume[c]);
  }
}

static void mark_destroy(kr_mixer *mixer, kr_mixer_path *path) {
  if (mixer->destroying == 2) {
    path->active = 4;
  } else {
    if (path->destroy_mark != 1) {
      path->destroy_mark = 1;
      path->active = 3;
    }
  }
}

static void set_crossfade(kr_mixer_path *path, float value) {

  kr_mixer_crossfader *crossfader;

  if (path->crossfader != NULL) {
    crossfader = path->crossfader;
    if ((crossfader->path[0] != NULL) && (crossfader->path[1] != NULL)) {
      crossfader->fade = value;
      update_volume(crossfader->path[0]);
      update_volume(crossfader->path[1]);
    }
  }
}

static void set_volume(kr_mixer_path *path, float value) {

  int c;
  float volume_temp;

  volume_temp = (value/100.0f);
  volume_temp *= volume_temp;
  //FIXME input gets bus volume, bus gets higher level bus vol
  if ((path->type == KR_MXR_INPUT) || (1 == 2)) {
    volume_temp = volume_temp * path->bus->volume_actual[0];

    if (path->crossfader != NULL) {
      volume_temp = volume_temp * get_crossfade(path);
    }
  }

  for (c = 0; c < path->channels; c++) {
    path->volume[c] = value;
    if (path->type == KR_MXR_BUS) {
      path->volume_actual[c] = volume_temp;
    } else {
      path->new_volume_actual[c] = volume_temp;
    }
  }

  if (path->type == KR_MXR_BUS) {

    int p;
    kr_mixer_path *pg;

    for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
      pg = path->mixer->path[p];
      if ((pg != NULL) && (pg->active == 2) && (pg->type == KR_MXR_INPUT)) {
        if (pg->bus == path) {
          update_volume(pg);
        }
      }
    }
  }
}

static void set_channel_volume(kr_mixer_path *path, int channel, float value) {

  float volume_temp;

  if (path->type == KR_MXR_INPUT) {
    path->volume[channel] = value;
    volume_temp = (path->volume[channel]/100.0f);
    volume_temp *= volume_temp;
    volume_temp = volume_temp * path->bus->volume_actual[0];
    if (path->crossfader != NULL) {
      volume_temp = volume_temp * get_crossfade(path);
    }
    path->new_volume_actual[channel] = volume_temp;
  }
}

static void update_meter_readings(kr_mixer_path *path) {

  float peak;

  peak = read_peak_scaled(path);
  if (peak != path->peak_last[0]) {
    path->peak_last[0] = peak;
   /* krad_radio_broadcast_subpath_control(path->mixer->broadcaster,
    *  &path->address, KR_PEAK, peak, NULL);
    */
  }
}

static int mixer_process(kr_mixer *mixer, uint32_t nframes) {

  int p, m;
  void *client;

  client = NULL;
  kr_mixer_path *path = NULL;
  kr_mixer_path *bus = NULL;
  kr_mixer_crossfader *crossfader = NULL;

  update_state(mixer);
  // Gets input/output port buffers
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->active == 2) &&
       //((path->type == KR_MXR_INPUT) || (path->type == KR_MXR_AUX))) {
       ((path->type == KR_MXR_INPUT))) {
      update_frames(path, nframes);
    }
  }

  for (p = 0; p < KR_MXR_MAX_PATHS / 2; p++) {
    crossfader = &mixer->crossfader[p];
    if ((crossfader != NULL) &&
        ((crossfader->path[0] != NULL) &&
        (crossfader->path[1] != NULL))) {
      if (crossfader->easer.active) {
        set_crossfade(crossfader->path[0],
         kr_easer_process(&crossfader->easer,
          crossfader->fade, &client));
       /* krad_radio_broadcast_subpath_control(mixer->broadcaster,
        * &crossfader->path[0]->address, KR_CROSSFADE,
        * crossfader->fade, client);
        */
      }
    }
  }

  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->active == 2) &&
        (path->volume_easer.active)) {
      set_volume(path,
       kr_easer_process(&path->volume_easer, path->volume[0],
       &client));
     /* krad_radio_broadcast_subpath_control(mixer->broadcaster,
      * &path->address, KR_VOLUME, path->volume[0], client);
      */
    }
  }

  // apply volume, effects and calc peaks on inputs
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->active == 2) &&
        (path->type == KR_MXR_INPUT)) {
      apply_volume(path, nframes);
      //experiment
      if (path->volume_actual[0] != 0.0f) {
        apply_effects(path, nframes);
      }
      compute_meters(path, nframes);
    }
  }

  // Clear Mixes
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->active == 2) &&
        (path->type == KR_MXR_BUS)) {
      clear_frames(path, nframes);
    }
  }

  // Mix
  for (m = 0; m < KR_MXR_MAX_PATHS; m++) {
    bus = mixer->path[m];
    if ((bus != NULL) && (bus->active) && (bus->type == KR_MXR_BUS)) {
      for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
        path = mixer->path[p];
        if ((path != NULL) && (path->active == 2) &&
            (path->bus == bus) && (path->type == KR_MXR_INPUT)) {
          mix(bus, path, nframes);
        }
      }
    }
  }
/*
  // copy to outputs, limit all outputs
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->active == 2) &&
        (path->direction == OUTPUT)) {
      copy_frames(path, path->bus, nframes);
      if (path->output_type == AUX) {
        apply_volume(path, nframes);
      }
      limit(path, nframes);
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
      path = mixer->path[p];
      if ((path != NULL) && (path->active == 2)) {
        update_meter_readings(path);
      }
    }
    if (mixer->master != NULL) {
      update_meter_readings(mixer->master);
    }
  }
  return 0;
}

void kr_mixer_xf_couple(kr_mixer *mixer, kr_mixer_path *path1,
 kr_mixer_path *path2) {

  int i;
  kr_mixer_crossfader *crossfader;

  if ((path1 == NULL) || ((path1->active != 1) && (path1->active != 2)) ||
      (path2 == NULL) || ((path2->active != 1) && (path2->active != 2))) {
    printke("Invalid path for crossfade!");
    return;
  }
  if (!(((path1->type == KR_MXR_INPUT) || (path1->type == KR_MXR_BUS)) &&
      ((path1->type == KR_MXR_INPUT) || (path2->type == KR_MXR_BUS)) &&
      (((path1->type == KR_MXR_BUS) && (path2->type == KR_MXR_BUS)) ||
       ((path1->type != KR_MXR_BUS) && (path2->type != KR_MXR_BUS))))) {
    printke("Invalid crossfade group!");
    return;
  }
  if (path1->crossfader != NULL) {
    kr_mixer_xf_decouple(mixer, path1->crossfader);
  }
  if (path2->crossfader != NULL) {
    kr_mixer_xf_decouple(mixer, path2->crossfader);
  }
  for (i = 0; i < KR_MXR_MAX_PATHS / 2; i++) {
    crossfader = &mixer->crossfader[i];
    if ((crossfader != NULL) && ((crossfader->path[0] == NULL) &&
        (crossfader->path[1] == NULL))) {
      break;
    }
  }

  crossfader->path[0] = path1;
  crossfader->path[1] = path2;
  path1->crossfader = crossfader;
  path2->crossfader = crossfader;

/*
 * krad_radio_broadcast_subpath_update(mixer->as->app_broadcaster,
 * &path1->address, KR_CROSSFADE_GROUP, KR_STRING, path2->name, NULL);
 */
  kr_mixer_ctl(mixer, path1->name, "crossfade", -100.0f, 0, NULL);
}

void kr_mixer_xf_decouple(kr_mixer *mixer, kr_mixer_crossfader *crossfader) {

  kr_mixer_path *path[2];

  path[0] = NULL;
  path[1] = NULL;

  if ((crossfader != NULL) && (crossfader->path[0] != NULL) &&
      (crossfader->path[1] != NULL)) {

    path[0] = crossfader->path[0];
    path[1] = crossfader->path[1];
    crossfader->path[0]->crossfader = NULL;
    crossfader->path[1]->crossfader = NULL;

    crossfader->path[0] = NULL;
    crossfader->path[1] = NULL;
    crossfader->fade = -100.0f;

    /* Hrm */
    kr_mixer_ctl(mixer, path[0]->name, "volume",
     path[0]->volume[0], 0, NULL);
    kr_mixer_ctl(mixer, path[1]->name, "volume",
     path[1]->volume[0], 0, NULL);
    /*
     * krad_radio_broadcast_subpath_update(mixer->as->app_broadcaster,
     * &path[0]->address, KR_CROSSFADE_GROUP, KR_STRING, "", NULL);
     */
  }
}

static kr_mixer_path *path_alloc(kr_mixer *mixer) {

  int i;

  for (i = 0; i < KR_MXR_MAX_PATHS; i++) {
    if (mixer->path[i]->active == 0) {
      mixer->path[i]->mixer = mixer;
      return mixer->path[i];
    }
  }
  return NULL;
}

static int path_setup_info_check(kr_mixer_path_info *info) {

  if (memchr(info->name + 1, '\0', sizeof(info->name) - 1) == NULL) {
    return -2;
  }
  if (strlen(info->name) == 0) return -3;

  if (info->channels < 1) return -1;
  if (info->channels > KR_MXR_MAX_CHANNELS) return -1;

  if ((info->type == KR_MXR_INPUT)
      || (info->type == KR_MXR_BUS)
      || (info->type == KR_MXR_AUX)) {
    return -4;
  }

  /* FIXME check all the things 
   *
   * bus exists
   * sfx params
   * volume
   * crossfade partner
   * matrix
   *
   * */

  return 0;
}

static int path_setup_check(kr_mixer_path_setup *setup) {
  if (setup->user == NULL) return -2;
  if ((setup->info.type != KR_MXR_BUS) && (setup->audio_cb == NULL)) return -3;
  if (path_setup_info_check(&setup->info)) return -4;
  return 0;
}

static void path_sfx_create(kr_mixer_path *path) {

  kr_sfx_setup sfx_setup;

  sfx_setup.channels = path->channels;
  sfx_setup.sample_rate = path->mixer->sample_rate;
  sfx_setup.user = path;
  sfx_setup.cb = NULL;
  /* FIXME actual sfx info callback
   * have sfx info callback hit path cb that propagates to path info cb
   * FIXME set sfx params from setup
   * */
  path->sfx = kr_sfx_create(&sfx_setup);
  kr_sfx_add(path->sfx, KR_SFX_EQ);
  kr_sfx_add(path->sfx, KR_SFX_LOWPASS);
  kr_sfx_add(path->sfx, KR_SFX_HIGHPASS);
  kr_sfx_add(path->sfx, KR_SFX_ANALOG);
}

static void path_create(kr_mixer_path *path, kr_mixer_path_setup *setup) {

  int c;

  strncpy(path->name, setup->info.name, sizeof(path->name));
  path->channels = setup->info.channels;
  path->type = setup->info.type;
  /* FIXME find my bus by name */
  path->bus = path->mixer->master;
  for (c = 0; c < KR_MXR_MAX_CHANNELS; c++) {
    if (c < path->channels) {
      /* FIXME take mapping from setup */
      path->mixmap[c] = c;
    } else {
      path->mixmap[c] = -1;
    }
    path->map[c] = c;
    path->mapped_samples[c] = &path->samples[c];
    path->volume[c] = setup->info.volume[c];
    path->volume_actual[c] = (float)(path->volume[c]/100.0f);
    path->volume_actual[c] *= path->volume_actual[c];
    path->new_volume_actual[c] = path->volume_actual[c];
    path->samples[c] = calloc(1, 4096);
  }
  path_sfx_create(path);
  path->active = 1;
  return path;
}

kr_mixer_path *kr_mixer_mkpath(kr_mixer *mixer, kr_mixer_path_setup *setup) {

  kr_mixer_path *path;

  if ((mixer == NULL) || (setup == NULL)) return NULL;
  if (path_setup_check(setup)) return NULL;
  path = kr_mixer_find(mixer, setup->info.name);
  if (path != NULL) return NULL;
  path = path_alloc(mixer);
  if (path == NULL) return NULL;
  path_create(path, setup);

  return path;
}


int kr_mixer_unlink(kr_mixer_path *path) {

  int c;
  kr_mixer *mixer;

  if (path == NULL) {
    return -1;
  }

  mixer = path->mixer;

  if (path->crossfader != NULL) {
    kr_mixer_xf_decouple(mixer, path->crossfader);
  }

  mark_destroy(mixer, path);

  while (path->active != 4) {
    usleep(1000);
  }

  path->delay = 0;
  path->delay_actual = 0;

  printk("Krad Mixer: Removing %s", path->name);

  for (c = 0; c < KR_MXR_MAX_CHANNELS; c++) {
    free(path->samples[c]);
  }

  if (path->sfx != NULL) {
    kr_sfx_destroy(path->sfx);
    path->sfx = NULL;
  }

  path->destroy_mark = 0;
  path->active = 0;

  return 0;
}

kr_mixer_path *kr_mixer_find(kr_mixer *mixer, char *name) {

  int p;
  int len;
  kr_mixer_path *path;

  if ((mixer == NULL) || (name == NULL)) return NULL;

  len = strlen(name);
  if (len < 1) return NULL;

  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && ((path->active == 1) || (path->active == 2))) {
      if ((strlen(path->name) == len) &&
          (strncmp(name, path->name, len) == 0)) {
        return path;
      }
    }
  }
  return NULL;
}

int kr_mixer_ctl(kr_mixer *mixer, char *name, char *control, float value,
 int duration, void *ptr) {

  kr_mixer_path *path;

  path = kr_mixer_find(mixer, name);

  if (path != NULL) {
    //if ((path->direction == OUTPUT) &&
    //    (path->output_type == DIRECT)) {
    //  return 1;
   // }
    if ((strncmp(control, "volume", 6) == 0) && (strlen(control) == 6)) {
      //path_set_volume (path, value);
      kr_easer_set(&path->volume_easer, value, duration, EASEINOUTSINE, ptr);
      return 1;
    }
    //Fixme add bus to bus crossfadin
    if ((path->type == KR_MXR_INPUT) && (path->type != KR_MXR_BUS)) {
      if ((strncmp(control, "crossfade", 9) == 0) && (strlen(control) == 9)) {
        if (path->crossfader != NULL) {
          //path_set_crossfade (path, value);
          kr_easer_set(&path->crossfader->easer, value, duration,
           EASEINOUTSINE, ptr);
        }
        return 1;
      }
      if (strncmp(control, "volume_left", 11) == 0) {
        set_channel_volume(path, 0, value);
        return 1;
      }
      if (strncmp(control, "volume_right", 12) == 0) {
        set_channel_volume(path, 1, value);
        return 1;
      }
    }
  }
  return 0;
}

void kr_mixer_channel_move(kr_mixer_path *path, int in_chan, int out_chan) {
  path->map[in_chan] = out_chan;
  path->mapped_samples[in_chan] = &path->samples[out_chan];
}

void kr_mixer_channel_copy(kr_mixer_path *path, int in_chan, int out_chan) {
  path->mixmap[out_chan] = in_chan;
}

uint32_t kr_mixer_period(kr_mixer *mixer) {
  return mixer->period_size;
}

int kr_mixer_period_set(kr_mixer *mixer, uint32_t period_size) {
  if ((period_size < KR_MXR_PERIOD_MIN) || (period_size > KR_MXR_PERIOD_MAX)) {
    return -1;
  }
  mixer->period_size = period_size;
  return mixer->period_size;
}

uint32_t kr_mixer_sample_rate(kr_mixer *mixer) {
  return mixer->sample_rate;
}

int kr_mixer_sample_rate_set(kr_mixer *mixer, uint32_t sample_rate) {
  if ((sample_rate < KR_MXR_SRATE_MIN) || (sample_rate > KR_MXR_SRATE_MAX)) {
    return -1;
  }
  mixer->new_sample_rate = sample_rate;
  if (mixer->sample_rate == 0) {
    mixer->sample_rate = sample_rate;
  }
  return mixer->new_sample_rate;
}

int kr_mixer_mix(kr_mixer *mixer) {
  return mixer_process(mixer, mixer->period_size);
}
/*
void kr_mixer_appserver_set(kr_mixer *mixer, kr_app_server *as) {
  mixer->as = as;
  mixer->broadcaster = krad_app_server_broadcaster_register(mixer->as);
}
*/
int kr_mixer_destroy(kr_mixer *mixer) {

  int p;

  printk("Krad Mixer shutdown started");

  mixer->destroying = 1;
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    if ((mixer->path[p]->active == 2) &&
        (mixer->path[p]->type != KR_MXR_BUS)) {
      mark_destroy(mixer, mixer->path[p]);
    }
  }
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    if ((mixer->path[p]->active != 0) &&
        (mixer->path[p]->type != KR_MXR_BUS)) {
      kr_mixer_unlink(mixer->path[p]);
    }
  }
  mixer->destroying = 2;
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    if (mixer->path[p]->active != 0) {
      kr_mixer_unlink(mixer->path[p]);
    }
  }
  free(mixer->crossfader);
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    free(mixer->path[p]);
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

kr_mixer *kr_mixer_create(kr_mixer_setup *setup) {

  int p;
  kr_mixer *mixer;

  if (setup == NULL) return NULL;

  if ((mixer = calloc(1, sizeof(kr_mixer))) == NULL) {
    failfast("Krad Mixer memory alloc failure");
  }

  kr_mixer_period_set(mixer, setup->period_size);
  kr_mixer_sample_rate_set(mixer, setup->sample_rate);
  mixer->avg_window_size = (mixer->sample_rate / 1000) * KR_MXR_RMS_WINDOW_MS;
  mixer->frames_per_peak_broadcast = 1536;
  mixer->crossfader = calloc(KR_MXR_MAX_PATHS / 2,
   sizeof(kr_mixer_crossfader));
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    mixer->path[p] = calloc(1, sizeof(kr_mixer_path));
  }
  kr_mixer_masterbus_setup(mixer);
  return mixer;
}

void kr_mixer_setup_init(kr_mixer_setup *setup) {

  if (setup == NULL) return;

  setup->period_size = KR_MXR_PERIOD_DEF;
  setup->sample_rate = KR_MXR_SRATE_DEF;
  setup->user = NULL;
  setup->cb = NULL;
}
