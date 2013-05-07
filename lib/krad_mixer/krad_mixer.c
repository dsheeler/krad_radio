#include "krad_mixer.h"

static void krad_mixer_ticker_thread_cleanup (void *arg);
static void *krad_mixer_ticker_thread (void *arg);
static void krad_mixer_stop_ticker (krad_mixer_t *krad_mixer);
static int krad_mixer_process (uint32_t nframes, krad_mixer_t *krad_mixer);
static int krad_mixer_local_audio_samples_callback (int nframes,
                                              krad_mixer_local_portgroup_t *krad_mixer_local_portgroup,
                                              float **samples);
static void krad_mixer_update_portgroups (krad_mixer_t *krad_mixer);
static void krad_mixer_portgroup_mark_destroy (krad_mixer_t *krad_mixer, krad_mixer_portgroup_t *portgroup);
static void krad_mixer_local_portgroup_destroy (krad_mixer_local_portgroup_t *krad_mixer_local_portgroup);
static void portgroup_limit (krad_mixer_portgroup_t *portgroup, uint32_t nframes);
static void portgroup_copy_samples (krad_mixer_portgroup_t *dest_portgroup, krad_mixer_portgroup_t *src_portgroup, uint32_t nframes);
static void portgroup_mix_samples (krad_mixer_portgroup_t *dest_portgroup, krad_mixer_portgroup_t *src_portgroup, uint32_t nframes);
static void portgroup_update_samples (krad_mixer_portgroup_t *portgroup, uint32_t nframes);
static void portgroup_get_samples (krad_mixer_portgroup_t *portgroup, uint32_t nframes);
static int portgroup_handle_delay (krad_mixer_portgroup_t *portgroup, uint32_t nframes);
static void portgroup_clear_samples (krad_mixer_portgroup_t *portgroup, uint32_t nframes);
static float krad_mixer_portgroup_read_peak (krad_mixer_portgroup_t *portgroup);
static float krad_mixer_portgroup_read_peak_scaled (krad_mixer_portgroup_t *portgroup);
static void krad_mixer_portgroup_update_meter_readings (krad_mixer_portgroup_t *portgroup);
//static float krad_mixer_portgroup_read_channel_peak (krad_mixer_portgroup_t *portgroup, int channel);
static void krad_mixer_portgroup_compute_meters (krad_mixer_portgroup_t *portgroup, uint32_t nframes);
static void krad_mixer_portgroup_compute_peaks (krad_mixer_portgroup_t *portgroup, uint32_t nframes);
static void krad_mixer_portgroup_compute_channel_peak (krad_mixer_portgroup_t *portgroup, int channel, uint32_t nframes);
static void portgroup_update_volume (krad_mixer_portgroup_t *portgroup);
static void portgroup_apply_volume (krad_mixer_portgroup_t *portgroup, int nframes);
static void portgroup_apply_effects (krad_mixer_portgroup_t *portgroup, int nframes);
static float portgroup_get_crossfade (krad_mixer_portgroup_t *portgroup);
static float get_fade_out (float crossfade_value);
static float get_fade_in (float crossfade_value);
static int krad_mixer_portgroup_is_jack (krad_mixer_portgroup_t *portgroup);
static void crossfade_group_set_crossfade (krad_mixer_crossfade_group_t *crossfade_group, float value);
static void portgroup_set_channel_volume (krad_mixer_portgroup_t *portgroup, int channel, float value);
static void portgroup_set_volume (krad_mixer_portgroup_t *portgroup, float value);
static void portgroup_set_crossfade (krad_mixer_portgroup_t *portgroup, float value);

static float get_fade_out (float crossfade_value) {

  float fade_out;

  fade_out = cos (3.14159f*0.5f*((crossfade_value + 100.0f) + 0.5f)/200.0f);
  fade_out = fade_out * fade_out;
    
  return fade_out;
}

static float get_fade_in (float crossfade_value) {
  return 1.0f - get_fade_out (crossfade_value);
}

static float portgroup_get_crossfade (krad_mixer_portgroup_t *portgroup) {

  if (portgroup->crossfade_group->portgroup[0] == portgroup) {
    return get_fade_out (portgroup->crossfade_group->fade);
  }

  if (portgroup->crossfade_group->portgroup[1] == portgroup) {
    return get_fade_in (portgroup->crossfade_group->fade);
  }
  
  failfast ("failed to get portgroup for crossfade!");

  return 0;
}

static void portgroup_apply_effects (krad_mixer_portgroup_t *portgroup, int nframes) {

  if (portgroup->effects->sample_rate != portgroup->krad_mixer->sample_rate) {
    kr_effects_set_sample_rate (portgroup->effects, portgroup->krad_mixer->sample_rate);
  }
  // FIXME hrm we count on thems being the same btw in them effects lookout
  kr_effects_process (portgroup->effects, portgroup->samples, portgroup->samples, nframes);
}

static void portgroup_apply_volume (krad_mixer_portgroup_t *portgroup, int nframes) {

  int c, s, sign;
  
  sign = 0;

  for (c = 0; c < portgroup->channels; c++) {

    if (portgroup->new_volume_actual[c] == portgroup->volume_actual[c]) {
      for (s = 0; s < nframes; s++) {
        portgroup->samples[c][s] = portgroup->samples[c][s] * portgroup->volume_actual[c];
      }
    } else {
      
      /* The way the volume change is set up here, the volume can only change once per callback, but thats 
         allways plenty of times per second */
      
      /* last_sign: 0 = unset, -1 neg, +1 pos */
        
      if (portgroup->last_sign[c] == 0) {
        if (portgroup->samples[c][0] > 0.0f) {
          portgroup->last_sign[c] = 1;
        } else {
          /* Zero counts as negative here, but its moot */
          portgroup->last_sign[c] = -1;
        }
      }
      
      for (s = 0; s < nframes; s++) {
        if (portgroup->last_sign[c] != 0) {
          if (portgroup->samples[c][s] > 0.0f) {
            sign = 1;
          } else {
            sign = -1;
          }
        
          if ((sign != portgroup->last_sign[c]) || (portgroup->samples[c][s] == 0.0f)) {
            portgroup->volume_actual[c] = portgroup->new_volume_actual[c];
            portgroup->last_sign[c] = 0;
          }
        }
        portgroup->samples[c][s] = (portgroup->samples[c][s] * portgroup->volume_actual[c]);
      }

      if (portgroup->last_sign[c] != 0) {
        portgroup->last_sign[c] = sign;
      }
    }
  }
}
/*
float krad_mixer_portgroup_read_channel_peak (krad_mixer_portgroup_t *portgroup, int channel) {

  float peak;
  
  peak = portgroup->peak[channel];
  portgroup->peak[channel] = 0.0f;

  return peak;
}
*/

