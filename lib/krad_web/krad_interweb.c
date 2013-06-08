#include "krad_interweb.h"

#include "krad_radio.html.h"
#include "krad_radio.js.h"

uint32_t interweb_ws_pack_frame_header(uint8_t *out, uint32_t size);

int strmatch (char *string1, char *string2) {
  
  int len1;
  
  if ((string1 == NULL) || (string2 == NULL)) {
    if ((string1 == NULL) && (string2 == NULL)) {
      return 1;
    }
    return 0;
  } 

  len1 = strlen (string1);

  if (len1 == strlen(string2)) {
    if (strncmp(string1, string2, len1) == 0) {
      return 1;
    }
  }
  return 0;
}

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










static int kr_interweb_server_socket_setup (char *interface, int port) {

  char port_string[6];
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s;
  int sfd = 0;
  char *interface_actual;
  
  interface_actual = interface;
  
  printk ("Krad Interweb Server: interface: %s port %d", interface, port);

  snprintf (port_string, 6, "%d", port);

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
  hints.ai_flags = AI_PASSIVE;     /* All interfaces */

  if ((strlen(interface) == 7) && (strncmp(interface, "0.0.0.0", 7) == 0)) {
    hints.ai_family = AF_INET;
    interface_actual = NULL;
  }
  if ((strlen(interface) == 4) && (strncmp(interface, "[::]", 4) == 0)) {
    hints.ai_family = AF_INET6;
    interface_actual = NULL;
  }

  s = getaddrinfo (interface_actual, port_string, &hints, &result);
  if (s != 0) {
    printke ("getaddrinfo: %s\n", gai_strerror (s));
    return -1;
  }

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    
    sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    
    if (sfd == -1) {
      continue;
    }

    s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
    
    if (s == 0) {
      /* We managed to bind successfully! */
      break;
    }

    close (sfd);
  }

  if (rp == NULL) {
    printke ("Could not bind %d\n", port);
    return -1;
  }

  freeaddrinfo (result);

  return sfd;
}

#ifdef KR_LINUX
int krad_interweb_server_listen_on_adapter (krad_iws_t *server,
                                            char *adapter,
                                            int32_t port) {

  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];
  int ret;
  int ifs;

  ret = 0;
  ifs = 0;

  if (getifaddrs (&ifaddr) == -1) {
    return -1;
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL) {
      continue;
    }

    family = ifa->ifa_addr->sa_family;

    if (family == AF_INET || family == AF_INET6) {
      s = getnameinfo(ifa->ifa_addr,
             (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                   sizeof(struct sockaddr_in6),
             host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        printke ("getnameinfo() failed: %s\n", gai_strerror(s));
        return -1;
      }
      ret = krad_interweb_server_listen_on (server, host, port);
      if (ret == 1) {
        ifs++;
      }
    }
  }

  freeifaddrs (ifaddr);

  return ifs;
}
#endif

int krad_interweb_server_listen_off (krad_iws_t *server,
                                     char *interface,
                                     int32_t port) {

  //FIXME needs to loop thru clients and disconnect remote ones .. optionally?

  int r;
  int d;
  int all_if;
  
  all_if = 0;
  d = 0;
  
  if (strlen(interface) == 0) {
    all_if = 1;
  }

  for (r = 0; r < MAX_REMOTES; r++) {
    if ((server->tcp_sd[r] != 0) &&
        ((port == 0) || (server->tcp_port[r] == port)) &&
         ((all_if == 1) || (strmatch(server->tcp_interface[r], interface)))) {
          
      close (server->tcp_sd[r]);
      server->tcp_sd[r] = 0;
      d++;
      printk ("Disabled interweb on %s port %d",
              server->tcp_interface[r], server->tcp_port[r]);

      server->tcp_port[r] = 0;
      free (server->tcp_interface[r]);
    }
  }

  return d;
}

