#include "krad_radio_client.h"
#include "krad_radio_client_internal.h"
#include "krad_mixer_common.h"

static int kr_mixer_response_get_string_from_mixer (kr_crate_t *crate, char **string);
static int kr_mixer_response_get_string_from_portgroup (kr_crate_t *crate, char **string);
static int kr_mixer_response_get_string_from_subunit_control (kr_crate_t *crate, char **string);

//static void kr_ebml_to_mixer_rep(kr_ebml2_t *ebml, kr_mixer_info *mixer_rep);
/* static int kr_ebml_to_mixer_portgroup_rep(kr_ebml2_t *ebml,
 kr_mixer_path_info *portgroup_rep); */


typedef struct kr_audioport_St kr_audioport_t;

struct kr_audioport_St {

  int samplerate;
  kr_shm_t *kr_shm;
  kr_client_t *client;
  int sd;

  int direction;

  int (*callback)(uint32_t, void *);
  void *pointer;

  int active;
  int error;

  pthread_t process_thread;

};

void kr_mixer_portgroup_list (kr_client_t *client) {

  unsigned char *mixer_command;
  unsigned char *get_portgroups;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_LIST_PORTGROUPS, &get_portgroups);

  kr_ebml2_finish_element (client->ebml2, get_portgroups);
  kr_ebml2_finish_element (client->ebml2, mixer_command);

  kr_client_push (client);
}

float *kr_audioport_get_buffer (kr_audioport_t *audioport, int channel) {
  return (float *)audioport->kr_shm->buffer + (channel * audioport->client->period_size);
}

void kr_audioport_set_callback (kr_audioport_t *kr_audioport, int callback (uint32_t, void *), void *pointer) {
  kr_audioport->callback = callback;
  kr_audioport->pointer = pointer;
}

void *kr_audioport_process_thread (void *arg) {

  kr_audioport_t *audioport = (kr_audioport_t *)arg;
  int ret;
  char buf[1];
  int max_timeout_ms;
  int timeout_total_ms;
  int timeout_ms;
  struct pollfd pollfds[1];

  timeout_ms = 30;
  timeout_total_ms = 0;
  max_timeout_ms = 1000;

  pollfds[0].fd = audioport->sd;

  krad_system_set_thread_name ("krc_audioport");

  while (audioport->active == 1) {

    pollfds[0].events = POLLIN;
    ret = poll (pollfds, 1, timeout_ms);

    if (ret == 0) {
      if (audioport->active == 1) {
        printke ("krad mixer client: audioport poll read timeout", ret);
      }
      timeout_total_ms += timeout_ms;
      if (timeout_total_ms > max_timeout_ms) {
        break;
      }
      continue;
    } else {
      timeout_total_ms = 0;
    }
    if (pollfds[0].revents & POLLHUP) {
      printke ("krad mixer client: audioport poll hangup", ret);
      break;
    }
    if (pollfds[0].revents & POLLERR) {
      printke ("krad mixer client: audioport poll error", ret);
      break;
    }

    ret = read (audioport->sd, buf, 1);
    if (ret != 1) {
      printke ("krad mixer client: unexpected read return value %d in kr_audioport_process_thread", ret);
      break;
    }

    audioport->callback (audioport->client->period_size, audioport->pointer);

    pollfds[0].events = POLLOUT;
    ret = poll (pollfds, 1, timeout_ms);

    if (ret == 0) {
      printke ("krad mixer client: audioport poll write timeout", ret);
      break;
    }

    if (pollfds[0].revents & POLLHUP) {
      printke ("krad mixer client: audioport poll hangup", ret);
      break;
    }
    if (pollfds[0].revents & POLLERR) {
      printke ("krad mixer client: audioport poll error", ret);
      break;
    }

    ret = write (audioport->sd, buf, 1);
    if (ret != 1) {
      printke ("krad mixer client: unexpected write return value %d in kr_audioport_process_thread", ret);
      break;
    }
  }

  if (audioport->active == 1) {
    audioport->error = 1;
  }

  return NULL;
}