static float krad_mixer_portgroup_read_peak_scaled (krad_mixer_portgroup_t *portgroup) {

  //  return krad_mixer_portgroup_read_peak (portgroup) * 100.0f;

  float db;
  float def;

  db = 20.0f * log10f (krad_mixer_portgroup_read_peak (portgroup) * 1.0f);

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

static float krad_mixer_portgroup_read_peak (krad_mixer_portgroup_t *portgroup) {

  //FIXME N channels

  float tmp = portgroup->running_peak[0];
  portgroup->running_peak[0] = 0.0f;

  float tmp2 = portgroup->running_peak[1];
  portgroup->running_peak[1] = 0.0f;
  if (tmp > tmp2) {
    return tmp;
  } else {
    return tmp2;
  }
}

static void krad_mixer_portgroup_compute_channel_peak (krad_mixer_portgroup_t *portgroup, int channel, uint32_t nframes) {

  int s;
  float sample;

  for(s = 0; s < nframes; s++) {
    sample = fabs(portgroup->samples[channel][s]);
    if (sample > portgroup->running_peak[channel]) {
      portgroup->running_peak[channel] = sample;
    }
  }
}

static void krad_mixer_portgroup_compute_peaks (krad_mixer_portgroup_t *portgroup, uint32_t nframes) {

  int c;
  
  for (c = 0; c < portgroup->channels; c++) {
    krad_mixer_portgroup_compute_channel_peak (portgroup, c, nframes);
  }
}

static void krad_mixer_portgroup_compute_meters (krad_mixer_portgroup_t *portgroup, uint32_t nframes) {
  krad_mixer_portgroup_compute_peaks (portgroup, nframes);
}

static void portgroup_clear_samples (krad_mixer_portgroup_t *portgroup, uint32_t nframes) {

  int c;
  int s;

  for (c = 0; c < portgroup->channels; c++) {
    for (s = 0; s < nframes; s++) {
      portgroup->samples[c][s] = 0.0f;
    }
  }
}

static int portgroup_handle_delay (krad_mixer_portgroup_t *portgroup, uint32_t nframes) {

  int delay_frames;

  if (portgroup->delay > portgroup->delay_actual) {
    delay_frames = portgroup->delay - portgroup->delay_actual;
    if (delay_frames > nframes) {
      delay_frames = nframes;
    }
    portgroup_clear_samples (portgroup, delay_frames);
    portgroup->delay += delay_frames;
    return nframes - delay_frames;
  }

  return nframes;
}

static void portgroup_get_samples (krad_mixer_portgroup_t *portgroup, uint32_t nframes) {

  int c;
  float *samples[KRAD_MIXER_MAX_CHANNELS];

  switch ( portgroup->io_type ) {
    case KRAD_TONE:
      krad_tone_run (portgroup->io_ptr, *portgroup->mapped_samples[0], nframes);
      break;
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      krad_audio_portgroup_samples_callback (nframes, portgroup->io_ptr, samples);
      
      for (c = 0; c < KRAD_MIXER_MAX_CHANNELS; c++ ) {
        portgroup->samples[c] = samples[portgroup->map[c]];
      }
      break;
    case KRAD_LINK:
      if (portgroup->direction == INPUT) {
        krad_link_audio_samples_callback (nframes, portgroup->io_ptr, portgroup->mapped_samples[0]);
      }
      break;
    case KLOCALSHM:
      if (portgroup->direction == INPUT) {
        krad_mixer_local_audio_samples_callback (nframes, portgroup->io_ptr, portgroup->mapped_samples[0]);
      }
      break;
  }
}

static void portgroup_update_samples (krad_mixer_portgroup_t *portgroup, uint32_t nframes) {

  if (portgroup->delay != portgroup->delay_actual) {
    nframes = portgroup_handle_delay (portgroup, nframes);
  }
  portgroup_get_samples (portgroup, nframes);
}

static void portgroup_mix_samples (krad_mixer_portgroup_t *dest_portgroup, krad_mixer_portgroup_t *src_portgroup, uint32_t nframes) {

  int c;
  int s;

  if (dest_portgroup->channels == src_portgroup->channels) {
    for (c = 0; c < dest_portgroup->channels; c++) {
      for (s = 0; s < nframes; s++) {
        dest_portgroup->samples[c][s] += src_portgroup->samples[c][s];
      }
    }
  } else {
    //up mix
    for (c = 0; c < dest_portgroup->channels; c++) {
      if (src_portgroup->mixmap[c] != -1) {
        for (s = 0; s < nframes; s++) {
          dest_portgroup->samples[c][s] += src_portgroup->samples[src_portgroup->mixmap[c]][s];
        }
      }
    }
  }
}

static void portgroup_copy_samples (krad_mixer_portgroup_t *dest_portgroup, krad_mixer_portgroup_t *src_portgroup, uint32_t nframes) {

  int c;
  int s;

  // Do mixdown here?

  switch ( dest_portgroup->io_type ) {
    case KRAD_TONE:
      break;
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      for (c = 0; c < dest_portgroup->channels; c++) {
        for (s = 0; s < nframes; s++) {
          dest_portgroup->samples[c][s] = src_portgroup->samples[c][s];
        }
      }
      break;
    case KRAD_LINK:
      if (dest_portgroup->direction == OUTPUT) {
        krad_link_audio_samples_callback (nframes, dest_portgroup->io_ptr, src_portgroup->samples);
      }
      break;
    case KLOCALSHM:
      if (dest_portgroup->direction == OUTPUT) {
        krad_mixer_local_audio_samples_callback (nframes, dest_portgroup->io_ptr, src_portgroup->samples);
      }
      break;
  }
}

static void portgroup_limit (krad_mixer_portgroup_t *portgroup, uint32_t nframes) {

  int c;

  for (c = 0; c < portgroup->channels; c++) {
    krad_hardlimit (portgroup->samples[c], nframes);
  }
}

static void krad_mixer_update_portgroups (krad_mixer_t *krad_mixer) {

  int p;
  krad_mixer_portgroup_t *portgroup;
  
  p = 0;
  portgroup = NULL;
    
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    portgroup = krad_mixer->portgroup[p];
    if ((portgroup != NULL) && (portgroup->active == 3)) {
      portgroup->active = 4;
    }
    if ((portgroup != NULL) && (portgroup->active == 1)) {
      portgroup->active = 2;
    }
  }
}