int krad_interweb_server_listen_on (kr_interweb_server_t *server,
                                    char *interface,
                                    int32_t port) {

  int r;
  int sd;
  
  sd = 0;
  
  if ((interface == NULL) || (strlen(interface) == 0)) {
    interface = "[::]";
  } else {
    #ifdef KR_LINUX
    if (krad_system_is_adapter (interface)) {
      //printk ("Krad Interweb Server: its an adapter,
      //we should probably bind to all ips of this adapter");
      return krad_interweb_server_listen_on_adapter (server, interface, port);
    }
    #else
      return 0;
    #endif
  }

  for (r = 0; r < MAX_REMOTES; r++) {
    if ((server->tcp_sd[r] != 0) && (server->tcp_port[r] == port)) {
      if ((strlen(interface) == strlen(server->tcp_interface[r])) &&
          (strncmp(interface, server->tcp_interface[r], strlen(interface)))) {
        return 0;
      }
    }
  }  
  
  for (r = 0; r < MAX_REMOTES; r++) {
    if ((server->tcp_sd[r] == 0) && (server->tcp_port[r] == 0)) {
    
      sd = kr_interweb_server_socket_setup (interface, port);
      
      if (sd < 0) {
        return 0;
      }
      
      server->tcp_port[r] = port;
      server->tcp_sd[r] = sd;

      if (server->tcp_sd[r] != 0) {
        listen (server->tcp_sd[r], SOMAXCONN);
        server->tcp_interface[r] = strdup (interface);
        printk ("Enable remote on interface %s port %d", interface, port);
        return 1;
      } else {
        server->tcp_port[r] = 0;
        return 0;
      }
    }
  }

  return 0;
}

static kr_iws_client_t *kr_iws_accept_client (kr_iws_t *server, int sd) {

  kr_iws_client_t *client = NULL;
  
  int i;
  struct sockaddr_un sin;
  socklen_t sin_len;
    
  while (client == NULL) {
    for(i = 0; i < KR_IWS_MAX_CLIENTS; i++) {
      if (server->clients[i].sd == 0) {
        client = &server->clients[i];
        break;
      }
    }
    if (client == NULL) {
      //printk ("Krad Interweb Server: Overloaded with clients!\n");
      return NULL;
    }
  }

  sin_len = sizeof (sin);
  client->sd = accept (sd, (struct sockaddr *)&sin, &sin_len);

  if (client->sd > -1) {
    krad_system_set_socket_nonblocking (client->sd);
    client->in = kr_io2_create ();
    client->out = kr_io2_create_size (128000);
    kr_io2_set_fd (client->in, client->sd);
    kr_io2_set_fd (client->out, client->sd);
    client->server = server;
    printk ("Krad Interweb Server: Client accepted!");  
    return client;
  } else {
    printke ("Krad Interweb Server: Client NOT accepted!");  
  }

  return NULL;
}

static void krad_interweb_disconnect_client (kr_interweb_server_t *server, kr_iws_client_t *client) {
  close (client->sd);
  client->sd = 0;
  client->type = 0;
  client->drop_after_sync = 0;
  client->hle = 0;
  client->hle_pos = 0;
  client->got_headers = 0;
  memset(&client->ws, 0, sizeof(interwebs_t));
  memset(client->get, 0, sizeof(client->get));
  kr_io2_destroy (&client->in);
  kr_io2_destroy (&client->out);
  if (client->ws.krclient != NULL) {
    kr_client_destroy (&client->ws.krclient);
  }
  printk ("Krad Interweb Server: Client Disconnected");
}

static void krad_interweb_server_update_pollfds (kr_interweb_server_t *server) {

  int r;
  int c;
  int s;

  s = 0;

  server->sockets[s].fd = krad_controller_get_client_fd (&server->krad_control);
  server->sockets[s].events = POLLIN;

  s++;
  
  for (r = 0; r < MAX_REMOTES; r++) {
    if (server->tcp_sd[r] != 0) {
      server->sockets[s].fd = server->tcp_sd[r];
      server->sockets[s].events = POLLIN;
      s++;
      server->socket_type[s] = KR_REMOTE_LISTEN;
    }
  }

  for (c = 0; c < KR_IWS_MAX_KRCLIENTS; c++) {
    if ((server->clients[c].sd > 0) && (server->clients[c].ws.krclient != NULL)) {
      server->sockets[s].fd = kr_client_get_fd (server->clients[c].ws.krclient);
      server->sockets[s].events = POLLIN;
      //if (kr_io2_want_out (server->clients[c].out)) {
      //  server->sockets[s].events |= POLLOUT;
      //}
      server->sockets_clients[s] = &server->clients[c];
      server->socket_type[s] = KR_APP;
      s++;
    }
  }

  for (c = 0; c < KR_IWS_MAX_CLIENTS; c++) {
    if (server->clients[c].sd > 0) {
      server->sockets[s].fd = server->clients[c].sd;
      server->sockets[s].events |= POLLIN;
      if (kr_io2_want_out (server->clients[c].out)) {
        server->sockets[s].events |= POLLOUT;
      }
      server->sockets_clients[s] = &server->clients[c];
      server->socket_type[s] = WS;
      s++;
    }
  }
  
  server->socket_count = s;

  //printk ("Krad Interweb Server: sockets rejiggerd!\n");  
}

