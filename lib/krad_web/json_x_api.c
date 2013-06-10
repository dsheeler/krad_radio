void interweb_json_pack (kr_iws_client_t *client) {

  int32_t pos;
  char *buffer;
  char *str;
  int32_t json_len;

  pos = 0;
  buffer = (char *)client->out->buf;

  str = cJSON_Print (client->ws.json);
  json_len = strlen(str);
  if (json_len > 0) {
    pos += interweb_ws_pack_frame_header((uint8_t *)buffer, json_len);
    memcpy(buffer + pos, str, json_len);
    pos += json_len;
    kr_io2_advance (client->out, pos);

    cjson_memreset ();

    client->ws.json = cJSON_CreateArray();
  }
}

static void json_to_cmd (kr_iws_client_t *client, char *value) {
  
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

  cmd = cJSON_Parse (value);
  
  if (!cmd) {
    printke ("Krad WebSocket: JSON Error before: [%s]", cJSON_GetErrorPtr());
  } else {

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
    //cjson_memreset ();
  }
}

/* callbacks from api handler to add JSON to websocket message */

void krad_websocket_set_tag (kr_iws_client_t *client, char *tag_item, char *tag_name, char *tag_value) {

  cJSON *msg;
  
  cJSON_AddItemToArray(client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradradio");
  cJSON_AddStringToObject (msg, "info", "tag");
  cJSON_AddStringToObject (msg, "tag_item", tag_item);
  cJSON_AddStringToObject (msg, "tag_name", tag_name);
  cJSON_AddStringToObject (msg, "tag_value", tag_value);

}

void krad_websocket_set_cpu_usage (kr_iws_client_t *client, int usage) {

  cJSON *msg;
  
  cJSON_AddItemToArray (client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradradio");
  cJSON_AddStringToObject (msg, "info", "cpu");
  cJSON_AddNumberToObject (msg, "system_cpu_usage", usage);

}

void krad_websocket_add_portgroup ( kr_iws_client_t *client, kr_mixer_portgroup_t *portgroup) {

  int i;
  cJSON *msg;
  cJSON *eq;
  cJSON *eqbands;
  cJSON *eqband;

  //for the moment will ignore these
  if ((portgroup->direction == OUTPUT) && (portgroup->output_type == DIRECT)) {
    return;
  }

  cJSON_AddItemToArray(client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "add_portgroup");
  cJSON_AddStringToObject (msg, "portgroup_name", portgroup->sysname);
  cJSON_AddNumberToObject (msg, "volume", portgroup->volume[0]);
  
  if (portgroup->crossfade_group[0] != '\0') {
    cJSON_AddStringToObject (msg, "crossfade_name", portgroup->crossfade_group);
    cJSON_AddNumberToObject (msg, "crossfade", portgroup->fade);
  } else {
    cJSON_AddStringToObject (msg, "crossfade_name", "");
    cJSON_AddNumberToObject (msg, "crossfade", 0);
  }
  
  cJSON_AddNumberToObject (msg, "xmms2", portgroup->has_xmms2);
  cJSON_AddNumberToObject (msg, "direction", portgroup->direction);

  if (portgroup->direction == INPUT) {
  
    eq = cJSON_CreateObject();
    eqbands = cJSON_CreateArray();
    cJSON_AddItemToObject(eq, "bands", eqbands);
    cJSON_AddItemToObject(msg, "eq", eq);
    for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
      eqband = cJSON_CreateObject();
      cJSON_AddNumberToObject (eqband, "hz", portgroup->eq.band[i].hz);
      cJSON_AddNumberToObject (eqband, "db", portgroup->eq.band[i].db);
      cJSON_AddNumberToObject (eqband, "bw", portgroup->eq.band[i].bandwidth);
      cJSON_AddItemToArray(eqbands, eqband);
    }
    
    cJSON_AddNumberToObject (msg, "lowpass_hz", portgroup->lowpass.hz);
    cJSON_AddNumberToObject (msg, "lowpass_bw", portgroup->lowpass.bandwidth);

    cJSON_AddNumberToObject (msg, "highpass_hz", portgroup->highpass.hz);
    cJSON_AddNumberToObject (msg, "highpass_bw", portgroup->highpass.bandwidth);
    
    cJSON_AddNumberToObject (msg, "analog_drive", portgroup->analog.drive);
    cJSON_AddNumberToObject (msg, "analog_blend", portgroup->analog.blend);

  }
}

void krad_websocket_remove_portgroup ( kr_iws_client_t *client, kr_address_t *address ) {

  cJSON *msg;
  
  cJSON_AddItemToArray(client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "remove_portgroup");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
}

void krad_websocket_set_portgroup_eff ( kr_iws_client_t *client, kr_address_t *address, float value) {

  cJSON *msg;  
  
  cJSON_AddItemToArray(client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "effect_control");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
  cJSON_AddStringToObject (msg, "effect_name", effect_type_to_string (address->sub_id + 1));
  cJSON_AddNumberToObject (msg, "effect_num", address->sub_id2);
  cJSON_AddStringToObject (msg, "control_name", effect_control_to_string(address->control.effect_control));
  cJSON_AddNumberToObject (msg, "value", value);
}

void krad_websocket_remove_subunit ( kr_iws_client_t *client, kr_address_t *address) {

  cJSON *msg;
  
  cJSON_AddItemToArray(client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradcompositor");
  
  cJSON_AddStringToObject (msg, "cmd", "remove_subunit");

  cJSON_AddStringToObject (msg, "subunit_type",
    kr_compositor_subunit_type_to_string(address->path.subunit.compositor_subunit));

  cJSON_AddNumberToObject (msg, "subunit_id", address->id.number);
}

void krad_websocket_update_subunit (kr_iws_client_t *client, kr_crate_t *crate) {

  cJSON *msg;  
  kr_address_t *address;

  address = crate->addr;

  cJSON_AddItemToArray(client->ws.json, msg = cJSON_CreateObject());
  
  /*
  if (crate->addr->path.unit == KR_MIXER) {
    cJSON_AddStringToObject (msg, "com", "kradmixer");
    cJSON_AddStringToObject (msg, "cmd", "control_portgroup");
    cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
    cJSON_AddStringToObject (msg, "control_name",
               portgroup_control_to_string(address->control.portgroup_control));
    if (kr_crate_contains_float (crate)) {
      cJSON_AddNumberToObject (msg, "value", crate->real);
    }
  }
  */

  if (crate->addr->path.unit == KR_COMPOSITOR) {
    cJSON_AddStringToObject (msg, "com", "kradcompositor");
    cJSON_AddStringToObject (msg, "cmd", "update_subunit");
    cJSON_AddStringToObject (msg, "subunit_type",
      kr_compositor_subunit_type_to_string(address->path.subunit.compositor_subunit));

    cJSON_AddNumberToObject (msg, "subunit_id", address->id.number);
    cJSON_AddStringToObject (msg, "control_name",
               kr_compositor_control_to_string(address->control.compositor_control));

    if (kr_crate_contains_float (crate)) {
      cJSON_AddNumberToObject (msg, "value", crate->real);
    } else {
      if (kr_crate_contains_int (crate)) {
        cJSON_AddNumberToObject (msg, "value", crate->integer);
      }
    }
  }
}

void krad_websocket_set_portgroup_control ( kr_iws_client_t *client, kr_address_t *address, float value) {

  cJSON *msg;  
  
  cJSON_AddItemToArray(client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "control_portgroup");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
  cJSON_AddStringToObject (msg, "control_name", portgroup_control_to_string(address->control.portgroup_control));
  cJSON_AddNumberToObject (msg, "value", value);
}

void krad_websocket_set_portgroup_peak ( kr_iws_client_t *client, kr_address_t *address, float value) {

  cJSON *msg;  
  
  cJSON_AddItemToArray(client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "peak_portgroup");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
  //cJSON_AddStringToObject (msg, "control_name", portgroup_control_to_string(address->control.portgroup_control));
  cJSON_AddNumberToObject (msg, "value", value);
}

void krad_websocket_update_portgroup ( kr_iws_client_t *client, kr_address_t *address, char *value ) {

  cJSON *msg;
  
  cJSON_AddItemToArray (client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "update_portgroup");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
  cJSON_AddStringToObject (msg, "control_name", portgroup_control_to_string(address->control.portgroup_control));
  cJSON_AddStringToObject (msg, "value", value);
  
  //kr_tags (client->ws.krclient, address->id.name);
}

void krad_websocket_set_mixer ( kr_iws_client_t *client, kr_mixer_t *mixer) {

  cJSON *msg;
  
  cJSON_AddItemToArray(client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "set_mixer_params");
  cJSON_AddNumberToObject (msg, "sample_rate", mixer->sample_rate);
}

void krad_websocket_add_comp_subunit ( kr_iws_client_t *client, kr_crate_t *crate) {

  cJSON *msg;
  kr_address_t *address;
  kr_compositor_subunit_controls_t controls;

  address = crate->addr;
  
  cJSON_AddItemToArray (client->ws.json, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradcompositor");
  
  cJSON_AddStringToObject (msg, "cmd", "add_subunit");
  cJSON_AddStringToObject (msg, "subunit_type",
    kr_compositor_subunit_type_to_string(address->path.subunit.compositor_subunit));  
  cJSON_AddNumberToObject (msg, "subunit_id", address->id.number);

  switch (address->path.subunit.compositor_subunit) {
    case KR_SPRITE:
      controls = crate->inside.sprite->controls;
      cJSON_AddStringToObject (msg, "filename", crate->inside.sprite->filename);
      cJSON_AddNumberToObject (msg, "rate", controls.tickrate);
      break;
    case KR_TEXT:
      controls = crate->inside.text->controls;
      cJSON_AddStringToObject (msg, "text", crate->inside.text->text);
      cJSON_AddStringToObject (msg, "font", crate->inside.text->font);
      cJSON_AddNumberToObject (msg, "red", crate->inside.text->red);
      cJSON_AddNumberToObject (msg, "green", crate->inside.text->green);
      cJSON_AddNumberToObject (msg, "blue", crate->inside.text->blue);
      break;  
    case KR_VIDEOPORT:
      controls = crate->inside.videoport->controls;
      cJSON_AddStringToObject (msg, "port_name", crate->inside.videoport->sysname);
      if (crate->inside.videoport->direction == OUTPUT) {
        cJSON_AddStringToObject (msg, "direction", "output");
      } else {
        cJSON_AddStringToObject (msg, "direction", "input");
      }
      break;
    case KR_VECTOR:
      controls = crate->inside.vector->controls;
      cJSON_AddStringToObject (msg, "type",
                               krad_vector_type_to_string(crate->inside.vector->type));
      break;
  }

  cJSON_AddNumberToObject (msg, "xscale", controls.xscale);
  cJSON_AddNumberToObject (msg, "yscale", controls.yscale);
  cJSON_AddNumberToObject (msg, "x", controls.x);
  cJSON_AddNumberToObject (msg, "y", controls.y);
  cJSON_AddNumberToObject (msg, "z", controls.z);
  cJSON_AddNumberToObject (msg, "r", controls.rotation);
  cJSON_AddNumberToObject (msg, "o", controls.opacity);
  cJSON_AddNumberToObject (msg, "width", controls.width);
  cJSON_AddNumberToObject (msg, "height", controls.height);
}

/* Krad API Handler */

static void crate_to_json (kr_iws_client_t *client, kr_crate_t *crate) {
  switch ( crate->contains ) {
    case KR_MIXER:
      krad_websocket_set_mixer (client, crate->inside.mixer);
      return;
    case KR_PORTGROUP:
      krad_websocket_add_portgroup (client, crate->inside.portgroup);
      return;
    case KR_SPRITE:
    case KR_VECTOR:
    case KR_TEXT:
    case KR_VIDEOPORT:
      krad_websocket_add_comp_subunit (client, crate);
      return;
  }
}

static int krad_delivery_handler (kr_iws_client_t *client) {

  kr_crate_t *crate;
  char *string;
  
  string = NULL;  
  crate = NULL;

  kr_delivery_recv(client->ws.krclient);

  while ((kr_delivery_get (client->ws.krclient, &crate) > 0) &&
         (crate != NULL)) {

    //printk("got a delivery!");

    /* Subunit updated */
    if (kr_crate_notice (crate) == EBML_ID_KRAD_SUBUNIT_CONTROL) {
      if (crate->addr->path.unit == KR_MIXER) {
        if (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP) {
          if (kr_crate_contains_float (crate)) {
            if (crate->addr->control.portgroup_control == KR_PEAK) {
              krad_websocket_set_portgroup_peak (client, crate->addr, crate->real);
            } else {
              krad_websocket_set_portgroup_control (client, crate->addr, crate->real);
            }
          } else {
            if ((crate->addr->control.portgroup_control == KR_CROSSFADE_GROUP) ||
                (crate->addr->control.portgroup_control == KR_XMMS2_IPC_PATH)) {
              if (kr_uncrate_string (crate, &string)) {
                krad_websocket_update_portgroup (client, crate->addr, string);
                kr_string_recycle (&string);
              } else {
                krad_websocket_update_portgroup (client, crate->addr, "");
              }
            }
          }
        }
        
        if (crate->addr->path.subunit.mixer_subunit == KR_EFFECT) {
          if (kr_crate_contains_float (crate)) {
            krad_websocket_set_portgroup_eff (client, crate->addr, crate->real);
          }
        }
      }

      if (crate->addr->path.unit == KR_COMPOSITOR) {
        krad_websocket_update_subunit (client, crate);
      }
      
      interweb_json_pack (client);
      kr_crate_recycle (&crate);
      continue;
    }

    /* Subunit Destroyed */
    if (kr_crate_notice (crate) == EBML_ID_KRAD_RADIO_UNIT_DESTROYED) {
      if ((crate->addr->path.unit == KR_MIXER) &&
          (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP)) {
        krad_websocket_remove_portgroup (client, crate->addr);
      }
      if (crate->addr->path.unit == KR_COMPOSITOR) {
        krad_websocket_remove_subunit (client, crate->addr);
      }
      interweb_json_pack (client);
      kr_crate_recycle (&crate);
      continue;
    }

    /* Initial list of subunits or subunit created */
    if (kr_crate_loaded (crate)) {
      crate_to_json (client, crate);
      interweb_json_pack (client);
      kr_crate_recycle (&crate);
      continue;
    }
  }
  
  return 0;
}
