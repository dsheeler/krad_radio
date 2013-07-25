#include "krad_mixer_interface.h"

void kr_mixer_unit_to_rep(kr_mixer_unit *unit, kr_mxr_unit_rep *unit_rep) {

  int i;
  kr_eq *eq;
  kr_pass *lowpass;
  kr_pass *highpass;
  kr_analog *analog;

  strcpy(unit_rep->name, unit->name);
//  unit_rep->channels = unit->channels;
//  unit_rep->direction = unit->direction;
//  unit_rep->output_type = unit->output_type;
//  unit_rep->io_type = unit->io_type;

  if (unit->bus != NULL) {
    strncpy(unit_rep->bus, unit->bus->name, sizeof(unit_rep->bus));
  } else {
    unit_rep->bus[0] = '\0';
  }

  for (i = 0; i < KRAD_MIXER_MAX_CHANNELS; i++) {
    unit_rep->volume[i] = unit->volume[i];
    unit_rep->map[i] = unit->map[i];
    unit_rep->mixmap[i] = unit->mixmap[i];
    unit_rep->rms[i] = unit->avg[i];
    unit_rep->peak[i] = unit->peak_last[i];
  }

  if (unit->xmms != NULL) {
    unit_rep->has_xmms2 = 1;
    strncpy(unit_rep->xmms2_ipc_path, unit->xmms->ipc_path,
     sizeof(unit_rep->xmms2_ipc_path));
  } else {
    unit_rep->has_xmms2 = 0;
    unit_rep->xmms2_ipc_path[0] = '\0';
  }

  if (unit->direction == INPUT) {
    eq = (kr_eq *)unit->effects->effect[0].effect[0];
    lowpass = (kr_lowpass *)unit->effects->effect[1].effect[0];
    highpass = (kr_highpass *)unit->effects->effect[2].effect[0];
    analog = (kr_analog *)unit->effects->effect[3].effect[0];
    for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
      unit_rep->eq.band[i].db = eq->band[i].db;
      unit_rep->eq.band[i].bandwidth = eq->band[i].bandwidth;
      unit_rep->eq.band[i].hz = eq->band[i].hz;
    }
    unit_rep->lowpass.hz = lowpass->hz;
    unit_rep->lowpass.bandwidth = lowpass->bandwidth;
    unit_rep->highpass.hz = highpass->hz;
    unit_rep->highpass.bandwidth = highpass->bandwidth;
    unit_rep->analog.drive = analog->drive;
    unit_rep->analog.blend = analog->blend;
  } else {
    for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
      unit_rep->eq.band[i].db = 0.0f;
      unit_rep->eq.band[i].bandwidth = 0.0f;
      unit_rep->eq.band[i].hz = 0.0f;
    }
    unit_rep->lowpass.hz = 0.0f;
    unit_rep->lowpass.bandwidth = 0.0f;
    unit_rep->highpass.hz = 0.0f;
    unit_rep->highpass.bandwidth = 0.0f;
    unit_rep->analog.drive = 0.0f;
    unit_rep->analog.blend = 0.0f;
  }
  if ((unit->crossfader != NULL) && (unit->crossfader->unit[0] == unit)) {
    unit_rep->fade = unit->crossfader->fade;
    strncpy(unit_rep->crossfade_group, unit->crossfader->unit[1]->name,
     sizeof(unit_rep->crossfade_group));
  } else {
    unit_rep->crossfade_group[0] = '\0';
    unit_rep->fade = 0.0f;
  }
}

void kr_mixer_to_rep(kr_mixer *mixer, kr_mixer_t *mixer_rep) {

  kr_mixer_unit *unit;
  int p;

  p = 0;
  unit = NULL;

  for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
    unit = mixer->unit[p];
    if ((unit != NULL) && ((unit->active == 1) || (unit->active == 2))) {
      if (unit->direction == INPUT) {
        mixer_rep->inputs++;
      }
      if (unit->direction == OUTPUT) {
        mixer_rep->outputs++;
      }
      if (unit->direction == MIX) {
        mixer_rep->buses++;
      }
    }
  }

  mixer_rep->period_size = kr_mixer_period(mixer);
  mixer_rep->sample_rate = kr_mixer_sample_rate(mixer);

  if (mixer->pusher == JACK) {
    strncpy(mixer_rep->time_source, "Jack", sizeof(mixer_rep->time_source));
  } else {
    strncpy(mixer_rep->time_source, "Internal Chronometer",
     sizeof(mixer_rep->time_source));
  }
}

void kr_mixer_rep_to_ebml(kr_ebml2_t *ebml, kr_mixer_t *mxr) {
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_SAMPLE_RATE, mxr->period_size);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_SAMPLE_RATE, mxr->sample_rate);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_COUNT, mxr->inputs);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_COUNT, mxr->outputs);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_COUNT, mxr->buses);
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_MIXER_TIME_SOURCE, mxr->time_source);
}

