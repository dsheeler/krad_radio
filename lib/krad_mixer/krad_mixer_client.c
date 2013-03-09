#include "krad_radio_client.h"
#include "krad_radio_client_internal.h"
#include "krad_mixer_common.h"

typedef struct kr_audioport_St kr_audioport_t;

struct kr_audioport_St {

  int samplerate;
  kr_shm_t *kr_shm;
  kr_client_t *client;
  int sd;
  
  krad_mixer_portgroup_direction_t direction;
  
  int (*callback)(uint32_t, void *);
  void *pointer;
  
  int active;
  
  pthread_t process_thread;  
  
};

void kr_mixer_portgroup_list (kr_client_t *client) {

  unsigned char *mixer_command;
  unsigned char *get_portgroups;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_LIST_PORTGROUPS, &get_portgroups);  

  kr_ebml2_finish_element (client->ebml2, get_portgroups);
  kr_ebml2_finish_element (client->ebml2, mixer_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_audioport_destroy_cmd (kr_client_t *client) {

  unsigned char *compositor_command;
  unsigned char *destroy_audioport;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_LOCAL_AUDIOPORT_DESTROY, &destroy_audioport);

  kr_ebml2_finish_element (client->ebml2, destroy_audioport);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_audioport_create_cmd (kr_client_t *client, krad_mixer_portgroup_direction_t direction) {

  unsigned char *compositor_command;
  unsigned char *create_audioport;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_LOCAL_AUDIOPORT_CREATE, &create_audioport);

  if (direction == OUTPUT) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_DIRECTION, "output");
  } else {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_DIRECTION, "input");
  }
  
  kr_ebml2_finish_element (client->ebml2, create_audioport);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}


float *kr_audioport_get_buffer (kr_audioport_t *kr_audioport, int channel) {
  return (float *)kr_audioport->kr_shm->buffer + (channel * 1600);
}


void kr_audioport_set_callback (kr_audioport_t *kr_audioport, int callback (uint32_t, void *), void *pointer) {
  kr_audioport->callback = callback;
  kr_audioport->pointer = pointer;
}

void *kr_audioport_process_thread (void *arg) {

  kr_audioport_t *kr_audioport = (kr_audioport_t *)arg;
  int ret;
  char buf[1];
  
  krad_system_set_thread_name ("krc_audioport");

  while (kr_audioport->active == 1) {
  
    // wait for socket to have a byte
    ret = read (kr_audioport->sd, buf, 1);
    if (ret != 1) {
      printke ("krad mixer client: unexpected read return value %d in kr_audioport_process_thread", ret);
    }
    //kr_audioport->callback (kr_audioport->kr_shm->buffer, kr_audioport->pointer);
    kr_audioport->callback (1600, kr_audioport->pointer);

    // write a byte to socket
    ret = write (kr_audioport->sd, buf, 1);
    if (ret != 1) {
      printke ("krad mixer client: unexpected write return value %d in kr_audioport_process_thread", ret);
    }

  }

  return NULL;
}

void kr_audioport_activate (kr_audioport_t *kr_audioport) {
  if ((kr_audioport->active == 0) && (kr_audioport->callback != NULL)) {
    pthread_create (&kr_audioport->process_thread, NULL, kr_audioport_process_thread, (void *)kr_audioport);
    kr_audioport->active = 1;
  }
}

void kr_audioport_deactivate (kr_audioport_t *kr_audioport) {

  if (kr_audioport->active == 1) {
    kr_audioport->active = 2;
    pthread_join (kr_audioport->process_thread, NULL);
    kr_audioport->active = 0;
  }
}