char *kr_interweb_server_load_file_or_string (char *input) {

  int fd;
  char *string;
  unsigned int length;
  struct stat file_stat;
  int bytes_read;
  int ret;
  
  fd = 0;
  bytes_read = 0;
  string = NULL;

  if (input == NULL) {
    return NULL;
  }

  if ((strlen(input)) && (input[0] == '/')) {

    fd = open (input, O_RDONLY);
    if (fd < 1) {
      printke("could not open");    
      return NULL;
    }
    fstat (fd, &file_stat);
    length = file_stat.st_size;
    if (length > 1000000) {
      printke("too big");
      close (fd);
      return NULL;
    }

    string = calloc (1, length);        

    while (bytes_read < length) {
    
      ret = read (fd, string + bytes_read, length - bytes_read);

      if (ret < 0) {
        printke("read fail");
        close (fd);
        free (string);
        return NULL;
      }
      bytes_read += ret;
    }
    close (fd);
    return string;
  } else {
    return strdup (input);
  }
}

void kr_interweb_server_setup_html (kr_interweb_t *server) {

  char string[64];
  char *html_template;
  int html_template_len;
  int total_len;
  int len;
  int pos;
  int template_pos;
  
  template_pos = 0;
  pos = 0;
  len = 0;
  total_len = 0;
  
  memset (string, 0, sizeof(string));
  snprintf (string, 7, "%d", server->uberport);
  total_len += strlen(string);
  server->api_js = (char *)lib_krad_web_res_krad_radio_js;
  server->api_js_len = lib_krad_web_res_krad_radio_js_len;
  server->api_js[server->api_js_len] = '\0';
  
  html_template = (char *)lib_krad_web_res_krad_radio_html;
  html_template_len = lib_krad_web_res_krad_radio_html_len - 1;
  total_len += html_template_len - 4;

  server->headcode = kr_interweb_server_load_file_or_string (server->headcode_source);
  server->htmlheader = kr_interweb_server_load_file_or_string (server->htmlheader_source);
  server->htmlfooter = kr_interweb_server_load_file_or_string (server->htmlfooter_source);
  
  if (server->headcode != NULL) {
    total_len += strlen(server->headcode);
  }
  if (server->htmlheader != NULL) {
    total_len += strlen(server->htmlheader);    
  }
  if (server->htmlfooter != NULL) {
    total_len += strlen(server->htmlfooter);    
  }

  server->html_len = total_len + 1;
  server->html = calloc (1, server->html_len);
  
  len = strcspn (html_template, "~");
  strncpy (server->html, html_template, len);
  strcpy (server->html + len, string);
  pos = len + strlen(string);
  template_pos = len + 1;
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (server->headcode != NULL) {
    len = strlen(server->headcode);
    strncpy (server->html + pos, server->headcode, len);
    pos += len;
  }
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (server->htmlheader != NULL) {
    len = strlen(server->htmlheader);
    strncpy (server->html + pos, server->htmlheader, len);
    pos += len;
  }
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (server->htmlfooter != NULL) {
    len = strlen(server->htmlfooter);
    strncpy (server->html + pos, server->htmlfooter, len);
    pos += len;
  }
  
  len = html_template_len - template_pos;
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len;
  pos += len;  
  
  if (template_pos != html_template_len) {
    failfast("html template miscalculation: %d %d", template_pos, html_template_len);
  }  
  
  if (pos != total_len) {
    printke("html miscalculation: %d %d", pos, total_len);
  }

  server->html[total_len] = '\0';

  if (server->headcode != NULL) {
    free (server->headcode);
    server->headcode = NULL;    
  }
  if (server->htmlheader != NULL) {
    free (server->htmlheader);
    server->htmlheader = NULL;    
  }
  if (server->htmlfooter != NULL) {
    free (server->htmlfooter);
    server->htmlfooter = NULL;    
  }    
}