void kr_audioport_connect(kr_audioport_t *audioport) {
  if ((audioport->active == 0) && (audioport->callback != NULL)) {
    audioport->active = 1;
    pthread_create (&audioport->process_thread, NULL, kr_audioport_process_thread, (void *)audioport);
  }
}

void kr_audioport_disconnect(kr_audioport_t *audioport) {
  if (audioport->active == 1) {
    audioport->active = 2;
    pthread_join (audioport->process_thread, NULL);
    audioport->active = 0;
    audioport->error = 0;
  }
}

int kr_audioport_error (kr_audioport_t *audioport) {
  if (audioport != NULL) {
    return audioport->error;
  }
  return -1;
}

kr_audioport_t *kr_audioport_create(kr_client_t *client, char *name,
 int direction) {

  kr_audioport_t *audioport;
  int sockets[2];

  if (!kr_client_local (client)) {
    // Local clients only
    return NULL;
  }

  audioport = calloc (1, sizeof(kr_audioport_t));

  if (audioport == NULL) {
    return NULL;
  }

  audioport->client = client;
  audioport->direction = direction;

  audioport->kr_shm = kr_shm_create (audioport->client);

  //sprintf (kr_audioport->kr_shm->buffer, "waa hoo its yaytime");

  if (audioport->kr_shm == NULL) {
    free (audioport);
    return NULL;
  }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
    kr_shm_destroy (audioport->kr_shm);
    free (audioport);
    return NULL;
    }

  audioport->sd = sockets[0];

  krad_system_set_socket_nonblocking (audioport->sd);
  /*
  krad_system_set_socket_blocking (audioport->client->krad_app_client->sd);
  kr_audioport_create_cmd (audioport->client, name, audioport->direction);
  usleep (5000);
  kr_send_fd (audioport->client, audioport->kr_shm->fd);
  kr_send_fd (audioport->client, sockets[1]);
  krad_system_set_socket_nonblocking (audioport->client->krad_app_client->sd);
  */
  return audioport;
}

void kr_audioport_destroy(kr_audioport_t *audioport) {

  if (audioport->active == 1) {
    kr_audioport_disconnect(audioport);
  }

  //kr_audioport_destroy_cmd (audioport->client);

  if (audioport != NULL) {
    if (audioport->sd != 0) {
      close (audioport->sd);
      audioport->sd = 0;
    }
    if (audioport->kr_shm != NULL) {
      kr_shm_destroy (audioport->kr_shm);
      audioport->kr_shm = NULL;
    }
    free (audioport);
  }
}

void kr_mixer_portgroup_xmms2_cmd (kr_client_t *client, char *portgroupname, char *xmms2_cmd) {

  unsigned char *mixer_command;
  unsigned char *bind;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_PORTGROUP_XMMS2_CMD, &bind);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_XMMS2_CMD, xmms2_cmd);
  kr_ebml2_finish_element (client->ebml2, bind);
  kr_ebml2_finish_element (client->ebml2, mixer_command);

  kr_client_push (client);
}

void kr_mixer_bind_portgroup_xmms2 (kr_client_t *client, char *portgroupname, char *ipc_path) {

  unsigned char *mixer_command;
  unsigned char *bind;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_BIND_PORTGROUP_XMMS2, &bind);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_XMMS2_IPC_PATH, ipc_path);
  kr_ebml2_finish_element (client->ebml2, bind);
  kr_ebml2_finish_element (client->ebml2, mixer_command);

  kr_client_push (client);
}

void kr_mixer_unbind_portgroup_xmms2 (kr_client_t *client, char *portgroupname) {

  unsigned char *mixer_command;
  unsigned char *unbind;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UNBIND_PORTGROUP_XMMS2, &unbind);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml2_finish_element (client->ebml2, unbind);
  kr_ebml2_finish_element (client->ebml2, mixer_command);

  kr_client_push (client);
}

void kr_mixer_set_sample_rate (kr_client_t *client, int sample_rate) {

  unsigned char *mixer_command;
  unsigned char *set_sample_rate;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_SET_SAMPLE_RATE, &set_sample_rate);

  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_SAMPLE_RATE, sample_rate);

  kr_ebml2_finish_element (client->ebml2, set_sample_rate);
  kr_ebml2_finish_element (client->ebml2, mixer_command);

  kr_client_push (client);
}