kr_audioport_t *kr_audioport_create (kr_client_t *client, krad_mixer_portgroup_direction_t direction) {

  kr_audioport_t *kr_audioport;
  int sockets[2];

  if (!kr_client_local (client)) {
    // Local clients only
    return NULL;
  }

  kr_audioport = calloc (1, sizeof(kr_audioport_t));

  if (kr_audioport == NULL) {
    return NULL;
  }

  kr_audioport->client = client;
  kr_audioport->direction = direction;

  kr_audioport->kr_shm = kr_shm_create (kr_audioport->client);

  //sprintf (kr_audioport->kr_shm->buffer, "waa hoo its yaytime");

  if (kr_audioport->kr_shm == NULL) {
    free (kr_audioport);
    return NULL;
  }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
    kr_shm_destroy (kr_audioport->kr_shm);
    free (kr_audioport);
    return NULL;
    }

  kr_audioport->sd = sockets[0];
  
  printf ("sockets %d and %d\n", sockets[0], sockets[1]);
  
  kr_audioport_create_cmd (kr_audioport->client, kr_audioport->direction);
  //FIXME use a return message from daemon to indicate ready to receive fds
  usleep (33000);
  kr_send_fd (kr_audioport->client, kr_audioport->kr_shm->fd);
  usleep (33000);
  kr_send_fd (kr_audioport->client, sockets[1]);
  usleep (33000);
  return kr_audioport;

}

void kr_audioport_destroy (kr_audioport_t *kr_audioport) {

  if (kr_audioport->active == 1) {
    kr_audioport_deactivate (kr_audioport);
  }

  kr_audioport_destroy_cmd (kr_audioport->client);

  if (kr_audioport != NULL) {
    if (kr_audioport->sd != 0) {
      close (kr_audioport->sd);
      kr_audioport->sd = 0;
    }
    if (kr_audioport->kr_shm != NULL) {
      kr_shm_destroy (kr_audioport->kr_shm);
      kr_audioport->kr_shm = NULL;
    }
    free(kr_audioport);
  }
}

void kr_mixer_portgroup_xmms2_cmd (kr_client_t *client, char *portgroupname, char *xmms2_cmd) {

  unsigned char *mixer_command;
  unsigned char *bind;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_PORTGROUP_XMMS2_CMD, &bind);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_XMMS2_CMD, xmms2_cmd);
  kr_ebml2_finish_element (client->ebml2, bind);
  kr_ebml2_finish_element (client->ebml2, mixer_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_bind_portgroup_xmms2 (kr_client_t *client, char *portgroupname, char *ipc_path) {

  unsigned char *mixer_command;
  unsigned char *bind;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_BIND_PORTGROUP_XMMS2, &bind);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_XMMS2_IPC_PATH, ipc_path);
  kr_ebml2_finish_element (client->ebml2, bind);
  kr_ebml2_finish_element (client->ebml2, mixer_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_unbind_portgroup_xmms2 (kr_client_t *client, char *portgroupname) {

  unsigned char *mixer_command;
  unsigned char *unbind;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UNBIND_PORTGROUP_XMMS2, &unbind);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml2_finish_element (client->ebml2, unbind);
  kr_ebml2_finish_element (client->ebml2, mixer_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_set_sample_rate (kr_client_t *client, int sample_rate) {

  unsigned char *mixer_command;
  unsigned char *set_sample_rate;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_SET_SAMPLE_RATE, &set_sample_rate);  

  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_SAMPLE_RATE, sample_rate);

  kr_ebml2_finish_element (client->ebml2, set_sample_rate);
  kr_ebml2_finish_element (client->ebml2, mixer_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_info (kr_client_t *client) {

  unsigned char *mixer_command;
  unsigned char *get_info;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_GET_INFO, &get_info);
  kr_ebml2_finish_element (client->ebml2, get_info);
  kr_ebml2_finish_element (client->ebml2, mixer_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_plug_portgroup (kr_client_t *client, char *name, char *remote_name) {

  unsigned char *command;
  unsigned char *plug;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_PLUG_PORTGROUP, &plug);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, name);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, remote_name);
  kr_ebml2_finish_element (client->ebml2, plug);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_unplug_portgroup (kr_client_t *client, char *name, char *remote_name) {

  unsigned char *command;
  unsigned char *unplug;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UNPLUG_PORTGROUP, &unplug);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, name);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, remote_name);
  kr_ebml2_finish_element (client->ebml2, unplug);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_create_portgroup (kr_client_t *client, char *name, char *direction, int channels) {

  unsigned char *command;
  unsigned char *create;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_CREATE_PORTGROUP, &create);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, name);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_DIRECTION, direction);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, channels);
  kr_ebml2_finish_element (client->ebml2, create);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}


