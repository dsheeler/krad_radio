static void json_to_cmd(kr_iws_client_t *client, char *value) {
  
  kr_unit_control_t uc;
  int sub_id;
  float floatval;
  cJSON *cmd;
  cJSON *part;
  cJSON *part2;
  cJSON *part3;
  cJSON *part4;
  cJSON *part5;

  sub_id = 0;
  floatval = 0;

  part = NULL;
  part2 = NULL;
  part3 = NULL;
  part4 = NULL;
  part5 = NULL;

  cmd = cJSON_Parse(value);
  
  if (!cmd) {
    printke ("Krad Interweb JSON Error: [%s]", cJSON_GetErrorPtr());
    return;
  }

  part = cJSON_GetObjectItem (cmd, "com");
  
  if ((part != NULL) && (strcmp(part->valuestring, "kradmixer") == 0)) {
    part = cJSON_GetObjectItem (cmd, "cmd");
    if ((part != NULL) && (strcmp(part->valuestring, "remove_portgroup") == 0)) {
      part2 = cJSON_GetObjectItem (cmd, "portgroup_name");
      memset (&uc, 0, sizeof (uc));
      uc.address.path.unit = KR_MIXER;
      uc.address.path.subunit.mixer_subunit = KR_PORTGROUP;
      strncpy (uc.address.id.name, part2->valuestring, sizeof(uc.address.id.name));
      kr_unit_destroy (client->ws.krclient, &uc.address);
      return;
    }      
    if ((part != NULL) && (strcmp(part->valuestring, "add_portgroup") == 0)) {
      part2 = cJSON_GetObjectItem (cmd, "portgroup_name");
      part3 = cJSON_GetObjectItem (cmd, "portgroup_direction");
      kr_mixer_create_portgroup (client->ws.krclient, part2->valuestring, part3->valuestring, 2);
      return;
    }
    if ((part != NULL) && (strcmp(part->valuestring, "update_portgroup") == 0)) {
      part = cJSON_GetObjectItem (cmd, "portgroup_name");
      part2 = cJSON_GetObjectItem (cmd, "control_name");
      part3 = cJSON_GetObjectItem (cmd, "value");
      if ((part != NULL) && (part2 != NULL) && (part3 != NULL)) {
        if (strcmp(part2->valuestring, "xmms2") == 0) {
          kr_mixer_portgroup_xmms2_cmd (client->ws.krclient, part->valuestring, part3->valuestring);
        } else {
          if (strcmp(part2->valuestring, "set_crossfade_group") == 0) {
            kr_mixer_set_portgroup_crossfade_group (client->ws.krclient, part->valuestring, part3->valuestring);
          } else {
            floatval = part3->valuefloat;
            kr_mixer_set_control (client->ws.krclient, part->valuestring, part2->valuestring, floatval, 0);
          }
        }
      }
      return;
    }
    if ((part != NULL) && (strcmp(part->valuestring, "control_effect") == 0)) {
      part = cJSON_GetObjectItem (cmd, "portgroup_name");
      part2 = cJSON_GetObjectItem (cmd, "effect_name");
      part3 = cJSON_GetObjectItem (cmd, "control_name");
      part4 = cJSON_GetObjectItem (cmd, "value");
      if ((part != NULL) && (part2 != NULL) && (part3 != NULL) && (part4 != NULL)) {
        floatval = part4->valuefloat;
        if (part2->valuestring[0] == 'e') {
          sub_id = 0;
        } else {
          if (part2->valuestring[0] == 'l') {
            sub_id = 1;
          } else {
            if (part2->valuestring[0] == 'h') {
              sub_id = 2;
            } else {
              if (part2->valuestring[0] == 'a') {
                sub_id = 3;
              }
            }
          }
        }
        kr_mixer_set_effect_control (client->ws.krclient, part->valuestring, sub_id, 0,
                                     part3->valuestring,
                                     floatval, 0, EASEINOUTSINE);
      }
      return;
    }
    
    if ((part != NULL) && (strcmp(part->valuestring, "control_eq_effect") == 0)) {
      part = cJSON_GetObjectItem (cmd, "portgroup_name");
      part2 = cJSON_GetObjectItem (cmd, "effect_name");
      part3 = cJSON_GetObjectItem (cmd, "effect_num");
      part4 = cJSON_GetObjectItem (cmd, "control_name");
      part5 = cJSON_GetObjectItem (cmd, "value");
      if ((part != NULL) && (part2 != NULL) && (part3 != NULL) && (part4 != NULL) && (part5 != NULL)) {
        floatval = part5->valuefloat;
        if (part2->valuestring[0] == 'e') {
          sub_id = 0;
        } else {
          if (part2->valuestring[0] == 'l') {
            sub_id = 1;
          } else {
            if (part2->valuestring[0] == 'h') {
              sub_id = 2;
            } else {
              if (part2->valuestring[0] == 'a') {
                sub_id = 3;
              }
            }
          }
        }
        kr_mixer_set_effect_control (client->ws.krclient, part->valuestring, sub_id, part3->valueint,
                                     part4->valuestring,
                                     floatval, 0, EASEINOUTSINE);
      }
      return;
    }
    
    if ((part != NULL) && (strcmp(part->valuestring, "push_dtmf") == 0)) {
      part = cJSON_GetObjectItem (cmd, "dtmf");
      if (part != NULL) {
        kr_mixer_push_tone (client->ws.krclient, part->valuestring);
      }
    }
    return;
  }
  
  if ((part != NULL) && (strcmp(part->valuestring, "kradcompositor") == 0)) {
    part = cJSON_GetObjectItem (cmd, "cmd");
    if ((part != NULL) && (strcmp(part->valuestring, "jsnap") == 0)) {
      kr_compositor_snapshot_jpeg (client->ws.krclient);
    }  
    if ((part != NULL) && (strcmp(part->valuestring, "snap") == 0)) {
      kr_compositor_snapshot (client->ws.krclient);
    }

    if ((part != NULL) && (strcmp(part->valuestring, "display") == 0)) {
       kr_transponder_subunit_create (client->ws.krclient, "rawout", "");
    }

    if ((part != NULL) && (strcmp(part->valuestring, "remove_subunit") == 0)) {
      memset (&uc, 0, sizeof (uc));
      part2 = cJSON_GetObjectItem (cmd, "subunit_id");
      part3 = cJSON_GetObjectItem (cmd, "subunit_type");
      uc.address.path.unit = KR_COMPOSITOR;
      uc.address.path.subunit.compositor_subunit = kr_string_to_comp_subunit_type (part3->valuestring);
      uc.address.id.number = part2->valueint;
      kr_unit_destroy (client->ws.krclient, &uc.address);
      return;
    }      

    if ((part != NULL) && (strcmp(part->valuestring, "update_subunit") == 0)) {
      part = cJSON_GetObjectItem (cmd, "subunit_id");
      part2 = cJSON_GetObjectItem (cmd, "subunit_type");
      part3 = cJSON_GetObjectItem (cmd, "control_name");
      part4 = cJSON_GetObjectItem (cmd, "value");
      if ((part != NULL) && (part2 != NULL) && (part3 != NULL) && (part4 != NULL)) {
        memset (&uc, 0, sizeof(uc));
        uc.address.path.unit = KR_COMPOSITOR;
        uc.address.path.subunit.compositor_subunit = kr_string_to_comp_subunit_type (part2->valuestring);
        uc.address.id.number = part->valueint;
        uc.address.control.compositor_control = krad_string_to_compositor_control (part3->valuestring);
        if ((uc.address.control.compositor_control == KR_OPACITY) || (uc.address.control.compositor_control == KR_ROTATION) ||
            (uc.address.control.compositor_control == KR_YSCALE) || (uc.address.control.compositor_control == KR_XSCALE)) {
          uc.value.real = part4->valuefloat;
        } else {
          uc.value.integer = part4->valueint;
        }
        kr_unit_control_set (client->ws.krclient, &uc);
      }
      return;
    }
    if ((part != NULL) && (strcmp(part->valuestring, "add_subunit") == 0)) {
      part2 = cJSON_GetObjectItem (cmd, "subunit_type");
      memset (&uc, 0, sizeof(uc));
      uc.address.path.unit = KR_COMPOSITOR;
      uc.address.path.subunit.compositor_subunit = kr_string_to_comp_subunit_type (part2->valuestring);
      if (uc.address.path.subunit.compositor_subunit == KR_SPRITE) {
        part3 = cJSON_GetObjectItem (cmd, "filename");
        kr_compositor_subunit_create (client->ws.krclient, KR_SPRITE, part3->valuestring, NULL);
      }
      if (uc.address.path.subunit.compositor_subunit == KR_TEXT) {
        part3 = cJSON_GetObjectItem (cmd, "text");
        part4 = cJSON_GetObjectItem (cmd, "font");
        kr_compositor_subunit_create (client->ws.krclient, KR_TEXT, part3->valuestring, part4->valuestring);
      }
      if (uc.address.path.subunit.compositor_subunit == KR_VECTOR) {
        part3 = cJSON_GetObjectItem (cmd, "vector_type");
        kr_compositor_subunit_create (client->ws.krclient, KR_VECTOR, part3->valuestring, NULL);
      }
    }
    return;
  }
 
  if ((part != NULL) && (strcmp(part->valuestring, "kradradio") == 0)) {
    part = cJSON_GetObjectItem (cmd, "cmd");    
    if ((part != NULL) && (strcmp(part->valuestring, "stag") == 0)) {
      part2 = cJSON_GetObjectItem (cmd, "tag_name");
      part3 = cJSON_GetObjectItem (cmd, "tag_value");
      if ((part != NULL) && (part2 != NULL) && (part3 != NULL)) {      
        kr_set_tag (client->ws.krclient, NULL, part2->valuestring, part3->valuestring);
        //printk("aye got %s %s", part2->valuestring, part3->valuestring);
      }
    }
    return;
  }
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
   "\"cmd\":\"add_portgroup\",\"portgroup_name\":\"%s\","
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
     portgroup->lowpass.hz, portgroup->lowpass.bandwidth);
    pos += snprintf(json + pos, sizeof(json) - pos,
     "\"analog_drive\":%g,\"analog_blend\":%g",
     portgroup->lowpass.hz, portgroup->lowpass.bandwidth);
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
   "\"cmd\":\"remove_portgroup\",\"portgroup_name\":\"%s\"}]",
   address->id.name);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_portgroup_eff(kr_iws_client_t *client,
 kr_address_t *address, float value) {

  char json[256];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"cmd\":\"effect_control\",\"portgroup_name\":\"%s\","
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
   "\"cmd\":\"remove_subunit\",\"subunit_type\":\"%s\","
   "\"subunit_id\":\"%d\"}]",
   kr_comp_strfsubtype(address->path.subunit.compositor_subunit),
   address->id.number);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_portgroup_control ( kr_iws_client_t *client,
 kr_address_t *address, float value) {

  char json[192];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"cmd\":\"control_portgroup\",\"portgroup_name\":\"%s\",\"control_name\":"
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
   "\"cmd\":\"peak_portgroup\",\"portgroup_name\":\"%s\", \"value\":%g}]",
   address->id.name, value);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_update_portgroup ( kr_iws_client_t *client,
 kr_address_t *address, char *value ) {

  char json[192];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"cmd\":\"update_portgroup\",\"portgroup_name\":\"%s\",\"control_name\":"
   "\"%s\", \"value\":\"%s\"}]",
   address->id.name,
   portgroup_control_to_string(address->control.portgroup_control),
   value);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_mixer(kr_iws_client_t *client, kr_mixer_t *mixer) {

  char json[96];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"cmd\":\"set_mixer_params\",\"sample_rate\":\"%d\"}]",
   mixer->sample_rate);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_tag (kr_iws_client_t *client, char *tag_item,
 char *tag_name, char *tag_value) {

  char json[1024];

  snprintf(json, sizeof(json), "[{\"com\":\"kradradio\","
   "\"info\":\"tag\",\"tag_item\":\"%s\",\"tag_name\":\"%s\","
   "\"tag_value\":\"%s\"}]",
   tag_item, tag_name, tag_value);

  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void krad_websocket_set_cpu_usage (kr_iws_client_t *client, int usage) {

  char json[96];

  snprintf(json, sizeof(json), "[{\"com\":\"kradmixer\","
   "\"cmd\":\"cpu\",\"system_cpu_usage\":\"%d\"}]",
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
   "\"cmd\":\"add_subunit\",\"subunit_type\":\"%s\","
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
     "\"cmd\":\"update_subunit\",\"subunit_type\":\"%s\",\"subunit_id\":%d,"
     "\"control_name\":\"%s\",",
     kr_comp_strfsubtype(address->path.subunit.compositor_subunit),
     address->id.number,
     kr_compositor_control_to_string(address->control.compositor_control));

    if (kr_crate_contains_float (crate)) {
      pos += snprintf(json + pos, sizeof(json) - pos, "\"value\":%g}]",
       crate->real);
    } else {
      if (kr_crate_contains_int (crate)) {
        pos += snprintf(json + pos, sizeof(json) - pos, "\"value\":%d}]",
         crate->integer);
      }
    }

    pos += snprintf(json + pos, sizeof(json) - pos, "}]");
    interweb_ws_pack(client, (uint8_t *)json, strlen(json));
  }
}

/* Krad API Handler */

static void handle_json_cmd(kr_iws_client_t *client, char *value) {
  json_to_cmd(client, value);
  cjson_memreset();
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