void kr_mixer_to_ebml(kr_ebml2_t *ebml, kr_mixer *mixer) {
  kr_mixer_t mixer_rep;
  memset(&mixer_rep, 0, sizeof(kr_mixer_t));
  kr_mixer_to_rep(mixer, &mixer_rep);
  kr_mixer_rep_to_ebml(ebml, &mixer_rep);
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

int kr_mixer_command(kr_io2_t *in, kr_io2_t *out, kr_radio_client *client) {

  kr_mixer_unit *unit;
  kr_mixer_unit *unit2;
  kr_mxr_unit unit_rep;
  kr_mixer_output_t output_type;
  char unitname[64];
  char unitname2[64];
  char controlname[16];
  void *ptr;
  float floatval;
  int direction;
  int p;
  kr_radio *radio;
  kr_mixer *mixer;
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
  direction = 0;
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
        duration = ms_to_cycles(mixer->sample_rate, mixer->period_size, duration);
      }
      kr_mixer_control(mixer, unitname, controlname, floatval, duration, ptr);
      break;
    case EBML_ID_KRAD_MIXER_CMD_SET_EFFECT_CONTROL:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[5]);
      kr_ebml2_unpack_element_string(&ebml_in, &element, controlname, sizeof(controlname));
      kr_ebml2_unpack_element_float(&ebml_in, &element, &floatval);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[6]);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[7]);
      unit = kr_mixer_unit_from_name(mixer, unitname);
      if (unit != NULL) {
        duration = numbers[6];
        if ((duration == 0) && (krad_app_server_current_client_is_subscriber(as))) {
          ptr = as->current_client;
        } else {
          duration = ms_to_cycles(mixer->sample_rate, mixer->period_size, duration);
        }
        kr_effects_control(unit->effects, numbers[0], numbers[5],
         kr_effects_strtoctrl(unit->effects->effect[numbers[0]].effect_type,
          controlname),
         floatval, duration, numbers[7], ptr);
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
      for (p = 0; p < KRAD_MIXER_MAX_PORTGROUPS; p++) {
        unit = mixer->unit[p];
        if ((unit != NULL) && ((unit->active == 1) || (unit->active == 2))) {
          krad_radio_address_to_ebml2(&ebml_out, &response, &unit->address);
          kr_ebml2_pack_uint32(&ebml_out,
                               EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                               EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          kr_mixer_unit_to_rep(unit, &unit_rep);
          kr_mixer_unit_rep_to_ebml(&unit_rep, &ebml_out);
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
        krad_radio_broadcast_subunit_created(as->app_broadcaster,
         &unit->address, (void *)unit);
        kr_mixer_control(mixer, unitname, "volume", 100.0f, 500, NULL);
      } else {
        printke("Krad Mixer: Failed to create unit: %s", unitname);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_DESTROY_PORTGROUP:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      unit = kr_mixer_unit_from_name(mixer, unitname);
      if (unit != NULL) {
        kr_mixer_unit_destroy(mixer, unit);
        address.path.unit = KR_MIXER;
        address.path.subunit.mixer_subunit = KR_PORTGROUP;
        strncpy(address.id.name, unitname, sizeof (address.id.name));
        krad_radio_broadcast_subunit_destroyed(as->app_broadcaster, &address);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_PORTGROUP_INFO:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      unit = kr_mixer_unit_from_name(mixer, unitname);
      if ((unit != NULL) && ((unit->active == 1) || (unit->active == 2))) {
        krad_radio_address_to_ebml2(&ebml_out, &response, &unit->address);
        kr_ebml2_pack_uint32(&ebml_out,
                             EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                             EBML_ID_KRAD_SUBUNIT_INFO);
        kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        kr_mixer_unit_to_rep(unit, &unit_rep);
        kr_mixer_unit_rep_to_ebml(&unit_rep, &ebml_out);
        kr_ebml2_finish_element(&ebml_out, payload);
        kr_ebml2_finish_element(&ebml_out, response);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP:
      kr_ebml2_unpack_element_string(&ebml_in, &element, unitname, sizeof(unitname));
      kr_ebml2_unpack_id(&ebml_in, &element, &size);
      if (element == EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE_NAME) {
        kr_ebml2_unpack_string(&ebml_in, string, size);
        unit = kr_mixer_unit_from_name(mixer, unitname);
        if (unit != NULL) {
          if (unit->crossfader != NULL) {
            kr_mixer_cf_detatch(mixer, unit->crossfader);
            if (strlen(string) == 0) {
              return 0;
            }
          }
          if (strlen(string) > 0) {
            unit2 = kr_mixer_unit_from_name(mixer, string);
            if (unit2 != NULL) {
              if (unit2->crossfader != NULL) {
                kr_mixer_cf_detatch(mixer, unit2->crossfader);
              }
              if (unit != unit2) {
                kr_mixer_cf_attach(mixer, unit, unit2);
              }
            }
          }
        }
      }
      if (element == EBML_ID_KRAD_MIXER_MAP_CHANNEL) {
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[1]);
        unit = kr_mixer_unit_from_name(mixer, unitname);
        if (unit != NULL) {
          kr_mixer_channel_move(unit, numbers[0], numbers[1]);
        }
      }
      if (element == EBML_ID_KRAD_MIXER_MIXMAP_CHANNEL) {
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[1]);
        unit = kr_mixer_unit_from_name(mixer, unitname);
        if (unit != NULL) {
          kr_mixer_channel_copy(unit, numbers[0], numbers[1]);
        }
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_GET_INFO:
      krad_radio_address_to_ebml2(&ebml_out, &response, &mixer->address);
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