void kr_mixer_info_get(kr_client_t *client) {

  unsigned char *mixer_command;
  unsigned char *get_info;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_GET_INFO, &get_info);
  kr_ebml2_finish_element (client->ebml2, get_info);
  kr_ebml2_finish_element (client->ebml2, mixer_command);

  kr_client_push (client);
}

int kr_mixer_get_info_wait (kr_client_t *client,
                            uint32_t *sample_rate,
                            uint32_t *period_size) {

  int wait_ms;
  int ret;
  kr_crate_t *crate;

  ret = 0;
  crate = NULL;
  wait_ms = 750;

  kr_mixer_info_get (client);

  while (kr_delivery_get_until_final (client, &crate, wait_ms)) {
    if (crate != NULL) {
      if (kr_crate_loaded (crate)) {
        if (kr_crate_addr_path_match(crate, KR_MIXER, KR_UNIT)) {
          if (sample_rate != NULL) {
            *sample_rate = crate->inside.mixer->sample_rate;
          }
          if (period_size != NULL) {
            *period_size = crate->inside.mixer->period_size;
          }
          ret = 1;
        }
      }
      kr_crate_recycle (&crate);
    }
  }

  return ret;
}

void kr_mixer_plug_portgroup (kr_client_t *client, char *name, char *remote_name) {

  unsigned char *command;
  unsigned char *plug;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_PLUG_PORTGROUP, &plug);

  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, name);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, remote_name);
  kr_ebml2_finish_element (client->ebml2, plug);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_mixer_unplug_portgroup (kr_client_t *client, char *name, char *remote_name) {

  unsigned char *command;
  unsigned char *unplug;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UNPLUG_PORTGROUP, &unplug);

  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, name);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, remote_name);
  kr_ebml2_finish_element (client->ebml2, unplug);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_mixer_create_portgroup(kr_client_t *client, char *name, char *type,
 char *direction, int channels) {

  unsigned char *command;
  unsigned char *create;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_CREATE_PORTGROUP, &create);

  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, name);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, type);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_DIRECTION, direction);
  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, channels);
  kr_ebml2_finish_element (client->ebml2, create);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}


void kr_mixer_push_tone (kr_client_t *client, int8_t tone) {

  unsigned char *command;
  unsigned char *push;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_PUSH_TONE, &push);

  kr_ebml_pack_int8(client->ebml2, EBML_ID_KRAD_MIXER_TONE_NAME, tone);

  kr_ebml2_finish_element (client->ebml2, push);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_mixer_update_portgroup_map_channel (kr_client_t *client, char *portgroupname, int in_channel, int out_channel) {

  unsigned char *command;
  unsigned char *update;
  unsigned char *map;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP, &update);

  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_MAP_CHANNEL, &map);
  kr_ebml2_finish_element (client->ebml2, map);

  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNEL, in_channel);
  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNEL, out_channel);

  kr_ebml2_finish_element (client->ebml2, update);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_mixer_update_portgroup_mixmap_channel (kr_client_t *client, char *portgroupname, int in_channel, int out_channel) {

  unsigned char *command;
  unsigned char *update;
  unsigned char *map;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP, &update);

  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_MIXMAP_CHANNEL, &map);
  kr_ebml2_finish_element (client->ebml2, map);

  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNEL, in_channel);
  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNEL, out_channel);

  kr_ebml2_finish_element (client->ebml2, update);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}