static void krad_interweb_pack_headers (kr_iws_client_t *client, char *content_type);
static void krad_interweb_pack_buffer (kr_iws_client_t *client, char *buffer, size_t length);

static void krad_interweb_pack_buffer (kr_iws_client_t *client, char *buffer, size_t length) {

  kr_io2_pack (client->out, buffer, length);
}

static void krad_interweb_pack_headers (kr_iws_client_t *client, char *content_type) {

  int32_t pos;
  char *buffer;
  
  pos = 0;

  buffer = (char *)client->out->buf;
  

  pos += sprintf (buffer + pos, "HTTP/1.0 200 OK\r\n");
  pos += sprintf (buffer + pos, "Status: 200 OK\r\n");
  pos += sprintf (buffer + pos, "Connection: close\r\n");
  pos += sprintf (buffer + pos, "Server: Krad-Radio\r\n");
  pos += sprintf (buffer + pos, "Content-Type: %s; charset=utf-8\r\n", content_type);
  pos += sprintf (buffer + pos, "\r\n");


  kr_io2_advance (client->out, pos);
}

static void krad_interweb_pack_404 (kr_iws_client_t *client) {

  int32_t pos;
  char *buffer;
  
  pos = 0;

  buffer = (char *)client->out->buf;
  pos += sprintf (buffer + pos, "HTTP/1.1 404 Not Found\r\n");
  pos += sprintf (buffer + pos, "Status: 404 Not Found\r\n");
  pos += sprintf (buffer + pos, "Connection: close\r\n");
  pos += sprintf (buffer + pos, "Server: Krad-Radio\r\n");  
  pos += sprintf (buffer + pos, "Content-Type: text/html; charset=utf-8\r\n");
  pos += sprintf (buffer + pos, "\r\n");
  pos += sprintf (buffer + pos, "404 Not Found");
  
  kr_io2_advance (client->out, pos);  
}

void krad_interweb_http_client_handle (kr_iws_client_t *client) {

  int32_t len;
  char *get;
  krad_interweb_t *s;

  s = client->server;
  get = client->get;

  if (get[0] == '/') {
    get = client->get + 1;
  }
  len = strlen(get);

  for (;;) {
    if ((len > -1) && (len < 32)) {
      if (strmatch(get, "kr_api.js")) {
        krad_interweb_pack_headers(client, "text/javascript");
        krad_interweb_pack_buffer(client, s->api_js, s->api_js_len);
        break;
      }
      if (strmatch(get, "kr_interface.js")) {
        krad_interweb_pack_headers(client, "text/javascript");
        krad_interweb_pack_buffer(client, s->iface_js, s->iface_js_len);
        break;
      }
      if (strmatch(get, "kr_dev_interface.js")) {
        krad_interweb_pack_headers(client, "text/javascript");
        krad_interweb_pack_buffer(client, s->deviface_js, s->deviface_js_len);
        break;
      }
      if ((len == 0) || (strmatch(get, "krad_radio.html"))) {
        krad_interweb_pack_headers(client, "text/html");
        krad_interweb_pack_buffer(client, s->html, s->html_len);
        break;
      }
    }
    krad_interweb_pack_404(client);
    break;
  }
  client->drop_after_sync = 1;
}