void kr_mixer_push_tone (kr_client_t *client, char *tone) {

  unsigned char *command;
  unsigned char *push;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_PUSH_TONE, &push);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_TONE_NAME, tone);
  
  kr_ebml2_finish_element (client->ebml2, push);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_update_portgroup_map_channel (kr_client_t *client, char *portgroupname, int in_channel, int out_channel) {

  unsigned char *command;
  unsigned char *update;
  unsigned char *map;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP, &update);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_MAP_CHANNEL, &map);
  kr_ebml2_finish_element (client->ebml2, map);

  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNEL, in_channel);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNEL, out_channel);
  
  kr_ebml2_finish_element (client->ebml2, update);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_update_portgroup_mixmap_channel (kr_client_t *client, char *portgroupname, int in_channel, int out_channel) {

  unsigned char *command;
  unsigned char *update;
  unsigned char *map;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP, &update);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_MIXMAP_CHANNEL, &map);
  kr_ebml2_finish_element (client->ebml2, map);

  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNEL, in_channel);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNEL, out_channel);

  kr_ebml2_finish_element (client->ebml2, update);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}


void kr_mixer_set_portgroup_crossfade_group (kr_client_t *client, char *portgroupname, char *crossfade_group) {

  unsigned char *command;
  unsigned char *update;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP, &update);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE_NAME, crossfade_group);
  
  kr_ebml2_finish_element (client->ebml2, update);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_portgroup_info (kr_client_t *client, char *portgroupname) {

  unsigned char *command;
  unsigned char *info;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_PORTGROUP_INFO, &info);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroupname);

  kr_ebml2_finish_element (client->ebml2, info);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_remove_portgroup (kr_client_t *client, char *name) {

  unsigned char *command;
  unsigned char *destroy;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_DESTROY_PORTGROUP, &destroy);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, name);

  kr_ebml2_finish_element (client->ebml2, destroy);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_set_effect_control (kr_client_t *client, char *portgroup_name, int effect_num, 
                                  int control_id, char *control_name, float control_value, int duration,
                                  krad_ease_t ease) {

  unsigned char *mixer_command;
  unsigned char *set_control;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_SET_EFFECT_CONTROL, &set_control);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroup_name);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_EFFECT_NUM, effect_num);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_SUBUNIT, control_id);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_NAME, control_name);
  kr_ebml2_pack_float (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_VALUE, control_value);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_DURATION, duration);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_DURATION, ease);
  kr_ebml2_finish_element (client->ebml2, set_control);
  kr_ebml2_finish_element (client->ebml2, mixer_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_mixer_set_control (kr_client_t *client, char *portgroup_name, char *control_name, float control_value, int duration) {

  unsigned char *mixer_command;
  unsigned char *set_control;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD, &mixer_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_MIXER_CMD_SET_CONTROL, &set_control);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroup_name);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_NAME, control_name);
  kr_ebml2_pack_float (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_VALUE, control_value);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_MIXER_CONTROL_DURATION, duration);

  kr_ebml2_finish_element (client->ebml2, set_control);
  kr_ebml2_finish_element (client->ebml2, mixer_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

int kr_ebml_to_mixer_portgroup_rep (unsigned char *ebml_frag, kr_mixer_portgroup_t *portgroup_rep) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int i;
  char string[256];
  int item_pos;

  item_pos = 0;
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, portgroup_rep->sysname);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  portgroup_rep->channels = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  portgroup_rep->direction = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  portgroup_rep->output_type = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, string);

  if (strncmp (string, "Jack", 4) == 0) {
    portgroup_rep->io_type = 0;
  } else {
    portgroup_rep->io_type = 1;    
  }
  
  for (i = 0; i < portgroup_rep->channels; i++) {
    item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
    portgroup_rep->volume[i] = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  }
  
  for (i = 0; i < portgroup_rep->channels; i++) {
    item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
    portgroup_rep->peak[i] = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  }
  
  for (i = 0; i < portgroup_rep->channels; i++) {
    item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
    portgroup_rep->rms[i] = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  }
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, portgroup_rep->mixbus);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, portgroup_rep->crossfade_group);  

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  portgroup_rep->fade = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  portgroup_rep->has_xmms2 = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  if (portgroup_rep->has_xmms2 == 1) {
    item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
    item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, portgroup_rep->xmms2_ipc_path);  
  }
  
  /*
  for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
    item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
    krad_mixer_portgroup_rep->eq.band[i].db = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
    item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
    krad_mixer_portgroup_rep->eq.band[i].bandwidth = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
    item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
    krad_mixer_portgroup_rep->eq.band[i].hz = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  }
  */
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_data_from_frag (ebml_frag + item_pos, &portgroup_rep->eq, ebml_data_size);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  portgroup_rep->lowpass.hz = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  portgroup_rep->lowpass.bandwidth = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  portgroup_rep->highpass.hz = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  portgroup_rep->highpass.bandwidth = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  portgroup_rep->analog.drive = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  portgroup_rep->analog.blend = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  return 1;
}

