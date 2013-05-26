#include "krad_compositor_interface.h"

void krad_compositor_videoport_rep_to_ebml2 (kr_port_t *port, kr_ebml2_t *ebml) {
  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_COMPOSITOR_TEXT, port->sysname);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, port->direction);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, port->controls.x);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->controls.y);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->controls.z);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->source_width);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->source_height);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->crop_x);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->crop_y);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->crop_width);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->crop_height);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->controls.width);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->controls.height);  
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, port->controls.opacity);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, port->controls.rotation);
}

void krad_compositor_text_rep_to_ebml2 (krad_text_rep_t *text, kr_ebml2_t *ebml) {

  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_COMPOSITOR_TEXT, text->text);
  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_COMPOSITOR_FONT, text->font);
  kr_ebml2_pack_float (ebml,
                       EBML_ID_KRAD_COMPOSITOR_RED,
                       text->red);
  kr_ebml2_pack_float (ebml,
                       EBML_ID_KRAD_COMPOSITOR_GREEN,
                       text->green);
  kr_ebml2_pack_float (ebml,
                       EBML_ID_KRAD_COMPOSITOR_BLUE,
                       text->blue);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, text->controls.x);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.y);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.z);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.width);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.height);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, text->controls.xscale);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, text->controls.opacity);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, text->controls.rotation);
}

void krad_compositor_sprite_rep_to_ebml2 (krad_sprite_rep_t *sprite, kr_ebml2_t *ebml) {
  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_COMPOSITOR_FILENAME, sprite->filename);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, sprite->controls.x);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.y);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.z);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.width);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.height);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_TICKRATE, sprite->controls.tickrate);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, sprite->controls.xscale);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, sprite->controls.yscale);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, sprite->controls.opacity);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, sprite->controls.rotation);
}

void krad_compositor_vector_rep_to_ebml2 (krad_vector_rep_t *vector, kr_ebml2_t *ebml) {
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, vector->type);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.width);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.height);  
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, vector->controls.x);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.y);
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.z);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, vector->controls.xscale);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, vector->controls.yscale);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, vector->controls.opacity);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, vector->controls.rotation);
}

int krad_videoport_to_rep (krad_compositor_port_t *videoport, kr_port_t *videoport_rep) {
  
  if ((videoport == NULL) || (videoport_rep == NULL)) {
    return 0;
  }

  strncpy (videoport_rep->sysname, videoport->sysname, sizeof(videoport_rep->sysname));
  videoport_rep->direction = videoport->direction;
  videoport_rep->controls.x = videoport->subunit.x;
  videoport_rep->controls.y = videoport->subunit.y;
  videoport_rep->controls.z = videoport->subunit.z;
  videoport_rep->source_width = videoport->source_width;
  videoport_rep->source_height = videoport->source_height;
  videoport_rep->crop_x = videoport->crop_x;
  videoport_rep->crop_y = videoport->crop_y;
  videoport_rep->crop_width = videoport->crop_width;
  videoport_rep->crop_height = videoport->crop_height;
  videoport_rep->controls.width = videoport->subunit.width;
  videoport_rep->controls.height = videoport->subunit.height;
  videoport_rep->controls.rotation = videoport->subunit.rotation;
  videoport_rep->controls.opacity = videoport->subunit.opacity;
   
  return 1;
}

void krad_compositor_to_ebml ( kr_ebml2_t *ebml, krad_compositor_t *krad_compositor ) {

  char *name;

  name = "";

  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_WIDTH, krad_compositor->width);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_HEIGHT, krad_compositor->height);  
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_FPS_NUMERATOR,
                                   krad_compositor->fps_numerator);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_FPS_DENOMINATOR,
                                   krad_compositor->fps_denominator);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_COUNT,
                                   krad_compositor->active_sprites);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_TEXT_COUNT,
                                   krad_compositor->active_texts);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_VECTOR_COUNT,
                                   krad_compositor->active_vectors);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_PORT_COUNT,
                                   krad_compositor->active_input_ports);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_PORT_COUNT,
                                   krad_compositor->active_output_ports);
  kr_ebml2_pack_uint32 (ebml, EBML_ID_KRAD_COMPOSITOR_PORT_COUNT,
                                   krad_compositor->frames);
  krad_compositor_get_background_name (krad_compositor, &name);
  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_COMPOSITOR_LAST_SNAPSHOT_NAME, name);
}