void kr_mixer_set_portgroup_crossfade_group (kr_client_t *client, char *portgroupname, char *crossfade_group) {

  unsigned char *command;
  unsigned char *update;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP, &update);

  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE_NAME, crossfade_group);

  kr_ebml2_finish_element (client->ebml2, update);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_mixer_portgroup_info (kr_client_t *client, char *portgroupname) {

  unsigned char *command;
  unsigned char *info;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_PORTGROUP_INFO, &info);

  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);

  kr_ebml2_finish_element (client->ebml2, info);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_mixer_remove_portgroup (kr_client_t *client, char *name) {

  unsigned char *command;
  unsigned char *destroy;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_DESTROY_PORTGROUP, &destroy);

  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, name);

  kr_ebml2_finish_element (client->ebml2, destroy);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_mixer_set_effect_control(kr_client_t *client, char *portgroup_name, int effect_num,
                                 int control_id, char *control_name, float control_value, int duration,
                                 kr_easing easing) {

  unsigned char *mixer_command;
  unsigned char *set_control;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_SET_EFFECT_CONTROL, &set_control);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroup_name);
  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_EFFECT_NUM, effect_num);
  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_SUBUNIT, control_id);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_NAME, control_name);
  kr_ebml_pack_float (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_VALUE, control_value);
  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_DURATION, duration);
  kr_ebml_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_DURATION, easing);
  kr_ebml2_finish_element (client->ebml2, set_control);
  kr_ebml2_finish_element (client->ebml2, mixer_command);

  kr_client_push (client);
}

void kr_mixer_set_control (kr_client_t *client, char *portgroup_name, char *control_name, float control_value, uint32_t duration) {

  unsigned char *mixer_command;
  unsigned char *set_control;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_SET_CONTROL, &set_control);

  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroup_name);
  kr_ebml_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_NAME, control_name);
  kr_ebml_pack_float (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_VALUE, control_value);
  kr_ebml_pack_uint32 (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_DURATION, duration);

  kr_ebml2_finish_element (client->ebml2, set_control);
  kr_ebml2_finish_element (client->ebml2, mixer_command);

  kr_client_push (client);
}

/*static int kr_ebml_to_mixer_portgroup_rep(kr_ebml2_t *ebml, kr_mixer_path_info *portgroup_rep) {

  int i;

  kr_ebml2_unpack_element_string(ebml, NULL, portgroup_rep->name, sizeof(portgroup_rep->name));
  kr_ebml2_unpack_element_uint32(ebml, NULL, &portgroup_rep->channels);
//  kr_ebml2_unpack_element_uint32(ebml, NULL, &portgroup_rep->output_type);
  for (i = 0; i < portgroup_rep->channels; i++) {
    kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->volume[i]);
  }
  for (i = 0; i < portgroup_rep->channels; i++) {
    kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->peak[i]);
  }
  for (i = 0; i < portgroup_rep->channels; i++) {
    kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->rms[i]);
  }
  kr_ebml2_unpack_element_string (ebml, NULL, portgroup_rep->bus, sizeof(portgroup_rep->bus));
  kr_ebml2_unpack_element_string (ebml, NULL, portgroup_rep->crossfade_group, sizeof(portgroup_rep->crossfade_group));
  kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->fade);
  kr_ebml2_unpack_element_data (ebml, NULL, &portgroup_rep->eq, sizeof (portgroup_rep->eq));
  kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->lowpass.hz);
  kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->lowpass.bw);
  kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->highpass.hz);
  kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->highpass.bw);
  kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->analog.drive);
  kr_ebml2_unpack_element_float (ebml, NULL, &portgroup_rep->analog.blend);

  return 1;
}*/

/*static void kr_ebml_to_mixer_rep(kr_ebml *ebml, kr_mixer_info *mixer) {
  kr_ebml2_unpack_element_uint32(ebml, NULL, &mixer->period_size);
  kr_ebml2_unpack_element_uint32(ebml, NULL, &mixer->sample_rate);
  kr_ebml2_unpack_element_uint32(ebml, NULL, &mixer->inputs);
  kr_ebml2_unpack_element_uint32(ebml, NULL, &mixer->outputs);
  kr_ebml2_unpack_element_uint32(ebml, NULL, &mixer->buses);
  kr_ebml2_unpack_element_string(ebml, NULL, mixer->clock,
   sizeof(mixer->clock));
}*/