int32_t interweb_ws_parse_frame_header(kr_iws_client_t *client) {

  interwebs_t *ws;
  uint8_t *size_bytes;
  uint8_t payload_sz_8;
  uint64_t payload_sz_64;
  uint16_t payload_sz_16;
  int32_t bytes_read;
  uint8_t frame_type;

  bytes_read = 0;

  ws = &client->ws;
  ws->input_len = client->in->len;
  ws->input = client->in->rd_buf;

  if (ws->input_len < 6) {
    return 0;
  }

  frame_type = ws->input[0];

  //printk("pframe type = %2X", frame_type);

  if (frame_type & WS_FIN_FRM) {
    //printk ("We have a fin frame!");
    frame_type ^= WS_FIN_FRM;
  }
  //printk("poframe type = %2X", frame_type);

  if (frame_type == WS_PING_FRM) {
    //printk ("We have a ping frame!");
  } else {
    if (frame_type == WS_CLOSE_FRM) {
      //printk ("We have a close frame!");
    } else {
      if (frame_type == WS_BIN_FRM) {
        //printk ("We have a bin frame!");
      } else {
        if (frame_type == WS_TEXT_FRM) {
          //printk ("We have a text frame!");
        } else {
          if (frame_type == WS_CONT_FRM) {
            //printk ("We have a CONT frame!");
          } else {
            printke ("Unknown frame type!");
            return -9;
          }
        }
      }
    }
  }

  payload_sz_8 = ws->input[1];

  if (payload_sz_8 & WS_MASK_BIT) {
    payload_sz_8 ^= WS_MASK_BIT;
  } else {
    printke("Mask Bit is NOT set");
    return -4;
  }

  if (payload_sz_8 < 126) {
    //printk("payload size is %u", payload_sz_8);
    ws->mask[0] = ws->input[2];
    ws->mask[1] = ws->input[3];
    ws->mask[2] = ws->input[4];
    ws->mask[3] = ws->input[5];
    ws->len = payload_sz_8;
    bytes_read = 6;
  } else {
    if (ws->input_len < 8) {
      return 0;
    }
    if (payload_sz_8 == 126) {
      size_bytes = (uint8_t *)&payload_sz_16;
      size_bytes[1] = ws->input[2];
      size_bytes[0] = ws->input[3];
      ws->mask[0] = ws->input[4];
      ws->mask[1] = ws->input[5];
      ws->mask[2] = ws->input[6];
      ws->mask[3] = ws->input[7];
      ws->len = payload_sz_16;
      bytes_read = 8;
    } else {
      if (ws->input_len < 14) {
        return 0;
      }
      size_bytes = (uint8_t *)&payload_sz_64;
      size_bytes[7] = ws->input[2];
      size_bytes[6] = ws->input[3];
      size_bytes[5] = ws->input[4];
      size_bytes[4] = ws->input[5];
      size_bytes[3] = ws->input[6];
      size_bytes[2] = ws->input[7];
      size_bytes[1] = ws->input[8];
      size_bytes[0] = ws->input[9];
      ws->mask[0] = ws->input[10];
      ws->mask[1] = ws->input[11];
      ws->mask[2] = ws->input[12];
      ws->mask[3] = ws->input[13];
      ws->len = payload_sz_64;
      bytes_read = 14;
    }
  }

  if (ws->len > 100000) {
    printke("input ws frame size too big");
    ws->len = 0;
    ws->pos = 0;
    return -10;
  }

  ws->pos = 0;
  ws->frames++;
  //printk("payload size is %"PRIu64"", ws->len);

  kr_io2_pulled (client->in, bytes_read);

  return bytes_read;
}

int32_t interweb_ws_parse_frame_data (kr_iws_client_t *client) {

  interwebs_t *ws;

  ws = &client->ws;
  ws->input_len = client->in->len;
  ws->input = client->in->rd_buf;

  if (ws->input_len < ws->len) {
    printke ("fraak!");
    return 0;
  }

  int32_t pos;
  int32_t max;
  uint8_t output[1024];

  ws->output = output;
  ws->output_len = sizeof(output);

  pos = 0;

  if ((ws->len == 0) || (ws->pos == ws->len) || (ws->input_len == 0) ||
      (ws->output_len == 0)) {
    return 0;
  }

  max = MIN(MIN((ws->len - ws->pos), ws->input_len), ws->output_len);

  //printk ("max is %d", max);

  for (pos = 0; pos < max; pos++) {
    ws->output[pos] = ws->input[ws->pos] ^ ws->mask[ws->pos % 4];
    ws->pos++;
  }

  output[pos] = '\0';
  //printk("unmasked %d bytes %s", pos, (char *)output);

  json_to_cmd (client, (char *)output);

  kr_io2_pulled (client->in, pos);

  if (ws->pos == ws->len) {
    ws->len = 0;
    ws->pos = 0;
  }

  return pos;
}