static int krad_mixer_local_audio_samples_callback (int nframes,
                                                     krad_mixer_local_portgroup_t *portgroup,
                                                     float **samples) {
  int ret;
  int wrote;
  char buf[1];
  struct pollfd pollfds[1];

  ret = 0;
  wrote = 0;
  buf[0] = 0;
  
  if (portgroup->direction == OUTPUT) {
    memcpy (portgroup->local_buffer,
            samples[0],
            2 * 4 * portgroup->krad_mixer->period_size);
  }
  
  pollfds[0].events = POLLOUT;
  pollfds[0].fd = portgroup->msg_sd;

  ret = poll (pollfds, 1, 0);
  if (ret < 0) {
    printke ("krad mixer poll failure %d", ret);
    return -2;
  }
  if (ret == 0) {
    printke ("krad mixer : local_audio port poll write timeout", ret);
    return -1;
  }
  if (ret == 1) {
    if (pollfds[0].revents & POLLHUP) {
      printke ("krad mixer: local_audio port poll hangup", ret);
      return -2;
    }
    if (pollfds[0].revents & POLLERR) {
      printke ("krad mixer: local_audio port poll error", ret);
      return -2;
    }  
    if (pollfds[0].revents & POLLOUT) {
      wrote = write (portgroup->msg_sd, buf, 1);
      if (wrote == 1) {
        portgroup->last_wrote++;
      }
    }
  }
  
  if (portgroup->last_wrote > 0) {
    pollfds[0].events = POLLIN;
    pollfds[0].fd = portgroup->msg_sd;
    ret = poll (pollfds, 1, -1);
    if (ret < 0) {
      printke ("krad mixer poll failure %d", ret);
      return -2;
    }
    if (ret == 0) {
      printke ("krad mixer : local_audio port poll read timeout", ret);
      //return -1;
    }
    if (ret == 1) {
      if (pollfds[0].revents & POLLHUP) {
        printke ("krad mixer: local_audio port poll hangup", ret);
        return -2;
      }
      if (pollfds[0].revents & POLLERR) {
        printke ("krad mixer: videoport poll error", ret);
        return -2;
      }  
      if (pollfds[0].revents & POLLIN) {
        ret = read (portgroup->msg_sd, buf, 1);
        if (ret == 1) {
          portgroup->last_wrote--;
          //return 0;
        }
      }
    }
  }
  
  return -1;
}

static void portgroup_update_volume (krad_mixer_portgroup_t *portgroup) {

  int c;
  
  for (c = 0; c < portgroup->channels; c++) {
    portgroup_set_channel_volume (portgroup, c, portgroup->volume[c]);
  }  
}

static void krad_mixer_ticker_thread_cleanup (void *arg) {

  krad_mixer_t *krad_mixer = (krad_mixer_t *)arg;
  
  if (krad_mixer->krad_ticker != NULL) {
    krad_ticker_destroy (krad_mixer->krad_ticker);
    krad_mixer->krad_ticker = NULL;
    printk ("Krad Mixer: Synthetic Timer Destroyed");
  }
}

static void *krad_mixer_ticker_thread (void *arg) {

  krad_mixer_t *krad_mixer = (krad_mixer_t *)arg;

  krad_system_set_thread_name ("kr_mixer");
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL);
  krad_mixer->krad_ticker = krad_ticker_create (krad_mixer->sample_rate, krad_mixer->period_size);
  pthread_cleanup_push (krad_mixer_ticker_thread_cleanup, krad_mixer);
  pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
  krad_ticker_start_at (krad_mixer->krad_ticker, krad_mixer->start_time);
  while (krad_mixer->ticker_running == 1) {
    pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL);
    krad_mixer_process (krad_mixer->period_size, krad_mixer);
    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
    krad_ticker_wait (krad_mixer->krad_ticker);
  }

  pthread_cleanup_pop (1);

  return NULL;
}

static void krad_mixer_stop_ticker (krad_mixer_t *krad_mixer) {

  if (krad_mixer->ticker_running == 1) {
    krad_mixer->ticker_running = 2;
    pthread_cancel (krad_mixer->ticker_thread);
    pthread_join (krad_mixer->ticker_thread, NULL);
    krad_mixer->ticker_running = 0;
  }
}

static int krad_mixer_portgroup_is_jack (krad_mixer_portgroup_t *portgroup) {

  krad_audio_portgroup_t *ka_portgroup;

  if (portgroup != NULL) {
    if (portgroup->io_type == KRAD_AUDIO) {
      ka_portgroup = portgroup->io_ptr;
      if (ka_portgroup->audio_api == JACK) {
        return 1;
      }
    }
  }
  
  return 0;
}

static void krad_mixer_local_portgroup_destroy (krad_mixer_local_portgroup_t *krad_mixer_local_portgroup) {
  close (krad_mixer_local_portgroup->msg_sd);
  close (krad_mixer_local_portgroup->shm_sd);
  munmap (krad_mixer_local_portgroup->local_buffer, krad_mixer_local_portgroup->local_buffer_size);
  free (krad_mixer_local_portgroup);
}

static void krad_mixer_portgroup_mark_destroy (krad_mixer_t *krad_mixer, krad_mixer_portgroup_t *portgroup) {
  if (krad_mixer->destroying == 2) {
    portgroup->active = 4;
  } else {
    if (portgroup->destroy_mark != 1) {
      portgroup->destroy_mark = 1;
      portgroup->active = 3;
    }
  }
}

static void portgroup_set_crossfade (krad_mixer_portgroup_t *portgroup, float value) {
  if (portgroup->crossfade_group != NULL) {
    crossfade_group_set_crossfade (portgroup->crossfade_group, value);
  }
}

static void crossfade_group_set_crossfade (krad_mixer_crossfade_group_t *crossfade_group, float value) {

  if (crossfade_group != NULL) {
    crossfade_group->fade = value;  

    if ((crossfade_group->portgroup[0] != NULL) && (crossfade_group->portgroup[1] != NULL)) {
      portgroup_update_volume (crossfade_group->portgroup[0]);
      portgroup_update_volume (crossfade_group->portgroup[1]);  
    }
  }
}

static void portgroup_set_volume (krad_mixer_portgroup_t *portgroup, float value) {

  int c;
  float volume_temp;

  volume_temp = (value/100.0f);
  volume_temp *= volume_temp;

  if (portgroup->direction == INPUT) {
    volume_temp = volume_temp * portgroup->mixbus->volume_actual[0];

    if (portgroup->crossfade_group != NULL) {
      volume_temp = volume_temp * portgroup_get_crossfade (portgroup);
    }
  }

  for (c = 0; c < portgroup->channels; c++) {
    portgroup->volume[c] = value;
    if (portgroup->direction == MIX) {
      portgroup->volume_actual[c] = volume_temp;
    } else {
      portgroup->new_volume_actual[c] = volume_temp;
    }
  }

  if (portgroup->direction == MIX) {
  
    int p;
    krad_mixer_portgroup_t *pg;

    for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
      pg = portgroup->krad_mixer->portgroup[p];
      if ((pg != NULL) && (pg->active == 2) && (pg->direction == INPUT)) {
        if (pg->mixbus == portgroup) {
          portgroup_update_volume (pg);
        }
      }
    }
  }
}

