#include "krad_websocket.h"

static void json_to_cmd (kr_ws_client_t *kr_ws_client, char *value, int len);
static void crate_to_json (kr_ws_client_t *kr_ws_client, kr_crate_t *crate);
static int krad_delivery_handler (kr_ws_client_t *kr_ws_client);
static void *krad_websocket_server_run (void *arg);
static void add_poll_fd (krad_websocket_t *krad_websocket, int fd, short events, int fd_is,
                         kr_ws_client_t *kr_ws_client, void *bspointer);
static void del_poll_fd (krad_websocket_t *krad_websocket, int fd);

static int callback_http (struct libwebsocket_context *this,
                          struct libwebsocket *wsi,
                          enum libwebsocket_callback_reasons reason, void *user,
                          void *in, size_t len);

static int callback_kr_client (struct libwebsocket_context *this,
                               struct libwebsocket *wsi,
                               enum libwebsocket_callback_reasons reason, void *user,
                               void *in, size_t len);

struct libwebsocket_protocols protocols[] = {
  /* first protocol must always be HTTP handler */

  {
    "http-only",    /* name */
    callback_http,    /* callback */
    0      /* per_session_data_size */
  },
  {
    "krad-ws-api",
    callback_kr_client,
    sizeof(kr_ws_client_t),
  },
  {
    NULL, NULL, 0    /* End of list */
  }
};

/* interpret JSON to speak Krad API */

static void json_to_cmd (kr_ws_client_t *kr_ws_client, char *value, int len) {
  
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
        kr_unit_destroy (kr_ws_client->kr_client, &uc.address);
        return;
      }      
      if ((part != NULL) && (strcmp(part->valuestring, "add_portgroup") == 0)) {
        part2 = cJSON_GetObjectItem (cmd, "portgroup_name");
        part3 = cJSON_GetObjectItem (cmd, "portgroup_direction");
        kr_mixer_create_portgroup (kr_ws_client->kr_client, part2->valuestring, part3->valuestring, 2);
        return;
      }
      if ((part != NULL) && (strcmp(part->valuestring, "update_portgroup") == 0)) {
        part = cJSON_GetObjectItem (cmd, "portgroup_name");
        part2 = cJSON_GetObjectItem (cmd, "control_name");
        part3 = cJSON_GetObjectItem (cmd, "value");
        if ((part != NULL) && (part2 != NULL) && (part3 != NULL)) {
          if (strcmp(part2->valuestring, "xmms2") == 0) {
            kr_mixer_portgroup_xmms2_cmd (kr_ws_client->kr_client, part->valuestring, part3->valuestring);
          } else {
            if (strcmp(part2->valuestring, "set_crossfade_group") == 0) {
              kr_mixer_set_portgroup_crossfade_group (kr_ws_client->kr_client, part->valuestring, part3->valuestring);
            } else {
              floatval = part3->valuefloat;
              kr_mixer_set_control (kr_ws_client->kr_client, part->valuestring, part2->valuestring, floatval, 0);
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
          kr_mixer_set_effect_control (kr_ws_client->kr_client, part->valuestring, sub_id, 0,
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
          kr_mixer_set_effect_control (kr_ws_client->kr_client, part->valuestring, sub_id, part3->valueint,
                                       part4->valuestring,
                                       floatval, 0, EASEINOUTSINE);
        }
        return;
      }
      
      if ((part != NULL) && (strcmp(part->valuestring, "push_dtmf") == 0)) {
        part = cJSON_GetObjectItem (cmd, "dtmf");
        if (part != NULL) {
          kr_mixer_push_tone (kr_ws_client->kr_client, part->valuestring);
        }
      }
      return;
    }
    
    if ((part != NULL) && (strcmp(part->valuestring, "kradcompositor") == 0)) {
      part = cJSON_GetObjectItem (cmd, "cmd");
      if ((part != NULL) && (strcmp(part->valuestring, "jsnap") == 0)) {
        kr_compositor_snapshot_jpeg (kr_ws_client->kr_client);
      }  
      if ((part != NULL) && (strcmp(part->valuestring, "snap") == 0)) {
        kr_compositor_snapshot (kr_ws_client->kr_client);
      }

      if ((part != NULL) && (strcmp(part->valuestring, "display") == 0)) {
         kr_transponder_subunit_create (kr_ws_client->kr_client, "rawout", "");
      }

      if ((part != NULL) && (strcmp(part->valuestring, "remove_subunit") == 0)) {
        memset (&uc, 0, sizeof (uc));
        part2 = cJSON_GetObjectItem (cmd, "subunit_id");
        part3 = cJSON_GetObjectItem (cmd, "subunit_type");
        uc.address.path.unit = KR_COMPOSITOR;
        uc.address.path.subunit.compositor_subunit = kr_string_to_comp_subunit_type (part3->valuestring);
        uc.address.id.number = part2->valueint;
        kr_unit_destroy (kr_ws_client->kr_client, &uc.address);
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
          kr_unit_control_set (kr_ws_client->kr_client, &uc);
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
          kr_compositor_subunit_create (kr_ws_client->kr_client, KR_SPRITE, part3->valuestring, NULL);
        }
        if (uc.address.path.subunit.compositor_subunit == KR_TEXT) {
          part3 = cJSON_GetObjectItem (cmd, "text");
          part4 = cJSON_GetObjectItem (cmd, "font");
          kr_compositor_subunit_create (kr_ws_client->kr_client, KR_TEXT, part3->valuestring, part4->valuestring);
        }
        if (uc.address.path.subunit.compositor_subunit == KR_VECTOR) {
          part3 = cJSON_GetObjectItem (cmd, "vector_type");
          kr_compositor_subunit_create (kr_ws_client->kr_client, KR_VECTOR, part3->valuestring, NULL);
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
          kr_set_tag (kr_ws_client->kr_client, NULL, part2->valuestring, part3->valuestring);
          //printk("aye got %s %s", part2->valuestring, part3->valuestring);
        }
      }
      return;
    }
    //cjson_memreset ();
  }
}

