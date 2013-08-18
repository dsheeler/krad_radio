#include "krad_mixer_interface.h"

void kr_mixer_unit_to_rep(kr_mixer_path *unit, kr_mixer_path_info *unit_rep) {

  int i;

  strcpy(unit_rep->name, unit->name);
  unit_rep->channels = unit->channels;
//unit_rep->io_type = unit->io_type;
  if (unit->bus != NULL) {
    strncpy(unit_rep->bus, unit->bus->name, sizeof(unit_rep->bus));
  } else {
    unit_rep->bus[0] = '\0';
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    unit_rep->volume[i] = unit->volume[i];
    unit_rep->map[i] = unit->map[i];
    unit_rep->mixmap[i] = unit->mixmap[i];
    unit_rep->rms[i] = unit->avg[i];
    unit_rep->peak[i] = unit->peak_last[i];
  }
  kr_sfx_effect_info(unit->sfx, 0, &unit_rep->eq);
  kr_sfx_effect_info(unit->sfx, 1, &unit_rep->lowpass);
  kr_sfx_effect_info(unit->sfx, 2, &unit_rep->highpass);
  kr_sfx_effect_info(unit->sfx, 3, &unit_rep->analog);
  if ((unit->crossfader != NULL) && (unit->crossfader->unit[0] == unit)) {
    unit_rep->fade = unit->crossfader->fade;
    strncpy(unit_rep->crossfade_group, unit->crossfader->unit[1]->name,
     sizeof(unit_rep->crossfade_group));
  } else {
    unit_rep->crossfade_group[0] = '\0';
    unit_rep->fade = 0.0f;
  }
}

void kr_mixer_to_rep(kr_mixer *mixer, kr_mixer_info *mixer_rep) {

  kr_mixer_path *unit;
  int p;

  p = 0;
  unit = NULL;

  for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && ((unit->active == 1) || (unit->active == 2))) {
      if (unit->type == KR_MXR_INPUT) {
        mixer_rep->inputs++;
      }
      if (unit->type == KR_MXR_AUX) {
        mixer_rep->auxes++;
      }
      if (unit->type == KR_MXR_BUS) {
        mixer_rep->buses++;
      }
    }
  }

  mixer_rep->period_size = kr_mixer_period(mixer);
  mixer_rep->sample_rate = kr_mixer_sample_rate(mixer);

  strncpy(mixer_rep->clock, "Something", sizeof(mixer_rep->clock));
/*
  if (mixer->pusher == KR_JACK) {
    strncpy(mixer_rep->clock, "Jack", sizeof(mixer_rep->clock));
  } else {
    strncpy(mixer_rep->clock, "Internal Chronometer",
     sizeof(mixer_rep->clock));
  }
*/
}

void kr_mixer_info_to_ebml(kr_ebml2_t *ebml, kr_mixer_info *mxr) {
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_SAMPLE_RATE, mxr->period_size);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_SAMPLE_RATE, mxr->sample_rate);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_COUNT, mxr->inputs);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_COUNT, mxr->auxes);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_COUNT, mxr->buses);
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_MIXER_TIME_SOURCE, mxr->clock);
}

void kr_mixer_to_ebml(kr_ebml *e, kr_mixer *mixer) {
  kr_mixer_info info;
  memset(&info, 0, sizeof(kr_mixer_info));
  kr_mixer_to_rep(mixer, &info);
  kr_mixer_info_to_ebml(e, &info);
}

static uint32_t ms_to_cycles(int sample_rate, int cycle_frames, int ms) {

  uint32_t cycles;
  float samples_ms;
  float cycle_ms;

  if ((ms < 1) || (ms > (10 * 60 * 1000))) {
    return 0;
  }

  samples_ms = sample_rate / 1000.0f;
  cycle_ms = cycle_frames / samples_ms;

  cycles = (ms / cycle_ms) + 1;

  //printk("MS: %d Cycles: %u samples_ms: %f cycle_ms %f", ms, cycles,
  // samples_ms, cycle_ms);

  return cycles;
}

static uint32_t mixer_ms_to_cycles(kr_mixer *mixer, int ms) {

  int sample_rate;
  int period_size;

  /* FIXME this should be cached info */
  sample_rate = kr_mixer_sample_rate(mixer);
  period_size = kr_mixer_period(mixer);

  return ms_to_cycles(sample_rate, period_size, ms);
}

