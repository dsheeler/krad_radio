#include "krad_mixer.h"

typedef enum {
  KR_MXP_NIL,
  KR_MXP_READY,
  KR_MXP_ACTIVE,
  KR_MXP_TERM
} mixer_path_state;

static float get_fade_in(float crossfade_value);
static float get_fade_out(float crossfade_value);
static float get_crossfade(kr_mixer_path *path);
static void apply_effects(kr_mixer_path *path, int nframes);
static void apply_volume(kr_mixer_path *path, int nframes);
static void mixer_process(kr_mixer *mixer, uint32_t frames);
static void update_state(kr_mixer *mixer);
static void limit(kr_mixer_path *path, uint32_t nframes);
static void copy_frames(kr_mixer_path *dest, kr_mixer_path *src, uint32_t n);
static void audio_cb(kr_mixer_path *path, uint32_t nframes);
static void mix_frames(kr_mixer_path *dest, kr_mixer_path *src, uint32_t n);
static void pull_frames(kr_mixer_path *path, uint32_t nframes);
static int handle_delay(kr_mixer_path *path, uint32_t nframes);
static void clear_frames(kr_mixer_path *path, uint32_t nframes);
static void update_volume(kr_mixer_path *path);
static void set_channel_volume(kr_mixer_path *path, int channel, float value);
static void set_volume(kr_mixer_path *path, float value);
static void set_crossfade(kr_mixer_path *path, float value);
static void path_release(kr_mixer_path *path);

#include "metering.c"

static float get_fade_in(float crossfade_value) {
  return 1.0f - get_fade_out(crossfade_value);
}

static float get_fade_out(float crossfade_value) {
  float fade_out;
  fade_out = cos (3.14159f*0.5f*((crossfade_value + 100.0f) + 0.5f)/200.0f);
  return fade_out * fade_out;
}

