#include "krad_mixer_interface.h"

void krad_mixer_portgroup_to_rep (krad_mixer_portgroup_t *portgroup,
                                  kr_portgroup_t *portgroup_rep) {

  int i;
  
  strcpy (portgroup_rep->sysname, portgroup->sysname);
  portgroup_rep->channels = portgroup->channels;
  portgroup_rep->direction = portgroup->direction;
  portgroup_rep->output_type = portgroup->output_type;
  portgroup_rep->io_type = portgroup->io_type;
  
  if (portgroup->mixbus != NULL) {
    strncpy(portgroup_rep->mixbus, portgroup->mixbus->sysname,
     sizeof(portgroup_rep->mixbus));
  } else {
    portgroup_rep->mixbus[0] = '\0';
  }
  
  for (i = 0; i < KRAD_MIXER_MAX_CHANNELS; i++) {
    portgroup_rep->volume[i] = portgroup->volume[i];
    portgroup_rep->map[i] = portgroup->map[i];
    portgroup_rep->mixmap[i] = portgroup->mixmap[i];
    portgroup_rep->rms[i] = portgroup->avg[i];
    portgroup_rep->peak[i] = portgroup->peak_last[i];
  }
  
  if (portgroup->krad_xmms != NULL) {
    portgroup_rep->has_xmms2 = 1;
    strncpy (portgroup_rep->xmms2_ipc_path,
             portgroup->krad_xmms->ipc_path,
             sizeof(portgroup_rep->xmms2_ipc_path));
  } else {
    portgroup_rep->has_xmms2 = 0;
    portgroup_rep->xmms2_ipc_path[0] = '\0';
  }
  
  kr_eq_t *eq;
  kr_pass_t *lowpass;
  kr_pass_t *highpass;
  kr_analog_t *analog;
  
  if (portgroup->direction == INPUT) {
    eq = (kr_eq_t *)portgroup->effects->effect[0].effect[0];
    lowpass = (kr_lowpass_t *)portgroup->effects->effect[1].effect[0];
    highpass = (kr_highpass_t *)portgroup->effects->effect[2].effect[0];
    analog = (kr_analog_t *)portgroup->effects->effect[3].effect[0];

    for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
      portgroup_rep->eq.band[i].db = eq->band[i].db;
      portgroup_rep->eq.band[i].bandwidth = eq->band[i].bandwidth;
      portgroup_rep->eq.band[i].hz = eq->band[i].hz;
    }
    
    portgroup_rep->lowpass.hz = lowpass->hz;
    portgroup_rep->lowpass.bandwidth = lowpass->bandwidth;
    portgroup_rep->highpass.hz = highpass->hz;
    portgroup_rep->highpass.bandwidth = highpass->bandwidth;
    
    portgroup_rep->analog.drive = analog->drive;
    portgroup_rep->analog.blend = analog->blend;
  
  } else {

    for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
      portgroup_rep->eq.band[i].db = 0.0f;
      portgroup_rep->eq.band[i].bandwidth = 0.0f;
      portgroup_rep->eq.band[i].hz = 0.0f;
    }
    
    portgroup_rep->lowpass.hz = 0.0f;
    portgroup_rep->lowpass.bandwidth = 0.0f;
    portgroup_rep->highpass.hz = 0.0f;
    portgroup_rep->highpass.bandwidth = 0.0f;
    
    portgroup_rep->analog.drive = 0.0f;
    portgroup_rep->analog.blend = 0.0f;
  }
  
  if ((portgroup->crossfade_group != NULL) && (portgroup->crossfade_group->portgroup[0] == portgroup)) {
    portgroup_rep->fade = portgroup->crossfade_group->fade;
    strncpy (portgroup_rep->crossfade_group,
             portgroup->crossfade_group->portgroup[1]->sysname,
             sizeof(portgroup_rep->crossfade_group));
  } else {
    portgroup_rep->crossfade_group[0] = '\0';
    portgroup_rep->fade = 0.0f;
  }
}

