static int handle_json(kr_iws_client_t *client, char *json, size_t len) {

  int pos;
  static const int shortest_json_len = 14;
  static const int longest_json_len = 220;
  static const char *json_pre_ctrl = "{\"ctrl\":\"";
  static const char *json_pre_xmms2 = "{\"xmms2\":\"";

  kr_unit_control_t uc;
  int cmplen;
  size_t addr_len;
  size_t dur_len;
  char *addr_str;
  
  size_t xmms2_len;
  char *xmms2_portgroup_name;
  char *xmms2_cmd;

  dur_len = 0;
  addr_len = 0;
  pos = 0;

  if (len < shortest_json_len) {
    printk("JSON IN is[%zu]: %s", len, json);
    printke("JSON json is too short");
    return -1;
  }
  if (len > longest_json_len) {
    printk("JSON IN is[%zu]: %s", len, json);
    printke("JSON json is too long");
    return -1;
  }
  if (json[len - 1] != '}') {
    printk("JSON IN is[%zu]: %s", len, json);
    printke("JSON json doesn't end well");
    return -1;
  }

  if ((strncmp(json, json_pre_ctrl, strlen(json_pre_ctrl)) != 0) 
 && (strncmp(json, json_pre_xmms2, strlen(json_pre_xmms2)) != 0)) {
    printk("JSON IN is[%zu]: %s", len, json);
    printke("JSON json seems to be in a bad way");
    return -1;
  }

  memset (&uc, 0, sizeof (uc));
  cmplen = strlen(json_pre_ctrl);
  pos = cmplen;

  if (strncmp(json, json_pre_ctrl, cmplen) == 0) {
    addr_len = strcspn(json + pos, " ");
    if (addr_len == 0) {
      printk("JSON IN is[%zu]: %s", len, json);
      printke("JSON json addr err");
      return -1;
    }
    json[pos + addr_len] = '\0';
    addr_str = json + pos;
    //printk("address string is: %s", addr_str);
    if (!(kr_string_to_address(addr_str, &uc.address))) {
      printke("Could not parse address");
      return -1;
    }
    pos += addr_len + 1;
    printk("rest is: %s", json + pos);
    if ((pos + 3) > len) {
      printke("could not find value part");
      return -1;
    }
    if (kr_unit_control_data_type_from_address(&uc.address, &uc.data_type) != 1) {
      printke("could not determine data type of control");
      return -1;
    }
    if (uc.data_type == KR_FLOAT) {
      uc.value.real = atof(json + pos);
    }
    if (uc.data_type == KR_INT32) {
      uc.value.integer = atoi(json + pos);
    }
    if (uc.data_type == KR_CHAR) {
      uc.value.byte = json[pos];
    }
    dur_len = strcspn(json + pos, " ");
    if (dur_len != 0) {
      /*printk("duration found: %s", json + pos + dur_len + 1);*/
      uc.duration = atoi(json + pos + dur_len + 1);
      /*printk("duration: %d", uc.duration);*/
    }
    if (kr_unit_control_set(client->ws.krclient, &uc) != 0) {
      printke("could not set control");
      return -1;
    }
  } else { 
       
    cmplen = strlen(json_pre_xmms2);
    if (strncmp(json, json_pre_xmms2, cmplen) == 0) {
      pos = cmplen;
      xmms2_len = strcspn(json + pos, " ");
      if (xmms2_len == 0) {
        printk("JSON IN is[%zu]: %s", len, json);
        printke("JSON json addr err");
        return -1;
      }
      json[pos + xmms2_len] = '\0';
      xmms2_portgroup_name = json + pos;

      pos += xmms2_len + 1;
      
      if ((pos + 3) > len) {
        printke("could not find xmms2 command");
        return -1;
      }

      xmms2_len = strcspn(json + pos, " ");
      if (xmms2_len == 0) {
        printk("JSON IN is[%zu]: %s", len, json);
        printke("JSON json addr err");
        return -1;
      }
      json[pos + xmms2_len] = '\0';
      xmms2_cmd = json + pos;

      kr_mixer_portgroup_xmms2_cmd (client->ws.krclient, xmms2_portgroup_name, xmms2_cmd);
    }
  }

  return 0;
}