void kr_ebml_to_mixer_rep (unsigned char *ebml_frag, kr_mixer_t *mixer_rep) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int item_pos;

  item_pos = 0;
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  mixer_rep->sample_rate = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  mixer_rep->inputs = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  mixer_rep->outputs = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  mixer_rep->buses = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, mixer_rep->time_source);

}

int kr_mixer_response_get_string_from_subunit_control (kr_address_t *address, unsigned char *ebml_frag, uint64_t item_size, char **string) {

  int pos;
  uint32_t ebml_id;
  uint64_t ebml_data_size;  
  int ebml_pos;
  float value;
  
  ebml_pos = 0;
  pos = 0;

  ebml_pos += krad_ebml_read_element_from_frag (ebml_frag + ebml_pos, &ebml_id, &ebml_data_size);

  if (address->path.subunit.mixer_subunit == KR_PORTGROUP) {
    if ((address->control.portgroup_control == KR_VOLUME) || (address->control.portgroup_control == KR_CROSSFADE) ||
         (address->control.portgroup_control == KR_PEAK)) {
      value = krad_ebml_read_float_from_frag_add (ebml_frag + ebml_pos, ebml_data_size, &ebml_pos);
      pos += sprintf (*string + pos, "%5.2f", value);
    }
    if ((address->control.portgroup_control == KR_CROSSFADE_GROUP) || (address->control.portgroup_control == KR_XMMS2_IPC_PATH)) {
      pos += krad_ebml_read_string_from_frag (ebml_frag + ebml_pos, ebml_data_size, *string + pos);
    }
  }

  if (address->path.subunit.mixer_subunit == KR_EFFECT) {
    value = krad_ebml_read_float_from_frag_add (ebml_frag + ebml_pos, ebml_data_size, &ebml_pos);
    pos += sprintf (*string + pos, "%5.2f", value);
  }
    
  return pos; 
}

int kr_mixer_response_get_string_from_mixer (unsigned char *ebml_frag, uint64_t item_size, char **string) {

  int pos;
  kr_mixer_t kr_mixer;

  pos = 0;

  kr_ebml_to_mixer_rep (ebml_frag, &kr_mixer);
  pos += sprintf (*string + pos, "Sample Rate: %u\n", kr_mixer.sample_rate);
  pos += sprintf (*string + pos, "Inputs: %u\n", kr_mixer.inputs);
  pos += sprintf (*string + pos, "Outputs: %u\n", kr_mixer.outputs);
  pos += sprintf (*string + pos, "Buses: %u\n", kr_mixer.buses);
  pos += sprintf (*string + pos, "Time Source: %s", kr_mixer.time_source);
  
  return pos; 
}