static int kr_mixer_response_get_string_from_subunit_control (kr_crate_t *crate, char **string) {

  int len;
  float real;

  len = 0;

  if (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP) {
    if ((crate->addr->control.portgroup_control == KR_VOLUME) || (crate->addr->control.portgroup_control == KR_CROSSFADE) ||
         (crate->addr->control.portgroup_control == KR_PEAK)) {
      kr_ebml2_unpack_element_float (&crate->payload_ebml, NULL, &real);
      len += sprintf (*string + len, "%5.2f", real);
    }
    if (crate->addr->control.portgroup_control == KR_CROSSFADE_GROUP) {
      kr_ebml2_unpack_element_string (&crate->payload_ebml, NULL, *string + len, 32);
      len += strlen (*string + len);
    }
  } else {
    if (crate->addr->path.subunit.mixer_subunit == KR_EFFECT) {
      kr_ebml2_unpack_element_float (&crate->payload_ebml, NULL, &real);
      len += sprintf (*string + len, "%5.2f", real);
    }
  }

  return len;
}

static int kr_mixer_response_get_string_from_mixer (kr_crate_t *crate, char **string) {

  int pos;
  kr_mixer_info mixer;

  pos = 0;

  //kr_ebml_to_mixer_rep (&crate->payload_ebml, &mixer);
  kr_mixer_info_fr_ebml(&crate->payload_ebml, &mixer);
  pos += kr_mixer_info_to_text(*string,(void *)&mixer,4096);

/*  pos += sprintf(*string + pos, "Mixer Status:\n");
  pos += sprintf(*string + pos, "Sample Rate: %u\n", mixer.sample_rate);
  pos += sprintf(*string + pos, "Inputs: %u\n", mixer.inputs);
  pos += sprintf(*string + pos, "Outputs: %u\n", mixer.outputs);
  pos += sprintf(*string + pos, "Buses: %u\n", mixer.buses);
  pos += sprintf(*string + pos, "Time Source: %s", mixer.clock);*/

  return pos;
}

static int kr_mixer_response_get_string_from_portgroup (kr_crate_t *crate, char **string) {

  int pos;
  int c;
  int i;
  kr_mixer_path_info portgroup_rep;

  pos = 0;

  //kr_ebml_to_mixer_portgroup_rep (&crate->payload_ebml, &portgroup_rep);
  memset(&portgroup_rep,0,sizeof(kr_mixer_path_info));
  kr_mixer_path_info_fr_ebml(&crate->payload_ebml, (void *)&portgroup_rep);
  //pos += kr_mixer_path_info_to_text(*string,(void *)&portgroup_rep,4096);

  //FIXME
//  if ((portgroup_rep.direction == OUTPUT) && (portgroup_rep.output_type == DIRECT)) {
//    pos += sprintf (*string + pos, "%d Channel ", portgroup_rep.channels);
//  } else {

    if ((portgroup_rep.channels == 1) || ((portgroup_rep.channels == 2) &&
         (portgroup_rep.volume[0] == portgroup_rep.volume[1]))) {

      pos += sprintf (*string + pos, "Volume: %6.2f%% ",
                      portgroup_rep.volume[0]);
    } else {
      for (c = 0; c < portgroup_rep.channels; c++) {
        pos += sprintf (*string + pos, "Chn %d Vol: %6.2f%% ",
                        c,
                        portgroup_rep.volume[c]);
      }
    }
//  }
/*
  if (portgroup_rep.direction == OUTPUT) {
    pos += sprintf (*string + pos, "%s ",
                   portgroup_output_type_to_string (portgroup_rep.output_type));
  }

  pos += sprintf (*string + pos, "%-7s",
                  portgroup_direction_to_string (portgroup_rep.direction));
*/
  if (portgroup_rep.channels == 1) {
    pos += sprintf (*string + pos, "%-8s (Mono)",
                    portgroup_rep.name);
  }
  if (portgroup_rep.channels == 2) {
    //pos += sprintf (*string + pos, " Stereo");
    pos += sprintf (*string + pos, "%-12s ",
                    portgroup_rep.name);
  }
  if (portgroup_rep.channels > 2) {
    pos += sprintf (*string + pos, "%-12s (%d Channel)",
                    portgroup_rep.name,
                    portgroup_rep.channels);
  }

  if (portgroup_rep.crossfade_group[0] != '\0') {
    pos += sprintf (*string + pos, "\n*Crossfade: %6.2f",
                    portgroup_rep.fade);
  }

//  if (portgroup_rep.direction == INPUT) {

//    if (portgroup_rep.has_xmms2 == 1) {
//      pos += sprintf (*string + pos, " [XMMS2] (%s)", portgroup_rep.xmms2_ipc_path);
 //   }

    pos += sprintf (*string + pos, "\n");

    pos += sprintf (*string + pos, " EQ Band \t %6s \t %6s \t %2s\n", "Db", "Hz", "BW");

    for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
      pos += sprintf (*string + pos, "     %2d:\t %6.2f \t %6.0f \t %0.2f\n",
                      i,
                      portgroup_rep.eq.band[i].db,
                      portgroup_rep.eq.band[i].hz,
                      portgroup_rep.eq.band[i].bw);
    }


    //if (krad_mixer_portgroup_rep->lowpass.hz != ) {
      pos += sprintf (*string + pos, "  Lowpass     Hz %8.2f      BW %5.2f\n",
                      portgroup_rep.lowpass.hz,
                      portgroup_rep.lowpass.bw);
    //}

    //if (krad_mixer_portgroup_rep->highpass.hz != ) {
      pos += sprintf (*string + pos, " Highpass     Hz %8.2f      BW %5.2f\n",
                      portgroup_rep.highpass.hz,
                      portgroup_rep.highpass.bw);
    //}

    //if (krad_mixer_portgroup_rep->analog.drive != ) {
      pos += sprintf (*string + pos, "   Analog  Drive    %5.2f   Blend %5.2f\n",
                      portgroup_rep.analog.drive,
                      portgroup_rep.analog.blend);
    //}

    pos += sprintf (*string + pos, "\n");
 // }

  return pos;
}