void krad_compositor_sprite_to_ebml2 ( kr_ebml2_t *ebml, krad_sprite_t *sprite ) {
  krad_sprite_rep_t sprite_rep;
  krad_sprite_to_rep (sprite, &sprite_rep);
  krad_compositor_sprite_rep_to_ebml2 (&sprite_rep, ebml);
}

void krad_compositor_text_to_ebml2 ( kr_ebml2_t *ebml, krad_text_t *text ) {
  krad_text_rep_t text_rep;
  krad_text_to_rep (text, &text_rep);
  krad_compositor_text_rep_to_ebml2 (&text_rep, ebml);
}

void krad_compositor_vector_to_ebml2 ( kr_ebml2_t *ebml, krad_vector_t *vector ) {
  krad_vector_rep_t vector_rep;
  krad_vector_to_rep (vector, &vector_rep);
  krad_compositor_vector_rep_to_ebml2 (&vector_rep, ebml);
}

void krad_compositor_videoport_to_ebml2 ( kr_ebml2_t *ebml, krad_compositor_port_t *videoport ) {
  kr_port_t videoport_rep;
  krad_videoport_to_rep (videoport, &videoport_rep);
  krad_compositor_videoport_rep_to_ebml2 (&videoport_rep, ebml);
}

int krad_compositor_command ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client ) {

  kr_unit_control_t unit_control;
  int s;
  int type;
  krad_radio_t *krad_radio;
  krad_compositor_t *krad_compositor;
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
  char string2[512];
  uint32_t numbers[10];
  int32_t direction;
  krad_app_server_t *app;

  krad_radio = client->krad_radio;
  krad_compositor = krad_radio->krad_compositor;
  app = krad_radio->app;
  s = 0;
  string[0] = '\0';
  string2[0] = '\0';

  if (!(kr_io2_has_in (in))) {
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_in, in->rd_buf, in->len );

  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if ((ret < 0) || (command != EBML_ID_KRAD_COMPOSITOR_CMD)) {
    printke ("krad_mixer_command invalid EBML ID Not found");
    return 0;
  }

  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if (ret < 0) {
    printke ("krad_mixer_command EBML ID Not found");
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_out, out->buf, out->space );

  printk ("comp command");

  switch ( command ) {
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_SUBUNIT:
      unit_control.address.path.unit = KR_COMPOSITOR;
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      unit_control.address.path.subunit.compositor_subunit = numbers[0];
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      unit_control.address.id.number = numbers[0];
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      unit_control.address.control.compositor_control = numbers[0];
      kr_unit_control_data_type_from_address (&unit_control.address, &unit_control.data_type);
      if (unit_control.data_type == KR_FLOAT) {
        kr_ebml2_unpack_element_float (&ebml_in, &element, &unit_control.value.real);
      }
      if (unit_control.data_type == KR_INT32) {
        kr_ebml2_unpack_element_int32 (&ebml_in, &element, &unit_control.value.integer);
      }
      if (unit_control.data_type == KR_STRING) {
        //bah
      }
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &unit_control.duration);
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[1]);
      //FIXME   test to see if subunit exists before broadcasting update to
      //        a phantom subunit
      krad_compositor_subunit_update (krad_compositor, &unit_control);
      if (unit_control.data_type == KR_FLOAT) {
        krad_radio_broadcast_subunit_update ( app->app_broadcaster, &unit_control.address, unit_control.address.control.compositor_control,
                                              unit_control.data_type, (void *)&unit_control.value.real, app->current_client );
      }
      if (unit_control.data_type == KR_INT32) {
        krad_radio_broadcast_subunit_update ( app->app_broadcaster, &unit_control.address, unit_control.address.control.compositor_control,
                                              unit_control.data_type, (void *)&unit_control.value.integer, app->current_client );
      }
      break;  
    case EBML_ID_KRAD_COMPOSITOR_CMD_REMOVE_SUBUNIT:
      address.path.unit = KR_COMPOSITOR;
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      address.path.subunit.compositor_subunit = numbers[0];
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      address.id.number = numbers[0];
      if (krad_compositor_subunit_destroy (krad_compositor, &address)) {
        krad_radio_broadcast_subunit_destroyed (app->app_broadcaster, &address);
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_ADD_SUBUNIT:
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      type = numbers[0];
      kr_ebml2_unpack_element_string (&ebml_in, &element, string, sizeof(string));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string2, sizeof(string2));
      s = krad_compositor_subunit_create (krad_compositor, type, string, string2);
      if (s > -1) {
        if (type == KR_SPRITE) {
          krad_radio_broadcast_subunit_created ( app->app_broadcaster,
                                                 &krad_compositor->sprite[s].subunit.address,
                                                 (void *)&krad_compositor->sprite[s]);        
        }
        if (type == KR_TEXT) {
          krad_radio_broadcast_subunit_created ( app->app_broadcaster,
                                                 &krad_compositor->text[s].subunit.address,
                                                 (void *)&krad_compositor->text[s]);
        }
        if (type == KR_VECTOR) {
          krad_radio_broadcast_subunit_created ( app->app_broadcaster,
                                                 &krad_compositor->vector[s].subunit.address,
                                                 (void *)&krad_compositor->vector[s]);        
        }
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_LIST_SUBUNITS:
      for (s = 0; s < KC_MAX_SPRITES; s++) {
        if (krad_compositor->sprite[s].subunit.active == 1) {
          krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_compositor->sprite[s].subunit.address);
          kr_ebml2_pack_uint32 ( &ebml_out,
                                 EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                 EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          krad_compositor_sprite_to_ebml2 ( &ebml_out, &krad_compositor->sprite[s]);
          kr_ebml2_finish_element (&ebml_out, payload);
          kr_ebml2_finish_element (&ebml_out, response);
        }
      }
      for (s = 0; s < KC_MAX_TEXTS; s++) {
        if (krad_compositor->text[s].subunit.active == 1) {
          krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_compositor->text[s].subunit.address);
          kr_ebml2_pack_uint32 ( &ebml_out,
                                 EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                 EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          krad_compositor_text_to_ebml2 ( &ebml_out, &krad_compositor->text[s]);
          kr_ebml2_finish_element (&ebml_out, payload);
          kr_ebml2_finish_element (&ebml_out, response);
        }
      }
      for (s = 0; s < KC_MAX_VECTORS; s++) {
        if (krad_compositor->vector[s].subunit.active == 1) {
          krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_compositor->vector[s].subunit.address);
          kr_ebml2_pack_uint32 ( &ebml_out,
                                 EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                 EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          krad_compositor_vector_to_ebml2 ( &ebml_out, &krad_compositor->vector[s]);
          kr_ebml2_finish_element (&ebml_out, payload);
          kr_ebml2_finish_element (&ebml_out, response);
        }
      }
      for (s = 0; s < KC_MAX_PORTS; s++) {
        if (krad_compositor->port[s].subunit.active == 1) {
          krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_compositor->port[s].subunit.address);
          kr_ebml2_pack_uint32 ( &ebml_out,
                                 EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                 EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          krad_compositor_videoport_to_ebml2 ( &ebml_out, &krad_compositor->port[s]);
          kr_ebml2_finish_element (&ebml_out, payload);
          kr_ebml2_finish_element (&ebml_out, response);
        }
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SUBUNIT_INFO:
      address.path.unit = KR_COMPOSITOR;
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      address.path.subunit.compositor_subunit = numbers[0];
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      address.id.number = numbers[0];
      s = address.id.number;
      switch (address.path.subunit.compositor_subunit) {
        case KR_VIDEOPORT:
          if (krad_compositor->port[s].subunit.active == 1) {
            krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_compositor->port[s].subunit.address);
            kr_ebml2_pack_uint32 ( &ebml_out,
                                   EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                   EBML_ID_KRAD_SUBUNIT_INFO);
            kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
            krad_compositor_videoport_to_ebml2 ( &ebml_out, &krad_compositor->port[s]);
            kr_ebml2_finish_element (&ebml_out, payload);
            kr_ebml2_finish_element (&ebml_out, response);
          }
          break;
        case KR_SPRITE:
          if (krad_compositor->sprite[s].subunit.active == 1) {
            krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_compositor->sprite[s].subunit.address);
            kr_ebml2_pack_uint32 ( &ebml_out,
                                   EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                   EBML_ID_KRAD_SUBUNIT_INFO);
            kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
            krad_compositor_sprite_to_ebml2 ( &ebml_out, &krad_compositor->sprite[s]);
            kr_ebml2_finish_element (&ebml_out, payload);
            kr_ebml2_finish_element (&ebml_out, response);
          }
          break;
        case KR_TEXT:
          if (krad_compositor->text[s].subunit.active == 1) {
            krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_compositor->text[s].subunit.address);
            kr_ebml2_pack_uint32 ( &ebml_out,
                                   EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                   EBML_ID_KRAD_SUBUNIT_INFO);
            kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
            krad_compositor_text_to_ebml2 ( &ebml_out, &krad_compositor->text[s]);
            kr_ebml2_finish_element (&ebml_out, payload);
            kr_ebml2_finish_element (&ebml_out, response);
          }
          break;
        case KR_VECTOR:
          if (krad_compositor->vector[s].subunit.active == 1) {
            krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_compositor->vector[s].subunit.address);
            kr_ebml2_pack_uint32 ( &ebml_out,
                                   EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                   EBML_ID_KRAD_SUBUNIT_INFO);
            kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
            krad_compositor_vector_to_ebml2 ( &ebml_out, &krad_compositor->vector[s]);
            kr_ebml2_finish_element (&ebml_out, payload);
            kr_ebml2_finish_element (&ebml_out, response);
          }
          break;
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_INFO:
    
      krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_compositor->address);
      kr_ebml2_pack_uint32 ( &ebml_out,
                             EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                             EBML_ID_KRAD_UNIT_INFO);
      kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      krad_compositor_to_ebml (&ebml_out, krad_compositor);
      kr_ebml2_finish_element (&ebml_out, payload);
      kr_ebml2_finish_element (&ebml_out, response);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_BACKGROUND:
      kr_ebml2_unpack_element_string (&ebml_in, &element, string, sizeof(string));
      krad_compositor_set_background (krad_compositor, string);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_FRAME_RATE:
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[1]);
      printke ("Krad Compositor: FIXME Set frame rate - %d / %d", numbers[0], numbers[1]);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_RESOLUTION:
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[1]);
      krad_compositor_set_resolution (krad_compositor, numbers[0], numbers[1]);
      break;
      
      
    int p;
    int sd1;
    int sd2;
      
    case EBML_ID_KRAD_COMPOSITOR_CMD_LOCAL_VIDEOPORT_DESTROY:
      for (p = 0; p < KC_MAX_PORTS; p++) {
        if (krad_compositor->port[p].local == 1) {
          krad_compositor_port_destroy (krad_compositor, &krad_compositor->port[p]);
          break;
        }
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_LOCAL_VIDEOPORT_CREATE:
    printk ("VID command!!");

      kr_ebml2_unpack_element_int32 (&ebml_in, &element, &direction);

      krad_system_set_socket_blocking (app->current_client->sd);
    
      sd1 = krad_app_server_recvfd (app->current_client);
      sd2 = krad_app_server_recvfd (app->current_client);
      printk ("VIDEOPORT_CREATE Got FD's %d and %d\n", sd1, sd2);
      krad_compositor_local_port_create (krad_compositor,
                                         "localport",
                                         direction, sd1, sd2);
      
      krad_system_set_socket_nonblocking (app->current_client->sd);
      
      break;
      
      
      
    default:
      return -1;    
  }

  if (((ebml_out.pos > 0) || (command == EBML_ID_KRAD_COMPOSITOR_CMD_LIST_SUBUNITS)) &&
       (!krad_app_server_current_client_is_subscriber (app))) {
    krad_radio_pack_shipment_terminator (&ebml_out);
  }

  kr_io2_pulled (in, ebml_in.pos);
  kr_io2_advance (out, ebml_out.pos);

  return 0;
}