int32_t interweb_ws_pack_gen_accept_resp (char *resp, char *key) {

  static char *ws_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  int32_t ret;
  char string[128];
  uint8_t hash[20];

  if ((resp == NULL) || (key == NULL)) {
    return -1;
  }

  snprintf(string, sizeof(string), "%s%s", key, ws_guid);
  string[127] = '\0';

  kr_sha1((uint8_t *)string, strlen(string), hash);

  ret = kr_base64((uint8_t *)resp, hash, 20, 64);

  return ret;
}

uint32_t interweb_ws_pack_frame_header(uint8_t *out, uint32_t size) {

  uint16_t size_16;
  uint64_t size_64;
  uint8_t *size_bytes;

  out[0] = WS_FIN_FRM | WS_TEXT_FRM;
  if (size < 126) {
    out[1] = size;
    return 2;
  } else {
    if (size < 65536) {
      out[1] = 126;
      size_16 = size;
      size_bytes = (uint8_t *)&size_16;
      out[2] = size_bytes[1];
      out[3] = size_bytes[0];
      return 4;
    } else {
      out[1] = 127;
      size_64 = size;
      size_bytes = (uint8_t *)&size_64;
      out[2] = size_bytes[7];
      out[3] = size_bytes[6];
      out[4] = size_bytes[5];
      out[5] = size_bytes[4];
      out[6] = size_bytes[3];
      out[7] = size_bytes[2];
      out[8] = size_bytes[1];
      out[9] = size_bytes[0];
      return 10;
    }
  }
}

int32_t interweb_ws_kr_client_connect (kr_iws_client_t *client) {

  client->ws.krclient = kr_client_create ("websocket client");

  if (client->ws.krclient == NULL) {
    return -1;
  }

  if (!kr_connect (client->ws.krclient, client->server->sysname)) {
    kr_client_destroy (&client->ws.krclient);
    return -1;
  }

  kr_mixer_info (client->ws.krclient);
  kr_mixer_portgroup_list (client->ws.krclient);
  kr_compositor_subunit_list (client->ws.krclient);
  kr_subscribe_all (client->ws.krclient);

  return 0;
}

int32_t interweb_ws_hello (kr_iws_client_t *client) {

  cJSON *msg;

  //JSON first start
  client->ws.json = cJSON_CreateArray();

  cJSON_AddItemToArray (client->ws.json, msg = cJSON_CreateObject());
  cJSON_AddStringToObject (msg, "com", "kradradio");
  cJSON_AddStringToObject (msg, "info", "sysname");
  cJSON_AddStringToObject (msg, "infoval", client->server->sysname);

  interweb_json_pack (client);

  return 0;
}

int32_t interweb_ws_shake (kr_iws_client_t *client) {

  int32_t pos;
  char *buffer;
  char acceptkey[64];

  pos = 0;
  buffer = (char *)client->out->buf;
  memset(acceptkey, 0, sizeof(acceptkey));

  interweb_ws_pack_gen_accept_resp (acceptkey, client->ws.key);

  pos += sprintf (buffer + pos, "HTTP/1.1 101 Switching Protocols\r\n");
  pos += sprintf (buffer + pos, "Upgrade: websocket\r\n");
  pos += sprintf (buffer + pos, "Connection: Upgrade\r\n");
  pos += sprintf (buffer + pos, "Sec-WebSocket-Protocol: krad-ws-api\r\n");
  pos += sprintf (buffer + pos, "Sec-WebSocket-Accept: %s\r\n", acceptkey);
  pos += sprintf (buffer + pos, "\r\n");

  kr_io2_advance (client->out, pos);
  client->ws.shaked = 1;

  interweb_ws_hello(client);
  interweb_ws_kr_client_connect(client);

  return 0;
}

int32_t krad_interweb_ws_client_handle (kr_iws_client_t *client) {
  
  if (!client->ws.shaked) {
    interweb_ws_shake(client);
  } else {
    for (;;) {
      if (client->ws.len == 0) {
        interweb_ws_parse_frame_header(client);
      }
      if (client->ws.len > 0) {
        interweb_ws_parse_frame_data(client);
      } else {
        break;
      }
    }
  }

  return 0;
}