int kr_mixer_command(kr_io2_t *in, kr_io2_t *out, kr_radio_client *client) {

  kr_mixer_path *unit;
  kr_mixer_path *unit2;
  kr_mixer_path_info unit_rep;
  char unitname[64];
  char unitname2[64];
  char controlname[16];
  void *ptr;
  float floatval;
  int p;
  kr_radio *radio;
  kr_mixer *mixer;
  kr_address_t address;
  unsigned char *response;
  unsigned char *payload;
  kr_ebml ebml_in;
  kr_ebml ebml_out;
  uint32_t command;
  uint32_t element;
  uint64_t size;
  int ret;
  char string[512];
  char string2[256];
  uint32_t numbers[10];
  kr_app_server *as;
  char tone;
  int sd1;
  int sd2;
  int duration;

  duration = 0;
  sd1 = 0;
  sd2 = 0;
  ptr = NULL;
  radio = client->krad_radio;
  mixer = radio->mixer;
  as = radio->app;
  unitname[0] = '\0';
  unitname2[0] = '\0';
  controlname[0] = '\0';
  string[0] = '\0';

  if (!(kr_io2_has_in(in))) {
    return 0;
  }

  kr_ebml2_set_buffer(&ebml_in, in->rd_buf, in->len);

  ret = kr_ebml2_unpack_id(&ebml_in, &command, &size);
  if ((ret < 0) || (command != EBML_ID_KRAD_MIXER_CMD)) {
    printke("krad_mixer_command invalid EBML ID Not found");
    return 0;
  }

  ret = kr_ebml2_unpack_id(&ebml_in, &command, &size);
  if (ret < 0) {
    printke("krad_mixer_command EBML ID Not found");
    return 0;
  }

  kr_ebml2_set_buffer(&ebml_out, out->buf, out->space);

  switch (command) {
    case EBML_ID_KRAD_MIXER_CMD_SET_CONTROL:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_element_string(&ebml_in, &element, controlname, sizeof(controlname));
      kr_ebml2_unpack_element_float(&ebml_in, &element, &floatval);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      duration = numbers[0];
      if ((duration == 0) && (krad_app_server_current_client_is_subscriber(as))) {
        ptr = as->current_client;
      } else {
        duration = mixer_ms_to_cycles(mixer, duration);
      }
      kr_mixer_ctl(mixer, unitname, controlname, floatval, duration, ptr);
      break;
    case EBML_ID_KRAD_MIXER_CMD_SET_EFFECT_CONTROL:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[5]);
      kr_ebml2_unpack_element_string(&ebml_in, &element, controlname, sizeof(controlname));
      kr_ebml2_unpack_element_float(&ebml_in, &element, &floatval);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[6]);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[7]);
      unit = kr_mixer_path_from_name(mixer, unitname);
      if (unit != NULL) {
        duration = numbers[6];
        if ((duration == 0) && (krad_app_server_current_client_is_subscriber(as))) {
          ptr = as->current_client;
        } else {
          duration = mixer_ms_to_cycles(mixer, duration);
        }
/*
        kr_sfx_effect_ctl(unit->sfx, numbers[0], numbers[5],
         kr_sfxeftctlstr(unit->sfx->effect[numbers[0]].type,
          controlname),
         floatval, duration, numbers[7], ptr);
*/
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_PUSH_TONE:
      tone = 0;
      kr_ebml2_unpack_element_int8(&ebml_in, &element, (int8_t *)&tone);
      //if (mixer->push_tone == -1) {
      //  mixer->push_tone = tone;
      //}
      break;
    case EBML_ID_KRAD_MIXER_CMD_PORTGROUP_XMMS2_CMD:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_element_string(&ebml_in, &element, string, sizeof(string));
      //kr_mixer_xmms_cmd(mixer, unitname, string);
      break;
    case EBML_ID_KRAD_MIXER_CMD_PLUG_PORTGROUP:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname2, sizeof(unitname2));
      //kr_mixer_plug(mixer, unitname, unitname2);
      break;
    case EBML_ID_KRAD_MIXER_CMD_UNPLUG_PORTGROUP:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname2, sizeof(unitname2));
      //kr_mixer_unplug(mixer, unitname, unitname2);
      break;
    case EBML_ID_KRAD_MIXER_CMD_BIND_PORTGROUP_XMMS2:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_element_string(&ebml_in, &element, string, sizeof(string));
      //kr_mixer_xmms_bind(mixer, unitname, string);
      break;
    case EBML_ID_KRAD_MIXER_CMD_UNBIND_PORTGROUP_XMMS2:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      //kr_mixer_xmms_unbind(mixer, unitname);
      break;
    case EBML_ID_KRAD_MIXER_CMD_LIST_PORTGROUPS:
      for (p = 0; p < KR_MXR_MAX_PATHS; p++) {
        unit = mixer->unit[p];
        if ((unit != NULL) && ((unit->active == 1) || (unit->active == 2))) {
          address.path.unit = KR_MIXER;
          address.path.subunit.mixer_subunit = KR_PORTGROUP;
          strcpy(address.id.name, unit->name);
          krad_radio_address_to_ebml2(&ebml_out, &response, &address);
          kr_ebml2_pack_uint32(&ebml_out,
                               EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                               EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          kr_mixer_unit_to_rep(unit, &unit_rep);
          kr_mixer_path_info_to_ebml(&unit_rep, &ebml_out);
          kr_ebml2_finish_element(&ebml_out, payload);
          kr_ebml2_finish_element(&ebml_out, response);
        }
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_CREATE_PORTGROUP:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_element_string(&ebml_in, &element, string, sizeof(string));
      kr_ebml2_unpack_element_string(&ebml_in, &element, string2, sizeof(string2));
      if (strncmp(string, "output", 6) == 0) {
        //direction = OUTPUT;
        //output_type = DIRECT;
      } else {
        if (strncmp(string, "auxout", 6) == 0) {
          //direction = OUTPUT;
          //output_type = AUX;
        } else {
          //direction = INPUT;
          //output_type = NOTOUTPUT;
        }
      }
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      if (strncmp(string2, "krad", 4) == 0) {
        krad_system_set_socket_blocking(as->current_client->sd);
        sd1 = krad_app_server_recvfd(as->current_client);
        sd2 = krad_app_server_recvfd(as->current_client);
        printk("AUDIOPORT_CREATE %s Got FD's %d and %d\n", string, sd1, sd2);
        //unit = kr_mixer_local_unit_create(mixer, string,
        // direction, sd1, sd2);
        krad_system_set_socket_nonblocking(as->current_client->sd);
      } else {
        //unit = kr_mixer_unit_create(mixer, unitname, direction, output_type,
        // numbers[0], 0.0f, mixer->master, KRAD_AUDIO, NULL, JACK);
      }
      if (unit != NULL) {
        address.path.unit = KR_MIXER;
        address.path.subunit.mixer_subunit = KR_PORTGROUP;
        strcpy(address.id.name, unit->name);
        krad_radio_broadcast_subunit_created(as->app_broadcaster, &address,
         (void *)unit);
        kr_mixer_ctl(mixer, unitname, "volume", 100.0f, 500, NULL);
      } else {
        printke("Krad Mixer: Failed to create unit: %s", unitname);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_DESTROY_PORTGROUP:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      unit = kr_mixer_path_from_name(mixer, unitname);
      if (unit != NULL) {
        kr_mixer_unlink(unit);
        address.path.unit = KR_MIXER;
        address.path.subunit.mixer_subunit = KR_PORTGROUP;
        strncpy(address.id.name, unitname, sizeof (address.id.name));
        krad_radio_broadcast_subunit_destroyed(as->app_broadcaster, &address);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_PORTGROUP_INFO:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      unit = kr_mixer_path_from_name(mixer, unitname);
      if ((unit != NULL) && ((unit->active == 1) || (unit->active == 2))) {
        address.path.unit = KR_MIXER;
        address.path.subunit.mixer_subunit = KR_PORTGROUP;
        strcpy(address.id.name, unit->name);
        krad_radio_address_to_ebml2(&ebml_out, &response, &address);
        kr_ebml2_pack_uint32(&ebml_out,
                             EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                             EBML_ID_KRAD_SUBUNIT_INFO);
        kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        kr_mixer_unit_to_rep(unit, &unit_rep);
        kr_mixer_path_info_to_ebml(&unit_rep, &ebml_out);
        kr_ebml2_finish_element(&ebml_out, payload);
        kr_ebml2_finish_element(&ebml_out, response);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_id(&ebml_in, &element, &size);
      if (element == EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE_NAME) {
        kr_ebml2_unpack_string(&ebml_in, string, size);
        unit = kr_mixer_path_from_name(mixer, unitname);
        if (unit != NULL) {
          if (unit->crossfader != NULL) {
            kr_mixer_xf_decouple(mixer, unit->crossfader);
            if (strlen(string) == 0) {
              return 0;
            }
          }
          if (strlen(string) > 0) {
            unit2 = kr_mixer_path_from_name(mixer, string);
            if (unit2 != NULL) {
              if (unit2->crossfader != NULL) {
                kr_mixer_xf_decouple(mixer, unit2->crossfader);
              }
              if (unit != unit2) {
                kr_mixer_xf_couple(mixer, unit, unit2);
              }
            }
          }
        }
      }
      if (element == EBML_ID_KRAD_MIXER_MAP_CHANNEL) {
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[1]);
        unit = kr_mixer_path_from_name(mixer, unitname);
        if (unit != NULL) {
          kr_mixer_channel_move(unit, numbers[0], numbers[1]);
        }
      }
      if (element == EBML_ID_KRAD_MIXER_MIXMAP_CHANNEL) {
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[1]);
        unit = kr_mixer_path_from_name(mixer, unitname);
        if (unit != NULL) {
          kr_mixer_channel_copy(unit, numbers[0], numbers[1]);
        }
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_GET_INFO:
      address.path.unit = KR_MIXER;
      address.path.subunit.mixer_subunit = KR_UNIT;
      krad_radio_address_to_ebml2(&ebml_out, &response, &address);
      kr_ebml2_pack_uint32(&ebml_out,
                           EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                           EBML_ID_KRAD_UNIT_INFO);
      kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      kr_mixer_to_ebml(&ebml_out, mixer);
      kr_ebml2_finish_element(&ebml_out, payload);
      kr_ebml2_finish_element(&ebml_out, response);
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
       (!krad_app_server_current_client_is_subscriber(as))) {
    krad_radio_pack_shipment_terminator(&ebml_out);
  }

  kr_io2_pulled(in, ebml_in.pos);
  kr_io2_advance(out, ebml_out.pos);

  return 0;
}