static void portgroup_set_channel_volume (krad_mixer_portgroup_t *portgroup, int channel, float value) {

  float volume_temp;

  if (portgroup->direction == INPUT) {

    portgroup->volume[channel] = value;
    volume_temp = (portgroup->volume[channel]/100.0f);
    volume_temp *= volume_temp;
    volume_temp = volume_temp * portgroup->mixbus->volume_actual[0];
    if (portgroup->crossfade_group != NULL) {
      volume_temp = volume_temp * portgroup_get_crossfade (portgroup);
    }

    portgroup->new_volume_actual[channel] = volume_temp;
  }
}

static void krad_mixer_portgroup_update_meter_readings (krad_mixer_portgroup_t *portgroup) {
  portgroup->peak[0] = krad_mixer_portgroup_read_peak_scaled (portgroup);
  if (portgroup->peak[0] != portgroup->last_peak[0]) {
    portgroup->peak[1] = portgroup->peak[0];
    portgroup->last_peak[0] = portgroup->peak[0];
    portgroup->last_peak[1] = portgroup->peak[0];
    krad_radio_broadcast_subunit_control (portgroup->krad_mixer->broadcaster, &portgroup->address, KR_PEAK,
                                          portgroup->peak[0], NULL);
  }
}

static int krad_mixer_process (uint32_t nframes, krad_mixer_t *krad_mixer) {
  
  int p, m;
  void *client;
  
  client = NULL;
  krad_mixer_portgroup_t *portgroup = NULL;
  krad_mixer_portgroup_t *mixbus = NULL;
  krad_mixer_crossfade_group_t *crossfade_group = NULL;

  krad_mixer_update_portgroups (krad_mixer);

  if (krad_mixer->push_tone != NULL) {
    krad_tone_add_preset (krad_mixer->tone_port->io_ptr, krad_mixer->push_tone);
    krad_mixer->push_tone = NULL;
  }
  
  // Gets input/output port buffers
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    portgroup = krad_mixer->portgroup[p];
    if ((portgroup != NULL) && (portgroup->active == 2) && ((portgroup->direction == INPUT) || (portgroup->direction == OUTPUT))) {
      portgroup_update_samples (portgroup, nframes);
    }
  }
  
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS / 2; p++) {
    crossfade_group = &krad_mixer->crossfade_group[p];
    if ((crossfade_group != NULL) && ((crossfade_group->portgroup[0] != NULL) && (crossfade_group->portgroup[1] != NULL))) {
      if (crossfade_group->fade_easing.active) {
        portgroup_set_crossfade (crossfade_group->portgroup[0], krad_easing_process (&crossfade_group->fade_easing, crossfade_group->fade, &client));
        krad_radio_broadcast_subunit_control ( krad_mixer->broadcaster, &crossfade_group->portgroup[0]->address, KR_CROSSFADE,
                                               crossfade_group->fade, client );
      }
    }
  }

  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    portgroup = krad_mixer->portgroup[p];
    if ((portgroup != NULL) && (portgroup->active == 2) && (portgroup->volume_easing.active)) {
      portgroup_set_volume (portgroup, krad_easing_process (&portgroup->volume_easing, portgroup->volume[0], &client));
      krad_radio_broadcast_subunit_control ( krad_mixer->broadcaster, &portgroup->address, KR_VOLUME, 
                                             portgroup->volume[0], client );
    }
  }

  // apply volume, effects and calc peaks on inputs
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    portgroup = krad_mixer->portgroup[p];
    if ((portgroup != NULL) && (portgroup->active == 2) && (portgroup->direction == INPUT)) {
      portgroup_apply_volume (portgroup, nframes);
      //experiment
      if (portgroup->volume_actual[0] != 0.0f) {
        portgroup_apply_effects (portgroup, nframes);
      }
      krad_mixer_portgroup_compute_meters (portgroup, nframes);
    }
  }
  
  // Clear Mixes  
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    portgroup = krad_mixer->portgroup[p];
    if ((portgroup != NULL) && (portgroup->active == 2) && (portgroup->io_type == MIXBUS)) {
      portgroup_clear_samples (portgroup, nframes);
    }
  }

  // Mix
  for (m = 0; m < KRAD_MIXER_MAX_PORTGROUPS; m++) {
    mixbus = krad_mixer->portgroup[m];
    if ((mixbus != NULL) && (mixbus->active) && (mixbus->io_type == MIXBUS)) {
      for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
        portgroup = krad_mixer->portgroup[p];
        if ((portgroup != NULL) && (portgroup->active == 2) && (portgroup->mixbus == mixbus) && (portgroup->direction == INPUT)) {
          portgroup_mix_samples ( mixbus, portgroup, nframes );
        }
      }
    }
  }

  // copy to outputs, limit all outputs
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    portgroup = krad_mixer->portgroup[p];
    if ((portgroup != NULL) && (portgroup->active == 2) && (portgroup->direction == OUTPUT)) {
      portgroup_copy_samples ( portgroup, portgroup->mixbus, nframes );
      if (portgroup->output_type == AUX) {
        portgroup_apply_volume (portgroup, nframes);
      }
      portgroup_limit ( portgroup, nframes );
    }
  }
  
  if (krad_mixer->master_mix != NULL) {
    krad_mixer_portgroup_compute_meters (krad_mixer->master_mix, nframes);
  }
  
  krad_mixer->frames_since_peak_read += nframes;

  if (krad_mixer->frames_since_peak_read >= krad_mixer->frames_per_peak_broadcast) {
    krad_mixer->frames_since_peak_read = 0;
    for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
      portgroup = krad_mixer->portgroup[p];
      if ((portgroup != NULL) && (portgroup->active == 2) && (portgroup->direction == INPUT)) {
        krad_mixer_portgroup_update_meter_readings (portgroup);
      }
    }
    if (krad_mixer->master_mix != NULL) {
      krad_mixer_portgroup_update_meter_readings (krad_mixer->master_mix);
    }
  }
  return 0;
}