int32_t krad_interweb_client_find_end_of_headers (kr_iws_client_t *client) {

  int i;
  uint8_t *buf;

  buf = client->in->rd_buf;

  for (i = 0; i < client->in->len; i++) {
    if ((buf[i] == '\n') || (buf[i] == '\r')) {
      if (client->hle_pos != (i - 1)) {
        client->hle = 0;
      }
      client->hle_pos = i;
      if (buf[i] == '\n') {
        client->hle += 1;
      }
      if (client->hle == 2) {
        client->got_headers = 1;
        return 1;
      }
    }
  }
  return 0;
}

int32_t interweb_get_header (char *buf, char *out, uint32_t max, char *header) {

  char *pos;
  int32_t len;
  int32_t hdr_len;
  
  hdr_len = strlen(header);

  pos = strstr(buf, header) + hdr_len;
  if (pos == NULL) {
    return -1;
  }

  len = strcspn(pos, " \n\r?");
  len = MIN(len, max - 1);
  memcpy(out, pos, len);
  out[len] = '\0';

  return 0;
}

int32_t krad_interweb_client_handle_headers (kr_iws_client_t *client) {

  char *buf;

  buf = (char *)client->in->rd_buf;

  if (client->got_headers == 0) {
    if (krad_interweb_client_find_end_of_headers (client)) {
      buf[client->hle_pos] = '\0';

      if (strstr(buf, "Upgrade: websocket") != NULL) {
        printk ("Krad Interweb websocket is YEAAY after %zu bytes",
         client->in->len);
        client->type = WS;

          interweb_get_header (buf, client->get,
            sizeof(client->get), "GET ");

          printk ("WS GET IS %s", client->get);

          interweb_get_header (buf, client->ws.key,
            sizeof(client->ws.key), "Sec-WebSocket-Key: ");

          printk ("KEY IS %s", client->ws.key);

          interweb_get_header (buf, client->ws.proto,
            sizeof(client->ws.proto), "Sec-WebSocket-Protocol: ");

          printk ("PROTO IS %s", client->ws.proto);

        kr_io2_pulled (client->in, client->hle_pos + 1);
        return 0;
      } else {
        if ((strstr(buf, "GET ") != NULL) &&
            (strstr(buf, " HTTP/1") != NULL)) {
          client->type = HTTP1;

          interweb_get_header (buf, client->get,
            sizeof(client->get), "GET ");

          printk ("GET IS %s", client->get);

          kr_io2_pulled (client->in, client->hle_pos);
          return 0;
        } else {
          return 1;
        }
      }
    }
  }

  if ((client->got_headers == 0) && (client->in->len >= 4096)) {
    printk ("Krad Interweb no header end in sight after %d bytes",
      client->in->len);
    return 1;
  }

  return 0;
}

void krad_interweb_client_handle (kr_iws_client_t *client) {
  if (client->type == WS) {
    krad_interweb_ws_client_handle (client);
  } else {
    if (client->type == HTTP1) {
      krad_interweb_http_client_handle (client);
    }
  }
}

