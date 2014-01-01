#include "krad_mixer_interface.h"

int kr_mixer_get_path_info(kr_mixer_path *unit, kr_mixer_path_info *info) {
  int i;
  if ((unit == NULL) || (info == NULL)) return -1;
  strcpy(info->name, unit->name);
  info->channels = unit->channels;
  if (unit->bus != NULL) {
    strncpy(info->bus, unit->bus->name, sizeof(info->bus));
  } else {
    info->bus[0] = '\0';
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    info->volume[i] = unit->volume[i];
    info->map[i] = unit->map[i];
    info->mixmap[i] = unit->mixmap[i];
    info->rms[i] = unit->avg[i];
    info->peak[i] = unit->peak_last[i];
  }
  kr_sfx_effect_info(unit->sfx, 0, &info->eq);
  kr_sfx_effect_info(unit->sfx, 1, &info->lowpass);
  kr_sfx_effect_info(unit->sfx, 2, &info->highpass);
  kr_sfx_effect_info(unit->sfx, 3, &info->analog);
  if ((unit->crossfader != NULL) && (unit->crossfader->path[0] == unit)) {
    info->fade = unit->crossfader->fade;
    strncpy(info->crossfade_group, unit->crossfader->path[1]->name,
     sizeof(info->crossfade_group));
  } else {
    info->crossfade_group[0] = '\0';
    info->fade = 0.0f;
  }
  return 0;
}

int kr_mixer_command(kr_io2_t *in, kr_io2_t *out, kr_radio_client *client) {
  kr_mixer_info mixer_info;
  kr_mixer_path *path;
  kr_mixer_path *unit;
  kr_mixer_path *unit2;
  kr_mixer_path_info info;
  char name[64];
  char control[16];
  void *ptr;
  float floatval;
  int i;
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
  uint32_t numbers[10];
  kr_app_server *as;
  int duration;
  i = 0;
  duration = 0;
  ptr = NULL;
  radio = client->krad_radio;
  mixer = radio->mixer;
  as = radio->app;
  path = NULL;
  name[0] = '\0';
  control[0] = '\0';
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
      kr_ebml2_unpack_element_string(&ebml_in, &element, name, sizeof(name));
      kr_ebml2_unpack_element_string(&ebml_in, &element, control, sizeof(control));
      kr_ebml2_unpack_element_float(&ebml_in, &element, &floatval);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      duration = numbers[0];
      if ((duration == 0) && (krad_app_server_current_client_is_subscriber(as))) {
        ptr = as->current_client;
      }
      path = kr_mixer_find(mixer, name);
      if (path) {
        kr_mixer_path_ctl(path, control, floatval, duration, ptr);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_SET_EFFECT_CONTROL:
      kr_ebml2_unpack_element_string(&ebml_in, &element, name, sizeof(name));
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[5]);
      kr_ebml2_unpack_element_string(&ebml_in, &element, control, sizeof(control));
      kr_ebml2_unpack_element_float(&ebml_in, &element, &floatval);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[6]);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[7]);
      path = kr_mixer_find(mixer, name);
      if (path) {
        duration = numbers[6];
        if ((duration == 0) && (krad_app_server_current_client_is_subscriber(as))) {
          ptr = as->current_client;
        }
        kr_sfx_effect_ctl(path->sfx, numbers[0], numbers[5],
         /*kr_sfxeftctlstr(path->sfx->effect[numbers[0]].type, control),*/
         control,
         floatval, duration, numbers[7], ptr);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_LIST_PORTGROUPS:
      address.path.unit = KR_MIXER;
      address.path.subunit.mixer_subunit = KR_PORTGROUP;
      while ((path = kr_mixer_path_iter(mixer, &i))) {
        strcpy(address.id.name, path->name);
        krad_radio_address_to_ebml2(&ebml_out, &response, &address);
        kr_ebml_pack_uint32(&ebml_out,
                             EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                             EBML_ID_KRAD_SUBUNIT_INFO);
        kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        kr_mixer_get_path_info(path, &info);
        kr_mixer_path_info_to_ebml(&ebml_out, (void *)&info);
        kr_ebml2_finish_element(&ebml_out, payload);
        kr_ebml2_finish_element(&ebml_out, response);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_PORTGROUP_INFO:
      kr_ebml2_unpack_element_string(&ebml_in, &element, name, sizeof(name));
      unit = kr_mixer_find(mixer, name);
      if ((unit != NULL) && ((unit->state == 1) || (unit->state == 2))) {
        address.path.unit = KR_MIXER;
        address.path.subunit.mixer_subunit = KR_PORTGROUP;
        strcpy(address.id.name, unit->name);
        krad_radio_address_to_ebml2(&ebml_out, &response, &address);
        kr_ebml_pack_uint32(&ebml_out,
                             EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                             EBML_ID_KRAD_SUBUNIT_INFO);
        kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        kr_mixer_get_path_info(unit, &info);
        kr_mixer_path_info_to_ebml(&ebml_out, (void *)&info); /* new generated call */
        kr_ebml2_finish_element(&ebml_out, payload);
        kr_ebml2_finish_element(&ebml_out, response);
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_UPDATE_PORTGROUP:
      kr_ebml2_unpack_element_string(&ebml_in, &element, name, sizeof(name));
      kr_ebml2_unpack_id(&ebml_in, &element, &size);
      if (element == EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE_NAME) {
        kr_ebml2_unpack_string(&ebml_in, string, size);
        unit = kr_mixer_find(mixer, name);
        if (unit != NULL) {
          if (unit->crossfader != NULL) {
            kr_mixer_xf_decouple(mixer, unit->crossfader);
            if (strlen(string) == 0) {
              return 0;
            }
          }
          if (strlen(string) > 0) {
            unit2 = kr_mixer_find(mixer, string);
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
        unit = kr_mixer_find(mixer, name);
        if (unit != NULL) {
          kr_mixer_channel_move(unit, numbers[0], numbers[1]);
        }
      }
      if (element == EBML_ID_KRAD_MIXER_MIXMAP_CHANNEL) {
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
        kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[1]);
        unit = kr_mixer_find(mixer, name);
        if (unit != NULL) {
          kr_mixer_channel_copy(unit, numbers[0], numbers[1]);
        }
      }
      break;
    case EBML_ID_KRAD_MIXER_CMD_GET_INFO:
      address.path.unit = KR_MIXER;
      address.path.subunit.mixer_subunit = KR_UNIT;
      krad_radio_address_to_ebml2(&ebml_out, &response, &address);
      kr_ebml_pack_uint32(&ebml_out,
                           EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                           EBML_ID_KRAD_UNIT_INFO);
      kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      kr_mixer_get_info(mixer, &mixer_info);
      kr_mixer_info_to_ebml(&ebml_out, (void *)&mixer_info);
      kr_ebml2_finish_element(&ebml_out, payload);
      kr_ebml2_finish_element(&ebml_out, response);
      break;
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