static float get_crossfade(kr_mixer_path *path) {
  if (path->crossfader->path[0] == path) {
    return get_fade_out(path->crossfader->fade);
  } else if (path->crossfader->path[1] == path) {
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

static void import_frames(kr_mixer_path *dest, kr_audio *src) {

  int s;
  int c;
  int frames;
  int channels;

  frames = MIN(dest->mixer->period_size, src->count);
  channels = MIN(dest->channels, src->channels);

  for (c = 0; c < channels; c++) {
    for (s = 0; s < frames; s++) {
      dest->samples[c][s] = src->samples[c][s];
    }
  }
}

static void export_frames(kr_audio *dest, kr_mixer_path *src) {

  int s;
  int c;
  int frames;
  int channels;

  frames = dest->count;
  channels = dest->channels;

  for (c = 0; c < channels; c++) {
    for (s = 0; s < frames; s++) {
      dest->samples[c][s] = src->samples[c][s];
    }
  }
}

static void audio_cb(kr_mixer_path *path, uint32_t nframes) {

  kr_mixer_path_audio_cb_arg cb_arg;

  cb_arg.audio.channels = path->channels;
  cb_arg.audio.count = nframes;
  cb_arg.audio.rate = path->mixer->sample_rate;
  cb_arg.type = path->type;
  cb_arg.user = path->user;
  cb_arg.path = path;
  path->audio_cb(&cb_arg);
  if (path->type == KR_MXR_INPUT) {
    import_frames(path, &cb_arg.audio);
  } else {
    export_frames(&cb_arg.audio, path);
  }
}

static void pull_frames(kr_mixer_path *path, uint32_t nframes) {
  if (path->delay != path->delay_actual) {
    nframes = handle_delay(path, nframes);
  }
  audio_cb(path, nframes);
}

static void limit(kr_mixer_path *path, uint32_t nframes) {
  int c;
  for (c = 0; c < path->channels; c++) {
    krad_hardlimit(path->samples[c], nframes);
  }
}

static void copy_frames(kr_mixer_path *dest, kr_mixer_path *src, uint32_t n) {

  int s;
  int c;

  for (c = 0; c < dest->channels; c++) {
    for (s = 0; s < n; s++) {
      dest->samples[c][s] = src->samples[c][s];
    }
  }
}

static void mix_frames(kr_mixer_path *dest, kr_mixer_path *src, uint32_t n) {

  int c;
  int s;

  if (dest->channels == src->channels) {
    for (c = 0; c < dest->channels; c++) {
      for (s = 0; s < n; s++) {
        dest->samples[c][s] += src->samples[c][s];
      }
    }
  } else {
    //up mix
    for (c = 0; c < dest->channels; c++) {
      if (src->mixmap[c] != -1) {
        for (s = 0; s < n; s++) {
          dest->samples[c][s] += src->samples[src->mixmap[c]][s];
        }
      }
    }
  }
}

static void update_controls(kr_mixer *mixer) {

  int p;
  void *client;
  kr_mixer_path *path;
  kr_mixer_crossfader *crossfader;

  client = NULL;
  path = NULL;
  crossfader = NULL;

  for (p = 0; p < KR_MXR_MAX_PATHS / 2; p++) {
    crossfader = &mixer->crossfader[p];
    if ((crossfader != NULL) &&
        ((crossfader->path[0] != NULL) &&
        (crossfader->path[1] != NULL))) {
      if ((crossfader->easer.active) || (crossfader->easer.updated)) {
        set_crossfade(crossfader->path[0], kr_easer_process(&crossfader->easer,
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
    if ((path != NULL) && (path->state == KR_MXP_ACTIVE) &&
        ((path->volume_easer.active) || (path->volume_easer.updated))) {
      set_volume(path,
      kr_easer_process(&path->volume_easer, path->volume[0], &client));
     /* krad_radio_broadcast_subpath_control(mixer->broadcaster,
      * &path->address, KR_VOLUME, path->volume[0], client);
      */
    }
  }
}

static void update_state(kr_mixer *mixer) {

  int p;
  kr_mixer_path *path;

  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if (path != NULL) {
      switch (path->state) {
        case KR_MXP_READY:
          path->state = KR_MXP_ACTIVE;
          break;
        case KR_MXP_TERM:
          path_release(path);
          break;
        default:
          break;
      }
    }
  }

  if (mixer->new_sample_rate != mixer->sample_rate) {
    mixer->sample_rate = mixer->new_sample_rate;
    for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
      path = mixer->path[p];
      kr_sfx_sample_rate_set(path->sfx, mixer->sample_rate);
    }
  }
  update_controls(mixer);
}

static void update_volume(kr_mixer_path *path) {

  int c;

  for (c = 0; c < path->channels; c++) {
    set_channel_volume(path, c, path->volume[c]);
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
      if ((pg != NULL)
          && (pg->state == KR_MXP_ACTIVE)
          && (pg->type == KR_MXR_INPUT)) {
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

static void mixer_process(kr_mixer *mixer, uint32_t nframes) {

  int p;
  int m;
  kr_mixer_path *path;
  kr_mixer_path *bus;

  path = NULL;
  bus = NULL;

  update_state(mixer);

  /* Pull input audio */
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->state == KR_MXP_ACTIVE)
        && ((path->type == KR_MXR_INPUT))) {
      pull_frames(path, nframes);
    }
  }

  /* Process input and compute metrics */
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->state == KR_MXP_ACTIVE) &&
        (path->type == KR_MXR_INPUT)) {
      apply_volume(path, nframes);
      //experiment
      if (path->volume_actual[0] != 0.0f) {
        apply_effects(path, nframes);
      }
      compute_meters(path, nframes);
    }
  }

  /* Mix, process and compute metrics */
  for (m = 0; m < KR_MXR_MAX_PATHS; m++) {
    bus = mixer->path[m];
    if ((bus != NULL) && (bus->state) && (bus->type == KR_MXR_BUS)) {
      for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
        path = mixer->path[p];
        if ((path != NULL) && (path->state == KR_MXP_ACTIVE) &&
            (path->bus == bus) && (path->type == KR_MXR_INPUT)) {
          mix_frames(bus, path, nframes);
        }
      }
    }
  }

  /* Copy output and process */
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->state == KR_MXP_ACTIVE)
     && (path->type == KR_MXR_OUTPUT) && (path->bus != NULL)) {
      copy_frames(path, path->bus, nframes);
      apply_volume(path, nframes);
      limit(path, nframes);
    }
  }

  /* Push output */
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->state == KR_MXP_ACTIVE)
        && (path->type == KR_MXR_OUTPUT)) {
      audio_cb(path, nframes);
    }
  }

  /* Compute metrics */
  /*
  compute_meters(mixer->..., nframes);
  */
  mixer->frames_since_peak_read += nframes;
  if (mixer->frames_since_peak_read >= mixer->frames_per_peak_broadcast) {
    mixer->frames_since_peak_read = 0;
    for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
      path = mixer->path[p];
      if ((path != NULL) && (path->state == KR_MXP_ACTIVE)) {
        update_meter_readings(path);
      }
    }
    /*
    update_meter_readings(mixer->....);
    */
  }

  /* Clear mixes for next cycle */
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    path = mixer->path[p];
    if ((path != NULL) && (path->state == KR_MXP_ACTIVE) &&
        (path->type == KR_MXR_BUS)) {
      clear_frames(path, nframes);
    }
  }
}