void krad_mixer_to_rep ( krad_mixer_t *krad_mixer, kr_mixer_t *mixer_rep ) {

  krad_mixer_portgroup_t *portgroup;
  int p;

  p = 0;
  portgroup = NULL;

  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    portgroup = krad_mixer->portgroup[p];
    if ((portgroup != NULL) && ((portgroup->active == 1) || (portgroup->active == 2))) {
      if (portgroup->direction == INPUT) {
        mixer_rep->inputs++;
      }
      if (portgroup->direction == OUTPUT) {
        mixer_rep->outputs++;
      }
      if (portgroup->direction == MIX) {
        mixer_rep->buses++;
      }
    }
  }

  mixer_rep->period_size = krad_mixer_get_period_size (krad_mixer);
  mixer_rep->sample_rate = krad_mixer_get_sample_rate (krad_mixer);
  
  if (krad_mixer->pusher == JACK) {
    strncpy (mixer_rep->time_source, "Jack", sizeof(mixer_rep->time_source));
  } else {
    strncpy (mixer_rep->time_source, "Internal Chronometer", sizeof(mixer_rep->time_source));  
  }  
}

void krad_mixer_rep_to_ebml ( kr_ebml2_t *ebml, kr_mixer_t *mixer_rep ) {
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_MIXER_SAMPLE_RATE, mixer_rep->period_size);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_MIXER_SAMPLE_RATE, mixer_rep->sample_rate);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_COUNT, mixer_rep->inputs);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_COUNT, mixer_rep->outputs);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_COUNT, mixer_rep->buses);
  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_MIXER_TIME_SOURCE, mixer_rep->time_source);
}

void krad_mixer_to_ebml ( kr_ebml2_t *ebml, krad_mixer_t *krad_mixer ) {
  kr_mixer_t mixer_rep;
  memset (&mixer_rep, 0, sizeof (kr_mixer_t));
  krad_mixer_to_rep (krad_mixer, &mixer_rep);
  krad_mixer_rep_to_ebml (ebml, &mixer_rep);
}