int kr_mixer_response_get_string_from_portgroup (unsigned char *ebml_frag, uint64_t item_size, char **string) {

  int pos;
  int c;
  int i;
  krad_mixer_portgroup_rep_t portgroup_rep;

  pos = 0;
  
  kr_ebml_to_mixer_portgroup_rep (ebml_frag, &portgroup_rep);
 
  if ((portgroup_rep.direction == OUTPUT) && (portgroup_rep.output_type == DIRECT)) {
    pos += sprintf (*string + pos, "%d Channel ", portgroup_rep.channels);
  } else {

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
  }
    
  if (portgroup_rep.direction == OUTPUT) {
    pos += sprintf (*string + pos, "%s ",
                   portgroup_output_type_to_string (portgroup_rep.output_type));
  }

  pos += sprintf (*string + pos, "%-7s",
                  portgroup_direction_to_string (portgroup_rep.direction));

  if (portgroup_rep.channels == 1) {
    pos += sprintf (*string + pos, "%-8s (Mono)",
                    portgroup_rep.sysname);
  }
  if (portgroup_rep.channels == 2) {
    //pos += sprintf (*string + pos, " Stereo");
    pos += sprintf (*string + pos, "%-12s ",
                    portgroup_rep.sysname);
  }
  if (portgroup_rep.channels > 2) {
    pos += sprintf (*string + pos, "%-12s (%d Channel)",
                    portgroup_rep.sysname,
                    portgroup_rep.channels);
  }
  
  if (portgroup_rep.crossfade_group[0] != '\0') {
    pos += sprintf (*string + pos, "\n*Crossfade: %6.2f",
                    portgroup_rep.fade);
  }

  if (portgroup_rep.direction == INPUT) {
  
    if (portgroup_rep.has_xmms2 == 1) {
      pos += sprintf (*string + pos, " [XMMS2] (%s)", portgroup_rep.xmms2_ipc_path);
    }

    pos += sprintf (*string + pos, "\n");

    pos += sprintf (*string + pos, " EQ Band \t %6s \t %6s \t %2s\n", "Db", "Hz", "BW");
    
    for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
      pos += sprintf (*string + pos, "     %2d:\t %6.2f \t %6.0f \t %0.2f\n",
                      i, 
                      portgroup_rep.eq.band[i].db,
                      portgroup_rep.eq.band[i].hz,
                      portgroup_rep.eq.band[i].bandwidth);
    }
    

    //if (krad_mixer_portgroup_rep->lowpass.hz != ) {
      pos += sprintf (*string + pos, "  Lowpass     Hz %8.2f      BW %5.2f\n", 
                      portgroup_rep.lowpass.hz,
                      portgroup_rep.lowpass.bandwidth);  
    //}

    //if (krad_mixer_portgroup_rep->highpass.hz != ) {
      pos += sprintf (*string + pos, " Highpass     Hz %8.2f      BW %5.2f\n", 
                      portgroup_rep.highpass.hz,
                      portgroup_rep.highpass.bandwidth);
    //}
    
    //if (krad_mixer_portgroup_rep->analog.drive != ) {
      pos += sprintf (*string + pos, "   Analog  Drive    %5.2f   Blend %5.2f\n", 
                      portgroup_rep.analog.drive,
                      portgroup_rep.analog.blend);  
    //}

    pos += sprintf (*string + pos, "\n");
  }

  return pos; 
}

int kr_mixer_response_to_string (kr_response_t *kr_response, char **string) {

  switch ( kr_response->notice ) {
    case EBML_ID_KRAD_SUBUNIT_CONTROL:
      *string = kr_response_alloc_string (kr_response->size * 4);
      return kr_mixer_response_get_string_from_subunit_control (&kr_response->address, kr_response->buffer, kr_response->size, string);
    case EBML_ID_KRAD_UNIT_INFO:
      *string = kr_response_alloc_string (kr_response->size * 4);
      return kr_mixer_response_get_string_from_mixer (kr_response->buffer, kr_response->size, string);
    case EBML_ID_KRAD_SUBUNIT_INFO:
      *string = kr_response_alloc_string (kr_response->size * 4);
      return kr_mixer_response_get_string_from_portgroup (kr_response->buffer, kr_response->size, string);
    case EBML_ID_KRAD_SUBUNIT_CREATED:
      *string = kr_response_alloc_string (kr_response->size * 4);
      return kr_mixer_response_get_string_from_portgroup (kr_response->buffer, kr_response->size, string);
  }
  
  return 0;
}