static uint32_t ms_to_cycles(kr_mixer *mixer, int ms) {

  uint32_t cycles;
  float samples_ms;
  float cycle_ms;

  if ((ms < 1) || (ms > (10 * 60 * 1000))) {
    return 0;
  }

  samples_ms = mixer->sample_rate / 1000.0f;
  cycle_ms = mixer->period_size / samples_ms;
  cycles = (ms / cycle_ms) + 1;

  return cycles;
}

void kr_mixer_xf_couple(kr_mixer *mixer, kr_mixer_path *path1,
 kr_mixer_path *path2) {

  int i;
  kr_mixer_crossfader *crossfader;

  if ((path1 == NULL) || ((path1->state != 1) && (path1->state != 2)) ||
      (path2 == NULL) || ((path2->state != 1) && (path2->state != 2))) {
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
  kr_mixer_path_ctl(path1, "crossfade", -100.0f, 0, NULL);
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
    kr_mixer_path_ctl(path[0], "volume", path[0]->volume[0], 0, NULL);
    kr_mixer_path_ctl(path[1], "volume", path[1]->volume[0], 0, NULL);
    /*
     * krad_radio_broadcast_subpath_update(mixer->as->app_broadcaster,
     * &path[0]->address, KR_CROSSFADE_GROUP, KR_STRING, "", NULL);
     */
  }
}

static void path_release(kr_mixer_path *path) {

  int c;

  printk("Krad Mixer: Destroying %s", path->name);

  path->delay = 0;
  path->delay_actual = 0;

  for (c = 0; c < KR_MXR_MAX_CHANNELS; c++) {
    free(path->samples[c]);
  }

  if (path->sfx != NULL) {
    kr_sfx_destroy(path->sfx);
    path->sfx = NULL;
  }

  path->state = KR_MXP_NIL;
}