void krad_websocket_add_portgroup ( kr_iws_client_t *client,
 kr_mixer_portgroup_t *portgroup) {

  int i;
  int pos;
  char json[2048];

  pos = 0;

  //for the moment will ignore these
  if ((portgroup->direction == OUTPUT) && (portgroup->output_type == DIRECT)) {
    return;
  }
  pos += snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"ctrl\":\"add_portgroup\",\"portgroup_name\":\"%s\","
   "\"volume\":%g,", portgroup->sysname, portgroup->volume[0]);
  if (portgroup->crossfade_group[0] != '\0') {
    pos += snprintf(json + pos, sizeof(json) - pos,
     "\"crossfade_name\":\"%s\",\"crossfade\":%g,",
     portgroup->crossfade_group, portgroup->fade);
  } else {
    pos += snprintf(json + pos, sizeof(json) - pos,
     "\"crossfade_name\":\"\",\"crossfade\":0,");
  }
  pos += snprintf(json + pos, sizeof(json) - pos,
   "\"xmms2\":%d,\"direction\":%d,",
   portgroup->has_xmms2, portgroup->direction);
  if (portgroup->direction == INPUT) {
    pos += snprintf(json + pos, sizeof(json) - pos, "\"eq\":{\"bands\":[");
    for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
      pos += snprintf(json + pos, sizeof(json) - pos,
       "{\"hz\":%g,\"db\":%g,\"bw\":%g},",
        portgroup->eq.band[i].hz, portgroup->eq.band[i].db,
        portgroup->eq.band[i].bandwidth);
    }
    pos--;
    pos += snprintf(json + pos, sizeof(json) - pos, "]},");
    pos += snprintf(json + pos, sizeof(json) - pos,
     "\"lowpass_hz\":%g,\"lowpass_bw\":%g,",
     portgroup->lowpass.hz, portgroup->lowpass.bandwidth);
    pos += snprintf(json + pos, sizeof(json) - pos,
     "\"highpass_hz\":%g,\"highpass_bw\":%g,",
     portgroup->highpass.hz, portgroup->highpass.bandwidth);
    pos += snprintf(json + pos, sizeof(json) - pos,
     "\"analog_drive\":%g,\"analog_blend\":%g",
     portgroup->analog.drive, portgroup->analog.blend);
  } else {
    pos--;
  }
  pos += snprintf(json + pos, sizeof(json) - pos, "}]");
  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_remove_portgroup (kr_iws_client_t *client,
 kr_address_t *address) {

  char json[192];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"ctrl\":\"remove_portgroup\",\"portgroup_name\":\"%s\"}]",
   address->id.name);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_portgroup_eff(kr_iws_client_t *client,
 kr_address_t *address, float value) {

  char json[256];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"ctrl\":\"effect_control\",\"portgroup_name\":\"%s\","
   "\"effect_name\":\"%s\",\"effect_num\":%d,"
   "\"control_name\":\"%s\","
   "\"value\":%g}]", address->id.name,
   effect_type_to_string (address->sub_id + 1), address->sub_id2,
   effect_control_to_string(address->control.effect_control), value);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_remove_subunit(kr_iws_client_t *client,
 kr_address_t *address) {

  char json[192];

  snprintf(json, sizeof(json), "[{\"com\":\"kradcompositor\","
   "\"ctrl\":\"remove_subunit\",\"subunit_type\":\"%s\","
   "\"subunit_id\":\"%d\"}]",
   kr_comp_strfsubtype(address->path.subunit.compositor_subunit),
   address->id.number);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_portgroup_control ( kr_iws_client_t *client,
 kr_address_t *address, float value) {

  char json[192];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"ctrl\":\"control_portgroup\",\"portgroup_name\":\"%s\",\"control_name\":"
   "\"%s\", \"value\":%g}]",
   address->id.name,
   portgroup_control_to_string(address->control.portgroup_control),
   value);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_portgroup_peak ( kr_iws_client_t *client,
 kr_address_t *address, float value) {

  char json[192];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"ctrl\":\"peak_portgroup\",\"portgroup_name\":\"%s\", \"value\":%g}]",
   address->id.name, value);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_update_portgroup ( kr_iws_client_t *client,
 kr_address_t *address, char *value ) {

  char json[192];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"ctrl\":\"update_portgroup\",\"portgroup_name\":\"%s\",\"control_name\":"
   "\"%s\", \"value\":\"%s\"}]",
   address->id.name,
   portgroup_control_to_string(address->control.portgroup_control),
   value);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_mixer(kr_iws_client_t *client, kr_mixer_t *mixer) {

  char json[96];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"ctrl\":\"set_mixer_params\",\"sample_rate\":\"%d\"}]",
   mixer->sample_rate);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_tag (kr_iws_client_t *client, char *tag_item,
 char *tag_name, char *tag_value) {

  char json[1024];

  snprintf(json, sizeof(json), "[{\"com\":\"kradradio\","
   "\"ctrl\":\"tag\",\"tag_item\":\"%s\",\"tag_name\":\"%s\","
   "\"tag_value\":\"%s\"}]",
   tag_item, tag_name, tag_value);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_cpu_usage (kr_iws_client_t *client, int usage) {

  char json[96];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"ctrl\":\"cpu\",\"system_cpu_usage\":\"%d\"}]",
   usage);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_add_comp_subunit(kr_iws_client_t *client,
 kr_crate_t *crate) {

  int pos;
  char json[2048];
  kr_address_t *address;
  kr_compositor_subunit_controls_t controls;

  address = crate->addr;
  pos = 0;

  pos += snprintf(json, sizeof(json), "[{\"com\":\"kradcompositor\","
   "\"ctrl\":\"add_subunit\",\"subunit_type\":\"%s\","
   "\"subunit_id\":%d,",
   kr_comp_strfsubtype(address->path.subunit.compositor_subunit),
   address->id.number);

  switch (address->path.subunit.compositor_subunit) {
    case KR_SPRITE:
      controls = crate->inside.sprite->controls;
      pos += snprintf(json + pos, sizeof(json) - pos,
       "\"filename\":\"%s\",\"rate\":%d,",
       crate->inside.sprite->filename, controls.tickrate);
      break;
    case KR_TEXT:
      controls = crate->inside.text->controls;
      pos += snprintf(json + pos, sizeof(json) - pos,
       "\"text\":\"%s\",\"font\":\"%s\",",
       crate->inside.text->text, crate->inside.text->font);
      pos += snprintf(json + pos, sizeof(json) - pos,
       "\"red\":%g,\"green\":%g,\"blue\":%g,",
       crate->inside.text->red, crate->inside.text->green,
       crate->inside.text->blue);
      break;
    case KR_VIDEOPORT:
      controls = crate->inside.videoport->controls;
      if (crate->inside.videoport->direction == OUTPUT) {
        pos += snprintf(json + pos, sizeof(json) - pos,
         "\"port_name\":\"%s\",\"direction\":\"%s\",",
         crate->inside.videoport->sysname, "output");
      } else {
        pos += snprintf(json + pos, sizeof(json) - pos,
         "\"port_name\":\"%s\",\"direction\":\"%s\",",
         crate->inside.videoport->sysname, "input");
      }
      break;
    case KR_VECTOR:
      controls = crate->inside.vector->controls;
      pos += snprintf(json + pos, sizeof(json) - pos,
       "\"type\":\"%s\",",
       krad_vector_type_to_string(crate->inside.vector->type));
      break;
  }

  pos += snprintf(json + pos, sizeof(json) - pos,
   "\"xscale\":%g,\"yscale\":%g,\"x\":%d,\"y\":%d,"
   "\"z\":%d,\"r\":%g,\"o\":%g,\"width\":%d,\"height\":%d",
   controls.xscale, controls.yscale, controls.x, controls.y, controls.z,
   controls.rotation, controls.opacity, controls.width, controls.height);

  pos += snprintf(json + pos, sizeof(json) - pos, "}]");
  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_update_subunit(kr_iws_client_t *client,
 kr_crate_t *crate) {

  int pos;
  char json[2048];
  kr_address_t *address;

  address = crate->addr;
  pos = 0;

  if (crate->addr->path.unit == KR_COMPOSITOR) {

    pos += snprintf(json + pos, sizeof(json) - pos,
     "[{\"com\":\"kradcompositor\","
     "\"ctrl\":\"update_subunit\",\"subunit_type\":\"%s\",\"subunit_id\":%d,"
     "\"control_name\":\"%s\",",
     kr_comp_strfsubtype(address->path.subunit.compositor_subunit),
     address->id.number,
     kr_compositor_control_to_string(address->control.compositor_control));

    if (kr_crate_contains_float (crate)) {
      pos += snprintf(json + pos, sizeof(json) - pos, "\"value\":%g",
       crate->real);
    } else {
      if (kr_crate_contains_int (crate)) {
        pos += snprintf(json + pos, sizeof(json) - pos, "\"value\":%d",
         crate->integer);
      }
    }

    pos += snprintf(json + pos, sizeof(json) - pos, "}]");
    interweb_ws_pack(client, (uint8_t *)json, strlen(json));
  }
}

static int crate_to_json(kr_iws_client_t *client, kr_crate_t *crate) {
  switch (crate->contains) {
    case 0:
      return 0;
    case KR_MIXER:
      krad_websocket_set_mixer(client, crate->inside.mixer);
      return 1;
    case KR_PORTGROUP:
      krad_websocket_add_portgroup(client, crate->inside.portgroup);
      return 1;
    case KR_SPRITE:
    case KR_VECTOR:
    case KR_TEXT:
    case KR_VIDEOPORT:
      krad_websocket_add_comp_subunit(client, crate);
      return 1;
  }
  return 0;
}

static int krad_delivery_handler (kr_iws_client_t *client) {

  kr_crate_t *crate;
  char *string;

  string = NULL;
  crate = NULL;

  kr_delivery_recv(client->ws.krclient);

  while ((kr_delivery_get (client->ws.krclient, &crate) > 0) &&
         (crate != NULL)) {

    /* Subunit updated */
    if (kr_crate_notice (crate) == EBML_ID_KRAD_SUBUNIT_CONTROL) {
      if (crate->addr->path.unit == KR_MIXER) {
        if (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP) {
          if (kr_crate_contains_float (crate)) {
            if (crate->addr->control.portgroup_control == KR_PEAK) {
              krad_websocket_set_portgroup_peak(client, crate->addr, crate->real);
            } else {
              krad_websocket_set_portgroup_control(client, crate->addr, crate->real);
            }
          } else {
            if ((crate->addr->control.portgroup_control == KR_CROSSFADE_GROUP) ||
                (crate->addr->control.portgroup_control == KR_XMMS2_IPC_PATH)) {
              if (kr_uncrate_string (crate, &string)) {
                krad_websocket_update_portgroup(client, crate->addr, string);
                kr_string_recycle (&string);
              } else {
                krad_websocket_update_portgroup(client, crate->addr, "");
              }
            }
          }
        }
        if (crate->addr->path.subunit.mixer_subunit == KR_EFFECT) {
          if (kr_crate_contains_float (crate)) {
            krad_websocket_set_portgroup_eff(client, crate->addr, crate->real);
          }
        }
      }
      if (crate->addr->path.unit == KR_COMPOSITOR) {
        krad_websocket_update_subunit(client, crate);
      }
      kr_crate_recycle (&crate);
      continue;
    }
    /* Subunit Destroyed */
    if (kr_crate_notice (crate) == EBML_ID_KRAD_RADIO_UNIT_DESTROYED) {
      if ((crate->addr->path.unit == KR_MIXER) &&
          (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP)) {
        krad_websocket_remove_portgroup(client, crate->addr);
      }
      if (crate->addr->path.unit == KR_COMPOSITOR) {
        krad_websocket_remove_subunit(client, crate->addr);
      }
      kr_crate_recycle (&crate);
      continue;
    }
    /* Initial list of subunits or subunit created */
    if (kr_crate_loaded(crate)) {
      crate_to_json(client, crate);
      kr_crate_recycle (&crate);
      continue;
    }
  }

  return 0;
}