int krad_mixer_command ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client ) {

  krad_mixer_portgroup_t *portgroup;
  krad_mixer_portgroup_t *portgroup2;
  krad_mixer_portgroup_rep_t portgroup_rep;
  krad_mixer_output_t output_type;
  char portgroupname[64];
  char portgroupname2[64];  
  char controlname[16];  
  void *ptr;
  float floatval;
  int direction;
  int p;
  krad_radio_t *krad_radio;
  krad_mixer_t *krad_mixer;  
  kr_address_t address;
  unsigned char *response;
  unsigned char *payload;
  kr_ebml2_t ebml_in;
  kr_ebml2_t ebml_out;
  uint32_t command;
  uint32_t element;
  uint64_t size;
  int ret;
  char string[512];
  char string2[256];  
  uint32_t numbers[10];
  krad_app_server_t *app;
  char tone;
  int sd1;
  int sd2;    

  sd1 = 0;
  sd2 = 0;
  ptr = NULL;
  krad_radio = client->krad_radio;
  krad_mixer = krad_radio->krad_mixer;
  app = krad_radio->app;
  direction = 0;
  portgroupname[0] = '\0';
  portgroupname2[0] = '\0';
  controlname[0] = '\0';
  string[0] = '\0';

  if (!(kr_io2_has_in (in))) {
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_in, in->rd_buf, in->len );

  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if ((ret < 0) || (command != EBML_ID_KRAD_MIXER_CMD)) {
    printke ("krad_mixer_command invalid EBML ID Not found");
    return 0;
  }

  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if (ret < 0) {
    printke ("krad_mixer_command EBML ID Not found");
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_out, out->buf, out->space );

  switch ( command ) {
    case EBML_ID_KRAD_MIXER_CMD_SET_CONTROL:
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      kr_ebml2_unpack_element_string (&ebml_in, &element, controlname, sizeof(controlname));
      kr_ebml2_unpack_element_float (&ebml_in, &element, &floatval);
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      if ((numbers[0] == 0) && (krad_app_server_current_client_is_subscriber (app))) {
        ptr = app->current_client;
      }
      krad_mixer_set_portgroup_control( krad_mixer, portgroupname, controlname, floatval, numbers[0], ptr );
      break;
    case EBML_ID_KRAD_MIXER_CMD_SET_EFFECT_CONTROL:
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[5]);
      kr_ebml2_unpack_element_string (&ebml_in, &element, controlname, sizeof(controlname));
      kr_ebml2_unpack_element_float (&ebml_in, &element, &floatval);
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[6]);
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[7]);;
      portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, portgroupname);
      if (portgroup != NULL) {
        kr_effects_effect_set_control (portgroup->effects, numbers[0], numbers[5],
                   kr_effects_string_to_effect_control(portgroup->effects->effect[numbers[0]].effect_type,
                                                        controlname),
                                       floatval, numbers[6], numbers[7]);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_PUSH_TONE:
      tone = 0;
      kr_ebml2_unpack_element_int8(&ebml_in, &element, (int8_t *)&tone);
      if (krad_mixer->push_tone == -1) {
        krad_mixer->push_tone = tone;
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_PORTGROUP_XMMS2_CMD:
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string, sizeof(string));
      krad_mixer_portgroup_xmms2_cmd (krad_mixer, portgroupname, string);
      break;
    case EBML_ID_KRAD_MIXER_CMD_PLUG_PORTGROUP:
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname2, sizeof(portgroupname2));
      krad_mixer_plug_portgroup (krad_mixer, portgroupname, portgroupname2);
      break;
    case EBML_ID_KRAD_MIXER_CMD_UNPLUG_PORTGROUP:
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname2, sizeof(portgroupname2));
      krad_mixer_unplug_portgroup (krad_mixer, portgroupname, portgroupname2);
      break;
    case EBML_ID_KRAD_MIXER_CMD_BIND_PORTGROUP_XMMS2:  
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string, sizeof(string));
      krad_mixer_portgroup_bind_xmms2 (krad_mixer, portgroupname, string);
      break;
    case EBML_ID_KRAD_MIXER_CMD_UNBIND_PORTGROUP_XMMS2:
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      krad_mixer_portgroup_unbind_xmms2 (krad_mixer, portgroupname);
      break;
    case EBML_ID_KRAD_MIXER_CMD_LIST_PORTGROUPS:
      for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
        portgroup = krad_mixer->portgroup[p];
        if ((portgroup != NULL) && ((portgroup->active == 1) || (portgroup->active == 2))) {
          krad_radio_address_to_ebml2 (&ebml_out, &response, &portgroup->address);
          kr_ebml2_pack_uint32 ( &ebml_out,
                                 EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                 EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          krad_mixer_portgroup_to_rep (portgroup, &portgroup_rep);
          krad_mixer_portgroup_rep_to_ebml2 (&portgroup_rep, &ebml_out);
          kr_ebml2_finish_element (&ebml_out, payload);
          kr_ebml2_finish_element (&ebml_out, response);
        }
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_CREATE_PORTGROUP:
      kr_ebml2_unpack_element_string(&ebml_in, &element, portgroupname, sizeof(portgroupname));
      kr_ebml2_unpack_element_string(&ebml_in, &element, string, sizeof(string));
      kr_ebml2_unpack_element_string(&ebml_in, &element, string2, sizeof(string2));
      
      if (strncmp(string, "output", 6) == 0) {
        direction = OUTPUT;
        output_type = DIRECT;
      } else {
        if (strncmp(string, "auxout", 6) == 0) {
          direction = OUTPUT;
          output_type = AUX;
        } else {
          direction = INPUT;
          output_type = NOTOUTPUT;
        }
      }
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      
      if (strncmp(string2, "krad", 4) == 0) {
        krad_system_set_socket_blocking (app->current_client->sd);
        
        sd1 = krad_app_server_recvfd (app->current_client);
        sd2 = krad_app_server_recvfd (app->current_client);
        printk ("AUDIOPORT_CREATE %s Got FD's %d and %d\n", string, sd1, sd2);
        
        portgroup = krad_mixer_local_portgroup_create(krad_mixer, string,
         direction, sd1, sd2);

        krad_system_set_socket_nonblocking (app->current_client->sd);
      } else {
        portgroup = krad_mixer_portgroup_create (krad_mixer, portgroupname,
                     direction, output_type, numbers[0],
                     0.0f, krad_mixer->master_mix, KRAD_AUDIO, NULL, JACK);      
      }
      if (portgroup != NULL) {
        krad_radio_broadcast_subunit_created ( app->app_broadcaster,
         &portgroup->address, (void *)portgroup);
        krad_mixer_set_portgroup_control(krad_mixer, portgroupname, "volume",
         100.0f, 500, NULL);
      } else {
        printke ("Krad Mixer: Failed to create portgroup: %s", portgroupname);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_DESTROY_PORTGROUP:
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, portgroupname);
      if (portgroup != NULL) {
        krad_mixer_portgroup_destroy (krad_mixer, portgroup);
        address.path.unit = KR_MIXER;
        address.path.subunit.mixer_subunit = KR_PORTGROUP;
        strncpy (address.id.name, portgroupname, sizeof (address.id.name));
        krad_radio_broadcast_subunit_destroyed (app->app_broadcaster, &address);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_PORTGROUP_INFO:
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, portgroupname);
      if ((portgroup != NULL) && ((portgroup->active == 1) || (portgroup->active == 2))) {
        krad_radio_address_to_ebml2 (&ebml_out, &response, &portgroup->address);
        kr_ebml2_pack_uint32 ( &ebml_out,
                               EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                               EBML_ID_KRAD_SUBUNIT_INFO);
        kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        krad_mixer_portgroup_to_rep (portgroup, &portgroup_rep);
        krad_mixer_portgroup_rep_to_ebml2 (&portgroup_rep, &ebml_out);
        kr_ebml2_finish_element (&ebml_out, payload);
        kr_ebml2_finish_element (&ebml_out, response);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP:      
      kr_ebml2_unpack_element_string (&ebml_in, &element, portgroupname, sizeof(portgroupname));
      kr_ebml2_unpack_id (&ebml_in, &element, &size);
      if (element == EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE_NAME) {
        kr_ebml2_unpack_string (&ebml_in, string, size);
        portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, portgroupname);
        if (portgroup != NULL) {
          if (portgroup->crossfade_group != NULL) {
            krad_mixer_crossfade_group_destroy (krad_mixer, portgroup->crossfade_group);
            if (strlen(string) == 0) {
              return 0;
            }
          }
          if (strlen(string) > 0) {
            portgroup2 = krad_mixer_get_portgroup_from_sysname (krad_mixer, string);
            if (portgroup2 != NULL) {
              if (portgroup2->crossfade_group != NULL) {
                krad_mixer_crossfade_group_destroy (krad_mixer, portgroup2->crossfade_group);
              }
              if (portgroup != portgroup2) {
                krad_mixer_crossfade_group_create (krad_mixer, portgroup, portgroup2);
              }
            }
          }
        }
      }
      if (element == EBML_ID_KRAD_MIXER_MAP_CHANNEL) {
        kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
        kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[1]);
        portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, portgroupname);
        if (portgroup != NULL) {
          krad_mixer_portgroup_map_channel (portgroup, numbers[0], numbers[1]);      
        }
      }
      if (element == EBML_ID_KRAD_MIXER_MIXMAP_CHANNEL) {
        kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
        kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[1]);
        portgroup = krad_mixer_get_portgroup_from_sysname (krad_mixer, portgroupname);
        if (portgroup != NULL) {
          krad_mixer_portgroup_mixmap_channel (portgroup, numbers[0], numbers[1]);
        }
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_GET_INFO:
      krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_mixer->address);
      kr_ebml2_pack_uint32 ( &ebml_out,
                             EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                             EBML_ID_KRAD_UNIT_INFO);
      kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      krad_mixer_to_ebml (&ebml_out, krad_mixer);
      kr_ebml2_finish_element (&ebml_out, payload);
      kr_ebml2_finish_element (&ebml_out, response);
      break;
    /*
    case EBML_ID_KRAD_MIXER_CMD_SET_SAMPLE_RATE:
      krad_ebml_read_element (app->current_client->krad_ebml, &ebml_id, &ebml_data_size);
      number = krad_ebml_read_number (app->current_client->krad_ebml, ebml_data_size);
      if (krad_mixer_has_pusher (krad_mixer) == 0) {
        krad_mixer_set_sample_rate (krad_mixer, number);
      }
      break;
    */
    default:
      return -1;    
  }
  
  if (((ebml_out.pos > 0) || (command == EBML_ID_KRAD_MIXER_CMD_LIST_PORTGROUPS)) &&
       (!krad_app_server_current_client_is_subscriber (app))) {
    krad_radio_pack_shipment_terminator (&ebml_out);
  }

  kr_io2_pulled (in, ebml_in.pos);
  kr_io2_advance (out, ebml_out.pos);
  
  return 0;
}