static kr_mixer_path *path_alloc(kr_mixer *mixer) {

  int i;

  for (i = 0; i < KR_MXR_MAX_PATHS; i++) {
    if (mixer->path[i]->state == KR_MXP_NIL) {
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

  if ((info->type != KR_MXR_INPUT)
      && (info->type != KR_MXR_BUS)
      && (info->type != KR_MXR_OUTPUT)) {
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
  if (setup->info_cb == NULL) return -3;
  if ((setup->info.type != KR_MXR_BUS) && (setup->audio_cb == NULL)) return -4;
  if ((setup->info.type == KR_MXR_BUS) && (setup->audio_cb != NULL)) return -5;
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
  path->info_cb = setup->info_cb;
  path->audio_cb = setup->audio_cb;
  path->user = setup->user;
  if (path->type == KR_MXR_BUS) {
    path->bus = NULL;
  } else {
    path->bus = kr_mixer_find(path->mixer, setup->info.bus);
  }
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
  path->state = KR_MXP_READY;
}

kr_mixer_path *kr_mixer_mkpath(kr_mixer *mixer, kr_mixer_path_setup *setup) {

  kr_mixer_path *path;

  if ((mixer == NULL) || (setup == NULL)) return NULL;
  if (path_setup_check(setup)) {
    printke("mixer mkpath failed setup check");
    return NULL;
  }
  path = kr_mixer_find(mixer, setup->info.name);
  if (path != NULL) {
    printke("mixer mkpath path with that name already exists");
    return NULL;
  }
  path = path_alloc(mixer);
  if (path == NULL) {
    printke("mixer mkpath could not alloc new path");
    return NULL;
  }
  path_create(path, setup);
  return path;
}

int kr_mixer_unlink(kr_mixer_path *path) {

  if (path == NULL) {
    printke("mixer path unlink called with null value");
    return -1;
  }

  printk("Krad Mixer: unlinking %s", path->name);

  if (path->crossfader != NULL) {
    kr_mixer_xf_decouple(path->mixer, path->crossfader);
  }

  path->state = KR_MXP_TERM;

  if (path->mixer->clock == NULL) {
    update_state(path->mixer);
  }

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
    if ((path != NULL)
        && ((path->state == KR_MXP_READY) || (path->state == KR_MXP_ACTIVE))) {
      if ((strlen(path->name) == len) &&
          (strncmp(name, path->name, len) == 0)) {
        return path;
      }
    }
  }
  return NULL;
}

int kr_mixer_path_ctl(kr_mixer_path *path, char *ctl, float value, int ms, void *ptr) {

  int duration;

  if (path == NULL) return -1;

  duration = ms_to_cycles(path->mixer, ms);
  if ((strncmp(ctl, "volume", 6) == 0) && (strlen(ctl) == 6)) {
    //path_set_volume (path, value);
    kr_easer_set(&path->volume_easer, value, duration, EASEINOUTSINE, ptr);
   return 0;
  }
  /* FIXME Add bus to bus and output to output crossfading */
  if ((path->type == KR_MXR_INPUT) && (path->type != KR_MXR_BUS)) {
    if ((strncmp(ctl, "crossfade", 9) == 0) && (strlen(ctl) == 9)) {
      if (path->crossfader != NULL) {
        //path_set_crossfade (path, value);
        kr_easer_set(&path->crossfader->easer, value, duration, EASEINOUTSINE,
         ptr);
      }
      return 0;
    }
    if (strncmp(ctl, "volume_left", 11) == 0) {
      set_channel_volume(path, 0, value);
      return 0;
    }
    if (strncmp(ctl, "volume_right", 12) == 0) {
      set_channel_volume(path, 1, value);
      return 0;
    }
  }
  return -2;
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
  mixer->new_period_size = period_size;
  if (mixer->period_size == 0) {
    mixer->period_size = period_size;
  }
  return mixer->new_period_size;
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

int kr_mixer_process(kr_mixer *mixer) {
  if (mixer == NULL) return -1;
  mixer_process(mixer, mixer->period_size);
  return mixer->period_size;
}

int kr_mixer_get_info(kr_mixer *mixer, kr_mixer_info *info) {

  kr_mixer_path *path;
  int i;

  if ((mixer == NULL) || (info == NULL)) return -1;
  memset(info, 0, sizeof(kr_mixer_info));

  for (i = 0; i < KR_MXR_MAX_PATHS; i++) {
    path = mixer->path[i];
    if ((path != NULL)
        && ((path->state == KR_MXP_READY) || (path->state == KR_MXP_ACTIVE))) {
      if (path->type == KR_MXR_INPUT) {
        info->inputs++;
      }
      if (path->type == KR_MXR_OUTPUT) {
        info->outputs++;
      }
      if (path->type == KR_MXR_BUS) {
        info->buses++;
      }
    }
  }

  info->period_size = mixer->period_size;
  info->sample_rate = mixer->sample_rate;

  strncpy(info->clock, "Something", sizeof(info->clock));

  return 0;
}

int kr_mixer_destroy(kr_mixer *mixer) {

  int p;

  printk("Krad Mixer shutdown started");

  if (mixer->clock != NULL) {
    for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
      if ((mixer->path[p]->state != KR_MXP_NIL) &&
          (mixer->path[p]->type != KR_MXR_BUS)) {
        kr_mixer_unlink(mixer->path[p]);
      }
    }
  }
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    if (mixer->path[p]->state != KR_MXP_NIL) {
      kr_mixer_unlink(mixer->path[p]);
    }
  }
  if (mixer->clock == NULL) {
    update_state(mixer);
  } else {
    /* FIXME maybe we need to wait maybe not well see (dobut it) */
  }
  free(mixer->crossfader);
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    free(mixer->path[p]);
  }
  free(mixer);
  printk("Krad Mixer shutdown complete");
  return 0;
}

kr_mixer *kr_mixer_create(kr_mixer_setup *setup) {

  int p;
  kr_mixer *mixer;

  if (setup == NULL) return NULL;

  mixer = calloc(1, sizeof(kr_mixer));
  mixer->user = setup->user;
  mixer->info_cb = setup->cb;
  kr_mixer_period_set(mixer, setup->period_size);
  kr_mixer_sample_rate_set(mixer, setup->sample_rate);
  mixer->avg_window_size = (mixer->sample_rate / 1000) * KR_MXR_RMS_WINDOW_MS;
  mixer->frames_per_peak_broadcast = 1536;
  mixer->crossfader = calloc(KR_MXR_MAX_PATHS / 2,
   sizeof(kr_mixer_crossfader));
  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    mixer->path[p] = calloc(1, sizeof(kr_mixer_path));
  }
  return mixer;
}

void kr_mixer_setup_init(kr_mixer_setup *setup) {

  if (setup == NULL) return;

  setup->period_size = KR_MXR_PERIOD_DEF;
  setup->sample_rate = KR_MXR_SRATE_DEF;
  setup->user = NULL;
  setup->cb = NULL;
}