void krad_mixer_crossfade_group_create (krad_mixer_t *krad_mixer, krad_mixer_portgroup_t *portgroup1, krad_mixer_portgroup_t *portgroup2) {

  int x;
  krad_mixer_crossfade_group_t *crossfade_group;

  if ((portgroup1 == NULL) || ((portgroup1->active != 1) && (portgroup1->active != 2)) ||
      (portgroup2 == NULL) || ((portgroup2->active != 1) && (portgroup2->active != 2))) {
    printke ("Invalid portgroup for crossfade!");
    return;
  }

  if (!(((portgroup1->direction == INPUT) || (portgroup1->direction == MIX)) &&
      ((portgroup1->direction == INPUT) || (portgroup2->direction == MIX)) &&
      (((portgroup1->io_type == MIXBUS) && (portgroup2->io_type == MIXBUS)) ||
       ((portgroup1->io_type != MIXBUS) && (portgroup2->io_type != MIXBUS))))) {
    printke ("Invalid crossfade group!");
    return;
  }
    
  if (portgroup1->crossfade_group != NULL) {
    krad_mixer_crossfade_group_destroy (krad_mixer, portgroup1->crossfade_group);
  }

  if (portgroup2->crossfade_group != NULL) {
    krad_mixer_crossfade_group_destroy (krad_mixer, portgroup2->crossfade_group);
  }

  for (x = 0; x < KRAD_MIXER_MAX_PORTGROUPS / 2; x++) {
    crossfade_group = &krad_mixer->crossfade_group[x];
    if ((crossfade_group != NULL) && ((crossfade_group->portgroup[0] == NULL) && (crossfade_group->portgroup[1] == NULL))) {
      break;
    }
  }

  crossfade_group->portgroup[0] = portgroup1;
  crossfade_group->portgroup[1] = portgroup2;
  
  portgroup1->crossfade_group = crossfade_group;
  portgroup2->crossfade_group = crossfade_group;

  krad_radio_broadcast_subunit_update ( krad_mixer->app->app_broadcaster, &portgroup1->address, KR_CROSSFADE_GROUP, 
                                        KR_STRING, portgroup2->sysname, NULL );

  krad_mixer_set_portgroup_control (krad_mixer, portgroup1->sysname, "crossfade", -100.0f, 0, NULL );
}

void krad_mixer_crossfade_group_destroy (krad_mixer_t *krad_mixer, krad_mixer_crossfade_group_t *crossfade_group) {

  krad_mixer_portgroup_t *portgroup[2];
  
  portgroup[0] = NULL;
  portgroup[1] = NULL;
    
  if ((crossfade_group != NULL) &&
      (crossfade_group->portgroup[0] != NULL) &&
      (crossfade_group->portgroup[1] != NULL)) {

    portgroup[0] = crossfade_group->portgroup[0];
    portgroup[1] = crossfade_group->portgroup[1];
    crossfade_group->portgroup[0]->crossfade_group = NULL;
    crossfade_group->portgroup[1]->crossfade_group = NULL;

    crossfade_group->portgroup[0] = NULL;
    crossfade_group->portgroup[1] = NULL;
    crossfade_group->fade = -100.0f;

    /* Hrm */
    krad_mixer_set_portgroup_control (krad_mixer, portgroup[0]->sysname, "volume", portgroup[0]->volume[0], 0, NULL );
    krad_mixer_set_portgroup_control (krad_mixer, portgroup[1]->sysname, "volume", portgroup[1]->volume[0], 0, NULL );

    krad_radio_broadcast_subunit_update ( krad_mixer->app->app_broadcaster, &portgroup[0]->address, KR_CROSSFADE_GROUP, 
                                          KR_STRING, "", NULL );
  
  }
}

krad_mixer_portgroup_t *krad_mixer_local_portgroup_create (krad_mixer_t *krad_mixer, char *sysname,
                               int direction, int shm_sd, int msg_sd) {

  krad_mixer_portgroup_t *krad_mixer_portgroup;
  krad_mixer_local_portgroup_t *krad_mixer_local_portgroup;

  krad_mixer_output_t output_type;

  krad_mixer_local_portgroup = calloc(1, sizeof(krad_mixer_local_portgroup_t));

  krad_mixer_local_portgroup->local_buffer_size = 960 * 540 * 4 * 2;
  
  krad_mixer_local_portgroup->shm_sd = shm_sd;
  krad_mixer_local_portgroup->msg_sd = msg_sd;
  
  krad_mixer_local_portgroup->local_buffer = mmap (NULL, krad_mixer_local_portgroup->local_buffer_size,
                         PROT_READ | PROT_WRITE, MAP_SHARED,
                         krad_mixer_local_portgroup->shm_sd, 0);

  //krad_system_set_socket_nonblocking (krad_mixer_local_portgroup->msg_sd);

  krad_mixer_local_portgroup->krad_mixer = krad_mixer;
  krad_mixer_local_portgroup->direction = direction;
  
  if (krad_mixer_local_portgroup->direction == INPUT) {
    output_type = NOTOUTPUT;
  } else {
    output_type = DIRECT;
  }

  krad_mixer_portgroup = krad_mixer_portgroup_create (krad_mixer, sysname, krad_mixer_local_portgroup->direction,
                          output_type, 2, 100.0f, krad_mixer->master_mix, KLOCALSHM, krad_mixer_local_portgroup, NOAUDIO);

  krad_mixer_portgroup->samples[0] = (float *)krad_mixer_local_portgroup->local_buffer;
  krad_mixer_portgroup->samples[1] = (float *)krad_mixer_local_portgroup->local_buffer + (1 * krad_mixer->period_size);

  krad_mixer_portgroup->active = 1;

  return krad_mixer_portgroup;

}


static krad_audio_portgroup_direction_t kr_mixer_dir_to_kr_audio_dir (krad_mixer_portgroup_direction_t dir) {

  if (dir == INPUT) {
    return KINPUT;
  }  
  return KOUTPUT;
}