/* callbacks from api handler to add JSON to websocket message */

void krad_websocket_set_tag (kr_ws_client_t *kr_ws_client, char *tag_item, char *tag_name, char *tag_value) {

  cJSON *msg;
  
  cJSON_AddItemToArray(kr_ws_client->msgs, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradradio");
  cJSON_AddStringToObject (msg, "info", "tag");
  cJSON_AddStringToObject (msg, "tag_item", tag_item);
  cJSON_AddStringToObject (msg, "tag_name", tag_name);
  cJSON_AddStringToObject (msg, "tag_value", tag_value);

}

void krad_websocket_set_cpu_usage (kr_ws_client_t *kr_ws_client, int usage) {

  cJSON *msg;
  
  cJSON_AddItemToArray (kr_ws_client->msgs, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradradio");
  cJSON_AddStringToObject (msg, "info", "cpu");
  cJSON_AddNumberToObject (msg, "system_cpu_usage", usage);

}

void krad_websocket_add_portgroup ( kr_ws_client_t *kr_ws_client, kr_mixer_portgroup_t *portgroup) {

  int i;
  cJSON *msg;
  cJSON *eq;
  cJSON *eqbands;
  cJSON *eqband;

  //for the moment will ignore these
  if ((portgroup->direction == OUTPUT) && (portgroup->output_type == DIRECT)) {
    return;
  }

  cJSON_AddItemToArray(kr_ws_client->msgs, msg = cJSON_CreateObject());
  
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

void krad_websocket_remove_portgroup ( kr_ws_client_t *kr_ws_client, kr_address_t *address ) {

  cJSON *msg;
  
  cJSON_AddItemToArray(kr_ws_client->msgs, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "remove_portgroup");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
}

void krad_websocket_set_portgroup_eff ( kr_ws_client_t *kr_ws_client, kr_address_t *address, float value) {

  cJSON *msg;  
  
  cJSON_AddItemToArray(kr_ws_client->msgs, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "effect_control");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
  cJSON_AddStringToObject (msg, "effect_name", effect_type_to_string (address->sub_id + 1));
  cJSON_AddNumberToObject (msg, "effect_num", address->sub_id2);
  cJSON_AddStringToObject (msg, "control_name", effect_control_to_string(address->control.effect_control));
  cJSON_AddNumberToObject (msg, "value", value);
}

void krad_websocket_remove_subunit ( kr_ws_client_t *kr_ws_client, kr_address_t *address) {

  cJSON *msg;
  
  cJSON_AddItemToArray(kr_ws_client->msgs, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradcompositor");
  
  cJSON_AddStringToObject (msg, "cmd", "remove_subunit");

  cJSON_AddStringToObject (msg, "subunit_type",
    kr_compositor_subunit_type_to_string(address->path.subunit.compositor_subunit));

  cJSON_AddNumberToObject (msg, "subunit_id", address->id.number);
}

void krad_websocket_update_subunit (kr_ws_client_t *kr_ws_client, kr_crate_t *crate) {

  cJSON *msg;  
  kr_address_t *address;

  address = crate->addr;

  cJSON_AddItemToArray(kr_ws_client->msgs, msg = cJSON_CreateObject());
  
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

void krad_websocket_set_portgroup_control ( kr_ws_client_t *kr_ws_client, kr_address_t *address, float value) {

  cJSON *msg;  
  
  cJSON_AddItemToArray(kr_ws_client->msgs, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "control_portgroup");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
  cJSON_AddStringToObject (msg, "control_name", portgroup_control_to_string(address->control.portgroup_control));
  cJSON_AddNumberToObject (msg, "value", value);
}

void krad_websocket_set_portgroup_peak ( kr_ws_client_t *kr_ws_client, kr_address_t *address, float value) {

  cJSON *msg;  
  
  cJSON_AddItemToArray(kr_ws_client->msgs, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "peak_portgroup");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
  //cJSON_AddStringToObject (msg, "control_name", portgroup_control_to_string(address->control.portgroup_control));
  cJSON_AddNumberToObject (msg, "value", value);
}

void krad_websocket_update_portgroup ( kr_ws_client_t *kr_ws_client, kr_address_t *address, char *value ) {

  cJSON *msg;
  
  cJSON_AddItemToArray (kr_ws_client->msgs, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "update_portgroup");
  cJSON_AddStringToObject (msg, "portgroup_name", address->id.name);
  cJSON_AddStringToObject (msg, "control_name", portgroup_control_to_string(address->control.portgroup_control));
  cJSON_AddStringToObject (msg, "value", value);
  
  //kr_tags (kr_ws_client->kr_client, address->id.name);
}

void krad_websocket_set_mixer ( kr_ws_client_t *kr_ws_client, kr_mixer_t *mixer) {

  cJSON *msg;
  
  cJSON_AddItemToArray(kr_ws_client->msgs, msg = cJSON_CreateObject());
  
  cJSON_AddStringToObject (msg, "com", "kradmixer");
  
  cJSON_AddStringToObject (msg, "cmd", "set_mixer_params");
  cJSON_AddNumberToObject (msg, "sample_rate", mixer->sample_rate);
}

void krad_websocket_add_comp_subunit ( kr_ws_client_t *kr_ws_client, kr_crate_t *crate) {

  cJSON *msg;
  kr_address_t *address;
  kr_compositor_subunit_controls_t controls;

  address = crate->addr;
  
  cJSON_AddItemToArray (kr_ws_client->msgs, msg = cJSON_CreateObject());
  
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

      cJSON_AddNumberToObject (msg, "view_top_left_x", crate->inside.videoport->view.top_left.x);
      cJSON_AddNumberToObject (msg, "view_top_left_y", crate->inside.videoport->view.top_left.y);

      cJSON_AddNumberToObject (msg, "view_top_right_x", crate->inside.videoport->view.top_right.x);
      cJSON_AddNumberToObject (msg, "view_top_right_y", crate->inside.videoport->view.top_right.y);

      cJSON_AddNumberToObject (msg, "view_bottom_left_x", crate->inside.videoport->view.bottom_left.x);
      cJSON_AddNumberToObject (msg, "view_bottom_left_y", crate->inside.videoport->view.bottom_left.y);

      cJSON_AddNumberToObject (msg, "view_bottom_right_x", crate->inside.videoport->view.bottom_right.x);
      cJSON_AddNumberToObject (msg, "view_bottom_right_y", crate->inside.videoport->view.bottom_right.y);


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

static void crate_to_json (kr_ws_client_t *kr_ws_client, kr_crate_t *crate) {
  switch ( crate->contains ) {
    case KR_MIXER:
      krad_websocket_set_mixer (kr_ws_client, crate->inside.mixer);
      return;
    case KR_PORTGROUP:
      krad_websocket_add_portgroup (kr_ws_client, crate->inside.portgroup);
      return;
    case KR_SPRITE:
    case KR_VECTOR:
    case KR_TEXT:
    case KR_VIDEOPORT:
      krad_websocket_add_comp_subunit (kr_ws_client, crate);
      return;
  }
}

static int
 krad_delivery_handler (kr_ws_client_t *kr_ws_client) {

  kr_crate_t *crate;
  char *string;
  
  string = NULL;  
  crate = NULL;

  while ((kr_delivery_get (kr_ws_client->kr_client, &crate) > 0) &&
         (crate != NULL)) {

    /* Subunit updated */
    if (kr_crate_notice (crate) == EBML_ID_KRAD_SUBUNIT_CONTROL) {
      if (crate->addr->path.unit == KR_MIXER) {
        if (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP) {
          if (kr_crate_contains_float (crate)) {
            if (crate->addr->control.portgroup_control == KR_PEAK) {
              krad_websocket_set_portgroup_peak (kr_ws_client, crate->addr, crate->real);
            } else {
              krad_websocket_set_portgroup_control (kr_ws_client, crate->addr, crate->real);
            }
          } else {
            if ((crate->addr->control.portgroup_control == KR_CROSSFADE_GROUP) ||
                (crate->addr->control.portgroup_control == KR_XMMS2_IPC_PATH)) {
              if (kr_uncrate_string (crate, &string)) {
                krad_websocket_update_portgroup (kr_ws_client, crate->addr, string);
                kr_string_recycle (&string);
              } else {
                krad_websocket_update_portgroup (kr_ws_client, crate->addr, "");
              }
            }
          }
        }
        
        if (crate->addr->path.subunit.mixer_subunit == KR_EFFECT) {
          if (kr_crate_contains_float (crate)) {
            krad_websocket_set_portgroup_eff (kr_ws_client, crate->addr, crate->real);
          }
        }
      }

      if (crate->addr->path.unit == KR_COMPOSITOR) {
        krad_websocket_update_subunit (kr_ws_client, crate);
      }
 
      kr_crate_recycle (&crate);
      continue;
    }

    /* Subunit Destroyed */
    if (kr_crate_notice (crate) == EBML_ID_KRAD_RADIO_UNIT_DESTROYED) {
      if ((crate->addr->path.unit == KR_MIXER) &&
          (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP)) {
        krad_websocket_remove_portgroup (kr_ws_client, crate->addr);
      }
      if (crate->addr->path.unit == KR_COMPOSITOR) {
        krad_websocket_remove_subunit (kr_ws_client, crate->addr);
      }
      kr_crate_recycle (&crate);
      continue;
    }

    /* Initial list of subunits or subunit created */
    if (kr_crate_loaded (crate)) {
      crate_to_json (kr_ws_client, crate);
      kr_crate_recycle (&crate);
      continue;
    }
  }
  
  return 0;
}

/****  Poll Functions  ****/

static void add_poll_fd (krad_websocket_t *krad_websocket, int fd, short events, int fd_is,
                         kr_ws_client_t *kr_ws_client, void *bspointer) {

  krad_websocket->fdof[krad_websocket->count_pollfds] = fd_is;
  if (fd_is == KRAD_APP) {
    krad_websocket->sessions[krad_websocket->count_pollfds] = kr_ws_client;
  }
  krad_websocket->pollfds[krad_websocket->count_pollfds].fd = fd;
  krad_websocket->pollfds[krad_websocket->count_pollfds].events = events;
  krad_websocket->pollfds[krad_websocket->count_pollfds++].revents = 0;
}

static void del_poll_fd (krad_websocket_t *krad_websocket, int fd) {

  int n;
  krad_websocket->count_pollfds--;

  for (n = 0; n < krad_websocket->count_pollfds; n++) {
    if (krad_websocket->pollfds[n].fd == fd) {
      while (n < krad_websocket->count_pollfds) {
        krad_websocket->pollfds[n] = krad_websocket->pollfds[n + 1];
        krad_websocket->sessions[n] = krad_websocket->sessions[n + 1];
        krad_websocket->fdof[n] = krad_websocket->fdof[n + 1];
        n++;
      }
    }
  }
}


/* WebSocket Functions */

static int callback_http (struct libwebsocket_context *this,
                          struct libwebsocket *wsi,
                          enum libwebsocket_callback_reasons reason,
                          void *user, void *in, size_t len) {

  int n;
  krad_websocket_t *krad_websocket;

  n = 0;
  krad_websocket = libwebsocket_context_user (this);

  switch (reason) {
    case LWS_CALLBACK_ADD_POLL_FD:
      krad_websocket->fdof[krad_websocket->count_pollfds] = MYSTERY;
      krad_websocket->pollfds[krad_websocket->count_pollfds].fd = (int)(long)in;
      krad_websocket->pollfds[krad_websocket->count_pollfds].events = (int)len;
      krad_websocket->pollfds[krad_websocket->count_pollfds++].revents = 0;
      break;
    case LWS_CALLBACK_DEL_POLL_FD:
      n = (int)(long)in;
      del_poll_fd (krad_websocket, n);
      break;
    case LWS_CALLBACK_SET_MODE_POLL_FD:
      for (n = 0; n < krad_websocket->count_pollfds; n++) {
        if (krad_websocket->pollfds[n].fd == (int)(long)in) {
          krad_websocket->pollfds[n].events |= (int)(long)len;
        }
      }
      break;
    case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
      for (n = 0; n < krad_websocket->count_pollfds; n++) {
        if (krad_websocket->pollfds[n].fd == (int)(long)in) {
          krad_websocket->pollfds[n].events &= ~(int)(long)len;
        }
      }
      break;
    case LWS_CALLBACK_ESTABLISHED:
      printk (" on http LWS_CALLBACK_ESTABLISHED");
      break;
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
      printk (" on http LWS_CALLBACK_CLIENT_CONNECTION_ERROR");
      break;
    case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
      printk (" on http LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH");
      break;
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
      printk (" on http LWS_CALLBACK_CLIENT_ESTABLISHED");
      break;
    case LWS_CALLBACK_CLOSED:
      printk (" on http LWS_CALLBACK_CLOSE");
      break;
    case LWS_CALLBACK_RECEIVE:
      printk (" on http LWS_CALLBACK_RECEIVE");
      break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
      printk (" on http LWS_CALLBACK_CLIENT_RECEIVE");
      break;
    case LWS_CALLBACK_CLIENT_WRITEABLE:
      printk (" on http LWS_CALLBACK_CLIENT_WRITEABLE");
      break;
    case LWS_CALLBACK_HTTP:
      printk (" on http LWS_CALLBACK_HTTP");
      break;
    case LWS_CALLBACK_HTTP_FILE_COMPLETION:
      printk (" on http LWS_CALLBACK_HTTP_FILE_COMPLETION");
      break;
    case LWS_CALLBACK_HTTP_WRITEABLE:
      printk (" on http LWS_CALLBACK_HTTP_WRITEABLE");
      break;
    case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
      printk (" on http LWS_CALLBACK_FILTER_NETWORK_CONNECTION");
      break;
    case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
      printk (" on http LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION");
      break;
    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
      printk (" on http LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS");
      break;
    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS:
      printk (" on http LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS");
      break;
    case LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
      printk (" on http LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION");
      break;
    case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
      printk (" on http LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER");
      break;
    case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY:
      printk (" on http LWS_CALLBACK_CONFIRM_EXTENSION_OKAY");
      break;
    case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
      printk (" on http LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED");
      break;
    case LWS_CALLBACK_PROTOCOL_INIT:
      printk (" on http LWS_CALLBACK_PROTOCOL_INIT");
      break;
    case LWS_CALLBACK_PROTOCOL_DESTROY:
      printk (" on http LWS_CALLBACK_PROTOCOL_DESTROY");
      break;
    default:
      printke ("callback_http for unknown case! %d on fd %d", (int)(long)in, reason);
      break;
  }
  return 0;
}


static int callback_kr_client (struct libwebsocket_context *this,
                               struct libwebsocket *wsi, 
                               enum libwebsocket_callback_reasons reason, 
                               void *user, void *in, size_t len) {

  int ret;
  krad_websocket_t *krad_websocket;
  kr_ws_client_t *kr_ws_client;
  
  krad_websocket = libwebsocket_context_user (this);
  kr_ws_client = user;
  
  switch (reason) {

    case LWS_CALLBACK_ESTABLISHED:

      kr_ws_client->context = this;
      kr_ws_client->wsi = wsi;
      kr_ws_client->krad_websocket = krad_websocket;
      kr_ws_client->buffer = malloc (KR_WS_BUFSIZE);
      kr_ws_client->kr_client = kr_client_create ("websocket client");
      
      if (kr_ws_client->kr_client == NULL) {
        return -1;
      }
      
      if (!kr_connect (kr_ws_client->kr_client, kr_ws_client->krad_websocket->sysname)) {
        kr_client_destroy (&kr_ws_client->kr_client);
        return -1;
      }

      kr_ws_client->kr_client_info = 0;
      kr_ws_client->hello_sent = 0;
      kr_ws_client->destroy = 0;
      kr_mixer_info (kr_ws_client->kr_client);
      //kr_compositor_info (kr_ws_client->kr_client);
      kr_mixer_portgroup_list (kr_ws_client->kr_client);
      kr_compositor_subunit_list (kr_ws_client->kr_client);
      //kr_transponder_decklink_list (kr_ws_client->kr_client);
      //kr_transponder_list (kr_ws_client->kr_client);
      //kr_tags (kr_ws_client->kr_client, NULL);
      kr_subscribe_all (kr_ws_client->kr_client);
      add_poll_fd (krad_websocket, kr_client_get_fd (kr_ws_client->kr_client), POLLIN, KRAD_APP, kr_ws_client, NULL);
      break;

    case LWS_CALLBACK_CLOSED:
      //printk (" on kr client  LWS_CALLBACK_CLOSED");
      del_poll_fd (krad_websocket, kr_client_get_fd (kr_ws_client->kr_client));
      kr_client_destroy (&kr_ws_client->kr_client);
      kr_ws_client->hello_sent = 0;
      free (kr_ws_client->buffer);
      kr_ws_client->context = NULL;
      kr_ws_client->wsi = NULL;
      break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
    
      if (kr_ws_client->destroy == 1) {
        //printk ("got a writable callback for a connection with a buffer to damn full so we should get closed callback next.");
        return -1;
      }

      if (kr_ws_client->kr_client_info == 1) {
        //memcpy (p, kr_ws_client->msgstext, kr_ws_client->msgstextlen + 1);
        //ret = libwebsocket_write (wsi, p, kr_ws_client->msgstextlen, LWS_WRITE_TEXT);
        //ret = libwebsocket_write (wsi, (unsigned char *)kr_ws_client->msgstext, kr_ws_client->msgstextlen, LWS_WRITE_TEXT);
        ret = libwebsocket_write (wsi, (unsigned char *)kr_ws_client->msgz, kr_ws_client->msgstextlen, LWS_WRITE_TEXT);
        if (ret < 0) {
          printke ("krad ws ERROR writing to socket");
          return 1;
        }
        kr_ws_client->kr_client_info = 0;
        kr_ws_client->msgstextlen = 0;
        //free (kr_ws_client->msgstext);
      }
      break;
    case LWS_CALLBACK_RECEIVE:
      if (kr_ws_client->destroy == 1) {
        //printk ("got a RECEIVE callback for a connection with a buffer to damn full so we should get closed callback next.");
        return -1;
      }
      json_to_cmd (kr_ws_client, in, len);
      cjson_memreset ();
      break;
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
      printk (" on kr client  LWS_CALLBACK_CLIENT_CONNECTION_ERROR");
      break;
    case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
      printk (" on kr client  LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH");
      break;
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
      printk (" on kr client  LWS_CALLBACK_CLIENT_ESTABLISHED");
      break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
      printk (" on kr client  LWS_CALLBACK_CLIENT_RECEIVE");
      break;
    case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
      printk (" on kr client  LWS_CALLBACK_CLIENT_RECEIVE_PONG");
      break;
    case LWS_CALLBACK_CLIENT_WRITEABLE:
      printk (" on kr client  LWS_CALLBACK_CLIENT_WRITEABLE");
      break;
    case LWS_CALLBACK_HTTP:
      printk (" on kr client  LWS_CALLBACK_HTTP");
      break;
    case LWS_CALLBACK_HTTP_FILE_COMPLETION:
      printk (" on kr client  LWS_CALLBACK_HTTP_FILE_COMPLETION");
      break;
    case LWS_CALLBACK_HTTP_WRITEABLE:
      printk (" on kr client  LWS_CALLBACK_HTTP_WRITEABLE");
      break;
    case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
      printk (" on kr client  LWS_CALLBACK_FILTER_NETWORK_CONNECTION");
      break;
    case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
      printk (" on kr client  LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION");
      break;
    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
      printk (" on kr client  LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS");
      break;
    case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS:
      printk (" on kr client  LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS");
      break;
    case LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
      printk (" on kr client  LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION");
      break;
    case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
      printk (" on kr client  LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER");
      break;
    case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY:
      printk (" on kr client  LWS_CALLBACK_CONFIRM_EXTENSION_OKAY");
      break;
    case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
      printk (" on kr client  LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED");
      break;
    case LWS_CALLBACK_PROTOCOL_INIT:
      printk (" on kr client  LWS_CALLBACK_PROTOCOL_INIT");
      break;
    case LWS_CALLBACK_PROTOCOL_DESTROY:
      printk (" on kr client  LWS_CALLBACK_PROTOCOL_DESTROY");
      break;
    case LWS_CALLBACK_ADD_POLL_FD:
      printk (" on kr client  LWS_CALLBACK_ADD_POLL_FD");
      break;
    case LWS_CALLBACK_DEL_POLL_FD:
      printk (" on kr client  LWS_CALLBACK_DEL_POLL_FD");
      break;
    case LWS_CALLBACK_SET_MODE_POLL_FD:
      printk (" on kr client  LWS_CALLBACK_SET_MODE_POLL_FD");
      break;
    case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
      printk (" on kr client  LWS_CALLBACK_CLEAR_MODE_POLL_FD");
      break;
    default:
      printke ("callback_kr_client for unknown reason! %d", reason);
      break;
  }

  return 0;
}

static void *krad_websocket_server_run (void *arg) {

  krad_websocket_t *krad_websocket = (krad_websocket_t *)arg;
  int n;
  kr_ws_client_t *kr_ws_client;
  char *str;
  
  krad_system_set_thread_name ("kr_websocket");

  n = 0;
  kr_ws_client = NULL;
  krad_websocket->shutdown = KRAD_WEBSOCKET_RUNNING;

  while (!krad_websocket->shutdown) {

    n = poll (krad_websocket->pollfds, krad_websocket->count_pollfds, -1);
    
    if ((n < 0) || (krad_websocket->shutdown)) {
      break;
    }

    if (n > 0) {

      if (krad_websocket->pollfds[0].revents) {
        break;
      }

      for (n = 1; n < krad_websocket->count_pollfds; n++) {
        
        if (krad_websocket->pollfds[n].revents) {
        
          if (krad_websocket->pollfds[n].revents && (krad_websocket->fdof[n] == MYSTERY)) {
            libwebsocket_service_fd (krad_websocket->context, &krad_websocket->pollfds[n]);  
            continue;
          }
        
          if ((krad_websocket->pollfds[n].revents & POLLERR) || (krad_websocket->pollfds[n].revents & POLLHUP)) {
              
            if (krad_websocket->pollfds[n].revents & POLLERR) {
            }
          
            if (krad_websocket->pollfds[n].revents & POLLHUP) {
            }
            
            switch ( krad_websocket->fdof[n] ) {
              case KRAD_APP:
              case KRAD_CONTROLLER:
              case MYSTERY:
                break;
            }
          
          } else {
          
            if (krad_websocket->pollfds[n].revents & POLLIN) {

              switch ( krad_websocket->fdof[n] ) {
                case KRAD_APP:

                  kr_ws_client = krad_websocket->sessions[n];

                  kr_ws_client->msgs = cJSON_CreateArray();
                  cJSON *msg;

                  if (kr_ws_client->hello_sent == 0) {
                    cJSON_AddItemToArray (kr_ws_client->msgs, msg = cJSON_CreateObject());
                    cJSON_AddStringToObject (msg, "com", "kradradio");
                    cJSON_AddStringToObject (msg, "info", "sysname");
                    cJSON_AddStringToObject (msg, "infoval", krad_websocket->sysname);
                    kr_ws_client->hello_sent = 1;
                    usleep (25000);
                  }
                  
                  kr_delivery_recv (kr_ws_client->kr_client);
                  krad_delivery_handler (kr_ws_client);

                  if ((cJSON_GetArraySize(kr_ws_client->msgs) > 0) && (kr_ws_client->destroy == 0)) {
                    kr_ws_client->msgz = (char *)&kr_ws_client->buffer[LWS_SEND_BUFFER_PRE_PADDING];
                    if (kr_ws_client->msgstextlen > 0) {
                      kr_ws_client->buffer[LWS_SEND_BUFFER_PRE_PADDING + kr_ws_client->msgstextlen - 1] = ',';
                      kr_ws_client->msgstext = (char *)&kr_ws_client->buffer[LWS_SEND_BUFFER_PRE_PADDING] + kr_ws_client->msgstextlen;
                      str = cJSON_Print (kr_ws_client->msgs) + 1;
                      if (strlen(str) + kr_ws_client->msgstextlen >= KR_WS_BUFSIZE - 1024) {
                        printke ("the buffer is too damn full!");
                        kr_ws_client->destroy = 1;
                      } else {
                        strcpy (kr_ws_client->msgstext, str);
                      }
                    } else {
                      kr_ws_client->msgstext = (char *)&kr_ws_client->buffer[LWS_SEND_BUFFER_PRE_PADDING];
                      str = cJSON_Print (kr_ws_client->msgs);
                      strcpy (kr_ws_client->msgstext, str);
                    }

                    //printk(kr_ws_client->msgstext);
                    //if (kr_ws_client->msgstextlen > 0) {
                    //  printk("oh shit!");
                    //}
                    
                    kr_ws_client->msgstextlen += strlen (kr_ws_client->msgstext);
                    
                  }
                  cjson_memreset ();
                  if (kr_ws_client->msgstextlen > 0) {
                    kr_ws_client->kr_client_info = 1;
                  }
                  if ((kr_ws_client->kr_client_info == 1) || (kr_ws_client->destroy == 1)) {
                    libwebsocket_callback_on_writable (kr_ws_client->context, kr_ws_client->wsi);
                  }
                  break;
                case KRAD_CONTROLLER:
                case MYSTERY:
                  break;
              }
            }
            
            if (krad_websocket->pollfds[n].revents & POLLOUT) {
              switch ( krad_websocket->fdof[n] ) {
                case KRAD_APP:
                case KRAD_CONTROLLER:
                case MYSTERY:
                  break;
              }
            }
          }
        }
      }
    }
  }
  
  krad_websocket->shutdown = KRAD_WEBSOCKET_SHUTINGDOWN;
  krad_controller_client_close (&krad_websocket->krad_control);
  
  return NULL;
}

void krad_websocket_server_destroy (krad_websocket_t *krad_websocket) {

  if (krad_websocket != NULL) {
    printk ("Krad Websocket shutdown started");  
    krad_websocket->shutdown = KRAD_WEBSOCKET_DO_SHUTDOWN;
    if (!krad_controller_shutdown (&krad_websocket->krad_control, &krad_websocket->server_thread, 30)) {
      krad_controller_destroy (&krad_websocket->krad_control, &krad_websocket->server_thread);
    }
    //free (krad_websocket->buffer);
    libwebsocket_context_destroy (krad_websocket->context);
    free (krad_websocket);
    printk ("Krad Websocket shutdown complete");
  }
}

krad_websocket_t *krad_websocket_server_create (char *sysname, int port) {

  krad_websocket_t *krad_websocket = calloc (1, sizeof (krad_websocket_t));
  struct lws_context_creation_info lws_create;
  
  krad_websocket->shutdown = KRAD_WEBSOCKET_STARTING;
  krad_websocket->port = port;
  strcpy (krad_websocket->sysname, sysname);

  if (krad_control_init (&krad_websocket->krad_control)) {
    free (krad_websocket);
    return NULL;
  }

  add_poll_fd (krad_websocket, krad_controller_get_client_fd (&krad_websocket->krad_control),
               POLLIN, KRAD_CONTROLLER, NULL, NULL);

  memset (&lws_create, 0, sizeof(struct lws_context_creation_info));
  lws_create.user = krad_websocket;
  lws_create.port = krad_websocket->port;
  lws_create.protocols = protocols;
  lws_create.extensions = NULL;
  lws_create.gid = -1;
  lws_create.uid = -1;

  krad_websocket->context = libwebsocket_create_context (&lws_create);

  if (krad_websocket->context == NULL) {
    printke ("libwebsocket init failed");
    krad_websocket_server_destroy (krad_websocket);
    return NULL;
  }
  
  pthread_create (&krad_websocket->server_thread, NULL, krad_websocket_server_run, (void *)krad_websocket);

  return krad_websocket;
}