int kr_mixer_crate_to_string (kr_crate_t *crate, char **string) {

  switch ( crate->notice ) {
    case EBML_ID_KRAD_SUBUNIT_CONTROL:
      *string = kr_response_alloc_string (crate->size * 4);
      return kr_mixer_response_get_string_from_subunit_control (crate, string);
    case EBML_ID_KRAD_UNIT_INFO:
      *string = kr_response_alloc_string (crate->size * 4);
      return kr_mixer_response_get_string_from_mixer (crate, string);
    case EBML_ID_KRAD_SUBUNIT_INFO:
      *string = kr_response_alloc_string (crate->size * 4);
      return kr_mixer_response_get_string_from_portgroup (crate, string);
    case EBML_ID_KRAD_SUBUNIT_CREATED:
      *string = kr_response_alloc_string (crate->size * 4);
      return kr_mixer_response_get_string_from_portgroup (crate, string);
  }

  return 0;
}

int kr_mixer_crate_to_rep (kr_crate_t *crate) {

  if ((crate->address.path.subunit.mixer_subunit != KR_PORTGROUP) &&
      (crate->notice == EBML_ID_KRAD_UNIT_INFO)) {
    crate->contains = KR_MIXER;
    //kr_ebml_to_mixer_rep (&crate->payload_ebml, &crate->rep.mixer);
    kr_mixer_info_fr_ebml(&crate->payload_ebml, &crate->rep.mixer);

    crate->client->sample_rate = crate->rep.mixer.sample_rate;
    crate->client->period_size = crate->rep.mixer.period_size;

    return 1;
  }
  switch ( crate->address.path.subunit.mixer_subunit ) {
    case KR_EFFECT:

      break;
    case KR_PORTGROUP:
      if ((crate->notice == EBML_ID_KRAD_SUBUNIT_CREATED) || (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO)) {
        crate->contains = KR_PORTGROUP;
        //kr_ebml_to_mixer_portgroup_rep (&crate->payload_ebml, &crate->rep.portgroup);
        kr_mixer_path_info_fr_ebml(&crate->payload_ebml, (void *)&crate->rep.portgroup);
        return 1;
      }
      break;
    default:
      break;
  }
  return 0;
}