krad_mixer_portgroup_t *krad_mixer_portgroup_create (krad_mixer_t *krad_mixer, char *sysname, int direction,
                           krad_mixer_output_t output_type, int channels, float volume,
                           krad_mixer_mixbus_t *mixbus, krad_mixer_portgroup_io_t io_type, 
                           void *io_ptr, krad_audio_api_t api) {

  int p;
  int c;
  krad_mixer_portgroup_t *portgroup;
  
  portgroup = NULL;

  /* prevent dupe names */
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if (krad_mixer->portgroup[p]->active != 0) {
      if (strncmp(sysname, krad_mixer->portgroup[p]->sysname, strlen(sysname)) == 0) {
        return NULL;
      }
    }
  }
  
  //FIXME race here if portgroup being created via app and transponder at same moment
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if (krad_mixer->portgroup[p]->active == 0) {
      portgroup = krad_mixer->portgroup[p];
      break;
    }
  }
  
  if (portgroup == NULL) {
    return NULL;
  }
  
  /* Prevent multiple JACK direct outputs as this is redundant */
  if ((api == JACK) && (direction == OUTPUT) && (output_type == DIRECT)) {
    for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
      if ((krad_mixer->portgroup[p]->active == 1) || (krad_mixer->portgroup[p]->active == 2)) {
        if ((krad_mixer_portgroup_is_jack(krad_mixer->portgroup[p])) &&
            (krad_mixer->portgroup[p]->direction == OUTPUT) &&
            (krad_mixer->portgroup[p]->output_type == DIRECT)) {
          return NULL;
        }
      }
    }
  }

  portgroup->krad_mixer = krad_mixer;

  strcpy (portgroup->sysname, sysname);
  portgroup->channels = channels;
  portgroup->io_type = io_type;
  portgroup->output_type = output_type;
  portgroup->mixbus = mixbus;
  portgroup->direction = direction;

  portgroup->address.path.unit = KR_MIXER;
  portgroup->address.path.subunit.mixer_subunit = KR_PORTGROUP;
  strcpy (portgroup->address.id.name, portgroup->sysname);

  for (c = 0; c < KRAD_MIXER_MAX_CHANNELS; c++) {

    if (c < portgroup->channels) {
      portgroup->mixmap[c] = c;
    } else {
      portgroup->mixmap[c] = -1;
    }
    portgroup->map[c] = c;
    portgroup->mapped_samples[c] = &portgroup->samples[c];
    if ((portgroup->direction != OUTPUT) || (portgroup->output_type == AUX)) {
      portgroup->volume[c] = volume;
    } else {
      portgroup->volume[c] = 100.0f;
    }
    portgroup->volume_actual[c] = (float)(portgroup->volume[c]/100.0f);
    portgroup->volume_actual[c] *= portgroup->volume_actual[c];
    portgroup->new_volume_actual[c] = portgroup->volume_actual[c];

    switch ( portgroup->io_type ) {
      case KRAD_TONE:
        portgroup->samples[c] = calloc (1, 16384);
        break;    
      case MIXBUS:
        portgroup->samples[c] = calloc (1, 16384);
        break;
      case KRAD_AUDIO:
        break;
      case KRAD_LINK:
        portgroup->samples[c] = calloc (1, 16384);
        break;
      case KLOCALSHM:
        break;
    }
  }
  
  switch ( portgroup->io_type ) {
    case KRAD_TONE:
      portgroup->io_ptr = krad_tone_create (krad_mixer->sample_rate);
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      portgroup->io_ptr = krad_audio_portgroup_create (krad_mixer->krad_audio, portgroup->sysname, 
                               kr_mixer_dir_to_kr_audio_dir(portgroup->direction), portgroup->channels, api);
      break;
    case KRAD_LINK:
      portgroup->io_ptr = io_ptr;
      break;
    case KLOCALSHM:
      portgroup->io_ptr = io_ptr;    
      break;      
  }
  
  if (portgroup->io_type != KRAD_LINK) {
    portgroup->krad_tags = krad_tags_create (portgroup->sysname);
    //if ((portgroup->krad_tags != NULL) && (krad_mixer->app != NULL)) {
    //  krad_tags_set_set_tag_callback (portgroup->krad_tags, krad_mixer->app, 
    //                  (void (*)(void *, char *, char *, char *, int))krad_app_server_broadcast_tag);
    //}
  } else {
    portgroup->krad_tags = krad_link_get_tags (portgroup->io_ptr);
  }

  if (portgroup->krad_tags == NULL) {
    failfast ("Oh I couldn't find me tags");
  }

  portgroup->effects = kr_effects_create (portgroup->channels, portgroup->krad_mixer->sample_rate);
  
  if (portgroup->effects == NULL) {
    failfast ("Oh I couldn't make effects");
  }

  if (portgroup->direction == INPUT) {
    kr_effects_effect_add2 (portgroup->effects, kr_effects_string_to_effect ("eq"),
                            portgroup->krad_mixer, portgroup->sysname);
    kr_effects_effect_add2 (portgroup->effects, kr_effects_string_to_effect ("lowpass"),
                            portgroup->krad_mixer, portgroup->sysname);
    kr_effects_effect_add2 (portgroup->effects, kr_effects_string_to_effect ("highpass"),
                            portgroup->krad_mixer, portgroup->sysname);
    kr_effects_effect_add2 (portgroup->effects, kr_effects_string_to_effect ("analog"),
                            portgroup->krad_mixer, portgroup->sysname);
  }
    
  if (portgroup->io_type != KLOCALSHM) {
    portgroup->active = 1;
  }

  return portgroup;
}

void krad_mixer_portgroup_destroy (krad_mixer_t *krad_mixer, krad_mixer_portgroup_t *portgroup) {

  int c;

  if (portgroup == NULL) {
    return;
  }
  
  if (portgroup->crossfade_group != NULL) {
    krad_mixer_crossfade_group_destroy (krad_mixer, portgroup->crossfade_group);
  }
  
  krad_mixer_portgroup_mark_destroy (krad_mixer, portgroup);

  while (portgroup->active != 4) {
    usleep (1000);
  }

  portgroup->delay = 0;
  portgroup->delay_actual = 0;

  printkd("Krad Mixer: Removing %d channel Portgroup %s", portgroup->channels, portgroup->sysname);

  for (c = 0; c < KRAD_MIXER_MAX_CHANNELS; c++) {
    switch ( portgroup->io_type ) {
      case KRAD_TONE:
        free ( portgroup->samples[c] );      
        break;
      case MIXBUS:
        free ( portgroup->samples[c] );
        break;
      case KRAD_AUDIO:
        break;
      case KRAD_LINK:
        free ( portgroup->samples[c] );      
        break;
      case KLOCALSHM:
        break;
    }
  }

  switch ( portgroup->io_type ) {
    case KRAD_TONE:
      krad_tone_destroy (portgroup->io_ptr);
    case MIXBUS:
      break;
    case KRAD_AUDIO:
      krad_audio_portgroup_destroy (portgroup->io_ptr);
      break;
    case KRAD_LINK:
      break;
    case KLOCALSHM:
      krad_mixer_local_portgroup_destroy (portgroup->io_ptr);
      break;
  }
  
  if (portgroup->krad_xmms != NULL) {
    krad_xmms_destroy (portgroup->krad_xmms);
    portgroup->krad_xmms = NULL;
  }
  
  if (portgroup->io_type != KRAD_LINK) {
    krad_tags_destroy (portgroup->krad_tags);  
  }  
  
  if (portgroup->effects != NULL) {
    kr_effects_destroy (portgroup->effects);
    portgroup->effects = NULL;
  }

  portgroup->destroy_mark = 0;
  portgroup->active = 0;
}