static void *krad_interweb_server_loop (void *arg) {

  kr_interweb_server_t *server = (kr_interweb_server_t *)arg;
  kr_iws_client_t *client;
  int32_t oret;
  int32_t ret;
  int32_t s;
  int32_t r;
  int32_t read_ret;

  krad_system_set_thread_name ("kr_interweb");
  server->shutdown = KRAD_INTERWEB_RUNNING;

  while (!server->shutdown) {

    s = 0;
    krad_interweb_server_update_pollfds (server);
    ret = poll (server->sockets, server->socket_count, -1);

    if ((ret < 1) ||
        (server->shutdown) ||
        (server->sockets[s].revents)) {
      break;
    }

    s++;

    for (r = 0; r < MAX_REMOTES; r++) {
      if (server->tcp_sd[r] != 0) {
        if ((server->tcp_sd[r] != 0) && (server->sockets[s].revents & POLLIN)) {
          kr_iws_accept_client (server, server->tcp_sd[r]);
          ret--;
        }
        s++;
      }
    }

    for (; ret > 0; s++) {
      if (server->sockets[s].revents) {
        ret--;
        client = server->sockets_clients[s];

        if (server->socket_type[s] == KR_APP) {
          if (server->sockets[s].revents & POLLIN) {
            krad_delivery_handler(client);
          }
          continue;
        }

        if (server->sockets[s].revents & POLLIN) {
          read_ret = kr_io2_read (client->in);
          if (read_ret > 0) {
            //printk ("Krad Interweb Server %d: Got %d bytes\n", s, read_ret);
            if (client->type == 0) {
              if (krad_interweb_client_handle_headers (client)) {
                krad_interweb_disconnect_client (server, client);
                continue;
              }
            }
            if (client->type != 0) {
              krad_interweb_client_handle (client);
              if (kr_io2_want_out (client->out)) {
                server->sockets[s].events |= POLLOUT;
              }
            }
          } else {
            if (read_ret == 0) {
              //printk ("Krad Interweb Server: Client EOF\n");
              krad_interweb_disconnect_client (server, client);
              continue;
            }
            if (read_ret == -1) {
              printke ("Krad Interweb Server: Client Socket Error");
              krad_interweb_disconnect_client (server, client);
              continue;
            }
          }
        }
        if (server->sockets[s].revents & POLLOUT) {
          oret = kr_io2_output (client->out);
          if (oret != 0) {
            printke ("panic dropping the client");
            krad_interweb_disconnect_client (server, client);
            continue;
          }
          if (!(kr_io2_want_out (client->out))) {
            if (client->drop_after_sync == 1) {
              krad_interweb_disconnect_client (server, client);
              continue;
            }
            server->sockets[s].events = POLLIN;
          }
        } else {
          if (server->sockets[s].revents & POLLHUP) {
            //printk ("Krad Interweb Server %d : POLLHUP\n", s);
            krad_interweb_disconnect_client (server, client);
            continue;
          }
        }
        if (server->sockets[s].revents & POLLERR) {
          printke ("Krad Interweb Server: POLLERR\n");
          krad_interweb_disconnect_client (server, client);
          continue;
        }
      }
    }
  }

  server->shutdown = KRAD_INTERWEB_SHUTINGDOWN;

  krad_controller_client_close (&server->krad_control);

  return NULL;
}

void krad_interweb_server_disable (kr_interweb_t *server) {

  printk ("Krad Interweb Server: Disable Started");

  if (!krad_controller_shutdown (&server->krad_control, &server->server_thread, 30)) {
    krad_controller_destroy (&server->krad_control, &server->server_thread);
  }

  krad_interweb_server_listen_off (server, "", 0);
  
  printk ("Krad Interweb Server: Disable Complete");
}

void krad_interweb_server_destroy (kr_interweb_t *server) {

  int i;

  printk ("Krad Interweb Server: Destroy Started");

  if (server->shutdown != KRAD_INTERWEB_SHUTINGDOWN) {
    krad_interweb_server_disable (server);
  }
  
  for (i = 0; i < KR_IWS_MAX_CLIENTS; i++) {
    if (server->clients[i].sd > 0) {
      krad_interweb_disconnect_client (server, &server->clients[i]);
    }
  }
  
  free (server->clients);
  free (server);
  
  printk ("Krad Interweb Server: Destroy Completed");
}

void krad_interweb_server_run (kr_interweb_server_t *server) {
  pthread_create (&server->server_thread,
                  NULL,
                  krad_interweb_server_loop,
                  (void *)server);
}

kr_interweb_server_t * krad_interweb_server_create (char *sysname, int32_t port,
                                      char *headcode, char *htmlheader, char *htmlfooter) {

  kr_interweb_server_t *server;

  server = calloc (1, sizeof (kr_interweb_server_t));
  
  if (krad_control_init (&server->krad_control)) {
    return NULL;
  }

  strcpy (server->sysname, sysname);

  server->uberport = port;
  server->headcode_source = headcode;
  server->htmlheader_source = htmlheader;
  server->htmlfooter_source = htmlfooter;
  
  server->shutdown = KRAD_INTERWEB_STARTING;
  
  server->clients = calloc (KR_IWS_MAX_CLIENTS, sizeof (kr_iws_client_t));
  
  kr_interweb_server_setup_html (server);
  
  krad_interweb_server_listen_on (server, NULL, port);

  krad_interweb_server_run (server);

  return server;
}