krad_mixer_portgroup_t *krad_mixer_get_portgroup_from_sysname (krad_mixer_t *krad_mixer, char *sysname) {

  int p;
  int len;
  krad_mixer_portgroup_t *portgroup;

  len = strlen (sysname);
  
  if (len > 0) {
    for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
      portgroup = krad_mixer->portgroup[p];
      if ((portgroup != NULL) && ((portgroup->active == 1) || (portgroup->active == 2))) {
        if ((strlen(portgroup->sysname) == len) && (strncmp(sysname, portgroup->sysname, len) == 0)) {
          return portgroup;
        }
      }
    }
  }

  printke ("Krad Mixer: Could not find portgroup called %s", sysname);

  return NULL;
}

int krad_mixer_set_portgroup_control (krad_mixer_t *krad_mixer, char *sysname,
                                      char *control, float value, int duration,
                                      void *ptr) {

  krad_mixer_portgroup_t *portgroup;

  portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, sysname);
  
  if (portgroup != NULL) {
      
    if ((portgroup->direction == OUTPUT) && (portgroup->output_type == DIRECT)) {
      return 1;
    }
      
    if ((strncmp(control, "volume", 6) == 0) && (strlen(control) == 6)) {
      /* Note: This will set volume for Bus and Auxouts as well as normal inputs */
      //portgroup_set_volume (portgroup, value);
      krad_easing_set_new_value (&portgroup->volume_easing, value, duration, EASEINOUTSINE, ptr);
      return 1;
    }

    if ((portgroup->direction == INPUT) && (portgroup->io_type != MIXBUS)) {

      if ((strncmp(control, "crossfade", 9) == 0) && (strlen(control) == 9)) {
        if (portgroup->crossfade_group != NULL) {
          //portgroup_set_crossfade (portgroup, value);
          krad_easing_set_new_value (&portgroup->crossfade_group->fade_easing, value, duration, EASEINOUTSINE, ptr);
        }
        return 1;
      }        

      if (strncmp(control, "volume_left", 11) == 0) {
        portgroup_set_channel_volume (portgroup, 0, value);
        return 1;  
      }

      if (strncmp(control, "volume_right", 12) == 0) {
        portgroup_set_channel_volume (portgroup, 1, value);
        return 1;
      }
    }
  }
  return 0;
}

void krad_mixer_portgroup_map_channel (krad_mixer_portgroup_t *portgroup, int in_channel, int out_channel) {
  portgroup->map[in_channel] = out_channel;
  portgroup->mapped_samples[in_channel] = &portgroup->samples[out_channel];
}

void krad_mixer_portgroup_mixmap_channel (krad_mixer_portgroup_t *portgroup, int in_channel, int out_channel) {
  portgroup->mixmap[out_channel] = in_channel;
}

void krad_mixer_portgroup_xmms2_cmd (krad_mixer_t *krad_mixer, char *portgroupname, char *xmms2_cmd) {

  krad_mixer_portgroup_t *portgroup;

  portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, portgroupname);

  if ((portgroup != NULL) && (portgroup->krad_xmms != NULL)) {
    if (strncmp(xmms2_cmd, "play", 4) == 0) {
      krad_xmms_playback_cmd (portgroup->krad_xmms, PLAY);
      return;
    }
    if (strncmp(xmms2_cmd, "pause", 5) == 0) {
      krad_xmms_playback_cmd (portgroup->krad_xmms, PAUSE);
      return;
    }
    if (strncmp(xmms2_cmd, "stop", 4) == 0) {
      krad_xmms_playback_cmd (portgroup->krad_xmms, STOP);
      return;
    }
    if (strncmp(xmms2_cmd, "next", 4) == 0) {
      krad_xmms_playback_cmd (portgroup->krad_xmms, NEXT);
      return;
    }
    if (strncmp(xmms2_cmd, "prev", 4) == 0) {
      krad_xmms_playback_cmd (portgroup->krad_xmms, PREV);
      return;
    }
  }
}

void krad_mixer_portgroup_bind_xmms2 (krad_mixer_t *krad_mixer, char *portgroupname, char *app_path) {

  krad_mixer_portgroup_t *portgroup;

  portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, portgroupname);

  if (portgroup != NULL) {
    krad_mixer_portgroup_unbind_xmms2 (krad_mixer, portgroupname);
    portgroup->krad_xmms = krad_xmms_create (krad_mixer->name, app_path, portgroup->krad_tags);
    krad_radio_broadcast_subunit_update ( krad_mixer->app->app_broadcaster, &portgroup->address, KR_XMMS2_IPC_PATH, 
                                          KR_STRING, app_path, NULL );
  }
}

void krad_mixer_portgroup_unbind_xmms2 (krad_mixer_t *krad_mixer, char *portgroupname) {

  krad_mixer_portgroup_t *portgroup;

  portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, portgroupname);

  if ((portgroup != NULL) && (portgroup->krad_xmms != NULL)) {
    krad_xmms_destroy (portgroup->krad_xmms);
    portgroup->krad_xmms = NULL;
    krad_radio_broadcast_subunit_update ( krad_mixer->app->app_broadcaster, &portgroup->address, KR_XMMS2_IPC_PATH, 
                                          KR_STRING, "", NULL );
  }
}

void krad_mixer_unplug_portgroup (krad_mixer_t *krad_mixer, char *name, char *remote_name) {

  krad_mixer_portgroup_t *portgroup;
  krad_audio_portgroup_t *ka_portgroup;
  krad_jack_portgroup_t *krad_jack_portgroup;

  portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, name);

  if (portgroup != NULL) {
    if (portgroup->io_type == KRAD_AUDIO) {
      ka_portgroup = portgroup->io_ptr;
      if (ka_portgroup->audio_api == JACK) {
        krad_jack_portgroup = ka_portgroup->api_portgroup;
        krad_jack_portgroup_unplug (krad_jack_portgroup, remote_name);
      }
    }
  }
}

void krad_mixer_plug_portgroup (krad_mixer_t *krad_mixer, char *name, char *remote_name) {

  krad_mixer_portgroup_t *portgroup;
  krad_audio_portgroup_t *ka_portgroup;
  krad_jack_portgroup_t *krad_jack_portgroup;

  portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, name);

  //printk ("mixer want to plug %s to %s", name, remote_name);

  if (portgroup != NULL) {
    if (portgroup->io_type == KRAD_AUDIO) {
      ka_portgroup = portgroup->io_ptr;
      if (ka_portgroup->audio_api == JACK) {
        krad_jack_portgroup = ka_portgroup->api_portgroup;
        krad_jack_portgroup_plug (krad_jack_portgroup, remote_name);
      }
    }
  }
}

void krad_mixer_start_ticker (krad_mixer_t *krad_mixer) {

  if (krad_mixer->ticker_running == 1) {
    krad_mixer_stop_ticker (krad_mixer);
  }
  if (krad_mixer->destroying == 0) {  
    clock_gettime (CLOCK_MONOTONIC, &krad_mixer->start_time);
    krad_mixer->ticker_running = 1;
    pthread_create (&krad_mixer->ticker_thread, NULL, krad_mixer_ticker_thread, (void *)krad_mixer);
  }
}

void krad_mixer_start_ticker_at (krad_mixer_t *krad_mixer, struct timespec start_time) {

  if (krad_mixer->ticker_running == 1) {
    krad_mixer_stop_ticker (krad_mixer);
  }
  if (krad_mixer->destroying == 0) {
    memcpy (&krad_mixer->start_time, &start_time, sizeof(struct timespec));
    krad_mixer->ticker_running = 1;
    pthread_create (&krad_mixer->ticker_thread, NULL, krad_mixer_ticker_thread, (void *)krad_mixer);
  }
}

void krad_mixer_unset_pusher (krad_mixer_t *krad_mixer) {
  if (krad_mixer->ticker_running == 1) {
    krad_mixer_stop_ticker (krad_mixer);
  }
  
  if (krad_mixer_get_period_size(krad_mixer) != KRAD_MIXER_DEFAULT_PERIOD_SIZE) {
    krad_mixer_set_period_size (krad_mixer, KRAD_MIXER_DEFAULT_PERIOD_SIZE);
  }
  
  krad_mixer_start_ticker (krad_mixer);
  krad_mixer->pusher = 0;
}

void krad_mixer_set_pusher (krad_mixer_t *krad_mixer, krad_audio_api_t pusher) {
  if (krad_mixer->ticker_running == 1) {
    krad_mixer_stop_ticker (krad_mixer);
  }  
  krad_mixer->pusher = pusher;
}

int krad_mixer_has_pusher (krad_mixer_t *krad_mixer) {
  if (krad_mixer->pusher == 0) {
    return 0;
  } else {
    return 1;
  }
}

krad_audio_api_t krad_mixer_get_pusher (krad_mixer_t *krad_mixer) {
  return krad_mixer->pusher;
}

uint32_t krad_mixer_get_period_size (krad_mixer_t *krad_mixer) {
  return krad_mixer->period_size;
}

void krad_mixer_set_period_size (krad_mixer_t *krad_mixer, uint32_t period_size) {
  krad_mixer->period_size = period_size;
}

uint32_t krad_mixer_get_sample_rate (krad_mixer_t *krad_mixer) {
  return krad_mixer->sample_rate;
}

void krad_mixer_set_sample_rate (krad_mixer_t *krad_mixer, uint32_t sample_rate) {
  krad_mixer->sample_rate = sample_rate;
  krad_tone_set_sample_rate (krad_mixer->tone_port->io_ptr,
                             krad_mixer->sample_rate);
  
  if (krad_mixer->ticker_running == 1) {
    krad_mixer_stop_ticker (krad_mixer);
    krad_mixer_start_ticker (krad_mixer);
  }
}

int krad_mixer_mix (uint32_t nframes, krad_mixer_t *krad_mixer) {
  return krad_mixer_process (nframes, krad_mixer);
}

void krad_mixer_set_app (krad_mixer_t *krad_mixer, krad_app_server_t *krad_app) {
  krad_mixer->app = krad_app;
  krad_mixer->broadcaster =
    krad_app_server_broadcaster_register ( krad_mixer->app );
}

void krad_mixer_destroy (krad_mixer_t *krad_mixer) {

  int p;
  
  printk ("Krad Mixer shutdown started");
  
  krad_mixer->destroying = 1;
  
  if (krad_mixer->pusher != JACK) {
    krad_mixer_stop_ticker (krad_mixer);
    krad_mixer->destroying = 2;
  }
  
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if ((krad_mixer->portgroup[p]->active == 2) &&
        (krad_mixer->portgroup[p]->io_type != MIXBUS)) {
      krad_mixer_portgroup_mark_destroy (krad_mixer, krad_mixer->portgroup[p]);
    }
  }
  
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if ((krad_mixer->portgroup[p]->active != 0) &&
        (krad_mixer->portgroup[p]->io_type != MIXBUS)) {
      krad_mixer_portgroup_destroy (krad_mixer, krad_mixer->portgroup[p]);
    }
  }
  
  krad_mixer->destroying = 2;
  
  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    if (krad_mixer->portgroup[p]->active != 0) {
      krad_mixer_portgroup_destroy (krad_mixer, krad_mixer->portgroup[p]);
    }
  }
  
  free ( krad_mixer->crossfade_group );

  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    free ( krad_mixer->portgroup[p] );
  }
  
  krad_audio_destroy (krad_mixer->krad_audio);
  
  free ( krad_mixer->name );
  free ( krad_mixer );
  
  printk ("Krad Mixer shutdown complete");
}

krad_mixer_t *krad_mixer_create (char *name) {

  int p;

  krad_mixer_t *krad_mixer;

  if ((krad_mixer = calloc (1, sizeof (krad_mixer_t))) == NULL) {
    failfast ("Krad Mixer memory alloc failure");
  }
  
  krad_mixer->address.path.unit = KR_MIXER;
  krad_mixer->address.path.subunit.mixer_subunit = KR_UNIT;
  
  krad_mixer->name = strdup (name);
  krad_mixer->sample_rate = KRAD_MIXER_DEFAULT_SAMPLE_RATE;
  krad_mixer->rms_window_size = (krad_mixer->sample_rate / 1000) *
                                 KRAD_MIXER_RMS_WINDOW_SIZE_MS;
  krad_mixer->period_size = KRAD_MIXER_DEFAULT_PERIOD_SIZE;
  krad_mixer->frames_per_peak_broadcast = 1536;
  
  krad_mixer->crossfade_group = calloc (KRAD_MIXER_MAX_PORTGROUPS / 2,
                                        sizeof (krad_mixer_crossfade_group_t));

  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    krad_mixer->portgroup[p] = calloc (1, sizeof (krad_mixer_portgroup_t));
  }
  
  krad_mixer->krad_audio = krad_audio_create (krad_mixer);
  
  krad_mixer->master_mix =
    krad_mixer_portgroup_create (krad_mixer, "MasterBUS", MIX,
                                 NOTOUTPUT, 2, DEFAULT_MASTERBUS_LEVEL,
                                 NULL, MIXBUS, NULL, 0);
  
  krad_mixer->tone_port =
    krad_mixer_portgroup_create (krad_mixer, "DTMF",
                                 INPUT, NOTOUTPUT, 1, 35.0f,
                                 krad_mixer->master_mix, KRAD_TONE, NULL, 0);
  
  krad_mixer_portgroup_mixmap_channel (krad_mixer->tone_port, 0, 1);

  return krad_mixer;
}

