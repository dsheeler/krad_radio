#include "krad_compositor_interface.h"

void path_info_to_ebml(kr_ebml *e, kr_compositor_path_info *info) {
  kr_ebml2_pack_string(e, EBML_ID_KRAD_COMPOSITOR_TEXT, info->sysname);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_X, info->type);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_X, info->controls.x);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->controls.y);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->controls.z);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->source_width);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->source_height);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->crop_x);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->crop_y);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->crop_width);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->crop_height);
  kr_ebml2_pack_uint32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->view.top_left.x);
  kr_ebml2_pack_uint32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->view.top_left.y);
  kr_ebml2_pack_uint32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->view.top_right.x);
  kr_ebml2_pack_uint32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->view.top_right.y);
  kr_ebml2_pack_uint32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->view.bottom_left.x);
  kr_ebml2_pack_uint32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->view.bottom_left.y);
  kr_ebml2_pack_uint32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->view.bottom_right.x);
  kr_ebml2_pack_uint32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->view.bottom_right.y);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->controls.width);
  kr_ebml2_pack_int32(e, EBML_ID_KRAD_COMPOSITOR_Y, info->controls.height);
  kr_ebml2_pack_float(e, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, info->controls.opacity);
  kr_ebml2_pack_float(e, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, info->controls.rotation);
}

void kr_text_info_to_ebml(kr_ebml *ebml, kr_text_info *text) {
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_COMPOSITOR_TEXT, text->text);
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_COMPOSITOR_FONT, text->font);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_RED, text->red);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_GREEN, text->green);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_BLUE, text->blue);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_X, text->controls.x);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.y);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.z);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.width);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.height);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, text->controls.opacity);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, text->controls.rotation);
}

void kr_sprite_info_to_ebml(kr_ebml *ebml, kr_sprite_info *sprite) {
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_COMPOSITOR_FILENAME, sprite->filename);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_X, sprite->controls.x);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.y);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.z);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.width);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.height);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_TICKRATE, sprite->controls.tickrate);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, sprite->controls.opacity);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, sprite->controls.rotation);
}

void kr_vector_info_to_ebml(kr_ebml *ebml, kr_vector_info *vector) {
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_X, vector->type);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.width);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.height);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_X, vector->controls.x);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.y);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.z);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, vector->controls.opacity);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, vector->controls.rotation);
}

void kr_compositor_to_ebml(kr_ebml *ebml, kr_compositor *compositor) {

  char *name;

  name = "";

  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_WIDTH, compositor->width);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_HEIGHT, compositor->height);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_FPS_NUMERATOR,
   compositor->fps_numerator);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_FPS_DENOMINATOR,
   compositor->fps_denominator);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_COUNT,
   compositor->active_sprites);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_TEXT_COUNT,
   compositor->active_texts);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_VECTOR_COUNT,
   compositor->active_vectors);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_PORT_COUNT,
   compositor->active_input_paths);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_PORT_COUNT,
   compositor->active_output_paths);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_PORT_COUNT,
   compositor->frames);
  kr_compositor_background(compositor, &name);
  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_COMPOSITOR_LAST_SNAPSHOT_NAME, name);
}

void kr_sprite_to_ebml(kr_ebml *ebml, kr_sprite *sprite) {
  kr_sprite_info sprite_rep;
  kr_sprite_to_info(sprite, &sprite_rep);
  kr_sprite_info_to_ebml(ebml, &sprite_rep);
}

void kr_text_to_ebml(kr_ebml *ebml, kr_text *text) {
  kr_text_info text_rep;
  kr_text_to_info(text, &text_rep);
  kr_text_info_to_ebml(ebml, &text_rep);
}

void kr_vector_to_ebml(kr_ebml *ebml, kr_vector *vector) {
  kr_vector_info vector_rep;
  kr_vector_to_info(vector, &vector_rep);
  kr_vector_info_to_ebml(ebml, &vector_rep);
}

void kr_compositor_path_to_ebml(kr_ebml *ebml, kr_compositor_path *path) {
  path_info_to_ebml(ebml, &path->info);
}

int kr_compositor_cmd(kr_io2_t *in, kr_io2_t *out, kr_radio_client *client) {

  kr_unit_control_t unit_control;
  int s;
  int i;
  int type;
  kr_radio *radio;
  kr_compositor *compositor;
  kr_address_t address;
  kr_compositor_path *path;
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
  kr_app_server *app;

  address.path.unit = KR_COMPOSITOR;
  radio = client->krad_radio;
  compositor = radio->compositor;
  app = radio->app;
  s = 0;
  string[0] = '\0';
  string2[0] = '\0';

  if (!(kr_io2_has_in (in))) {
    return 0;
  }

  kr_ebml2_set_buffer(&ebml_in, in->rd_buf, in->len);

  ret = kr_ebml2_unpack_id(&ebml_in, &command, &size);
  if ((ret < 0) || (command != EBML_ID_KRAD_COMPOSITOR_CMD)) {
    printke ("krad_mixer_command invalid EBML ID Not found");
    return 0;
  }

  ret = kr_ebml2_unpack_id(&ebml_in, &command, &size);
  if (ret < 0) {
    printke ("krad_mixer_command EBML ID Not found");
    return 0;
  }

  kr_ebml2_set_buffer(&ebml_out, out->buf, out->space);

  switch (command) {
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_SUBUNIT:
      unit_control.address.path.unit = KR_COMPOSITOR;
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      unit_control.address.path.subunit.compositor_subunit = numbers[0];
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      unit_control.address.id.number = numbers[0];
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      unit_control.address.control.compositor_control = numbers[0];
      kr_unit_control_data_type_from_address(&unit_control.address, &unit_control.data_type);
      if (unit_control.data_type == KR_FLOAT) {
        kr_ebml2_unpack_element_float(&ebml_in, &element, &unit_control.value.real);
      }
      if (unit_control.data_type == KR_INT32) {
        kr_ebml2_unpack_element_int32(&ebml_in, &element, &unit_control.value.integer);
      }
      if (unit_control.data_type == KR_STRING) {
        //bah
      }
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &unit_control.duration);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[1]);
      //FIXME   test to see if subunit exists before broadcasting update to
      //        a phantom subunit
      krad_compositor_subunit_update(compositor, &unit_control);
      if (unit_control.data_type == KR_FLOAT) {
        krad_radio_broadcast_subunit_update(app->app_broadcaster, &unit_control.address, unit_control.address.control.compositor_control,
                                              unit_control.data_type, (void *)&unit_control.value.real, app->current_client );
      }
      if (unit_control.data_type == KR_INT32) {
        krad_radio_broadcast_subunit_update(app->app_broadcaster, &unit_control.address, unit_control.address.control.compositor_control,
                                            unit_control.data_type, (void *)&unit_control.value.integer, app->current_client );
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_REMOVE_SUBUNIT:
      address.path.unit = KR_COMPOSITOR;
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      address.path.subunit.compositor_subunit = numbers[0];
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      address.id.number = numbers[0];
      if(krad_compositor_subunit_destroy(compositor, &address)) {
        krad_radio_broadcast_subunit_destroyed(app->app_broadcaster, &address);
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_ADD_SUBUNIT:
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      type = numbers[0];
      kr_ebml2_unpack_element_string(&ebml_in, &element, string, sizeof(string));
      kr_ebml2_unpack_element_string(&ebml_in, &element, string2, sizeof(string2));
      s = krad_compositor_subunit_create(compositor, type, string, string2);
      if (s > -1) {
        if (type == KR_SPRITE) {
          krad_radio_broadcast_subunit_created(app->app_broadcaster,
                                               &compositor->sprite[s].subunit.address,
                                               (void *)&compositor->sprite[s]);
        }
        if (type == KR_TEXT) {
          krad_radio_broadcast_subunit_created(app->app_broadcaster,
                                               &compositor->text[s].subunit.address,
                                               (void *)&compositor->text[s]);
        }
        if (type == KR_VECTOR) {
          krad_radio_broadcast_subunit_created(app->app_broadcaster,
                                               &compositor->vector[s].subunit.address,
                                               (void *)&compositor->vector[s]);
        }
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_LIST_SUBUNITS:
      for (s = 0; s < KC_MAX_SPRITES; s++) {
        if(compositor->sprite[s].subunit.active == 1) {
          krad_radio_address_to_ebml2(&ebml_out, &response, &compositor->sprite[s].subunit.address);
          kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          kr_sprite_to_ebml(&ebml_out, &compositor->sprite[s]);
          kr_ebml2_finish_element(&ebml_out, payload);
          kr_ebml2_finish_element(&ebml_out, response);
        }
      }
      for (s = 0; s < KC_MAX_TEXTS; s++) {
        if(compositor->text[s].subunit.active == 1) {
          krad_radio_address_to_ebml2(&ebml_out, &response, &compositor->text[s].subunit.address);
          kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          kr_text_to_ebml(&ebml_out, &compositor->text[s]);
          kr_ebml2_finish_element(&ebml_out, payload);
          kr_ebml2_finish_element(&ebml_out, response);
        }
      }
      for (s = 0; s < KC_MAX_VECTORS; s++) {
        if(compositor->vector[s].subunit.active == 1) {
          krad_radio_address_to_ebml2(&ebml_out, &response, &compositor->vector[s].subunit.address);
          kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          kr_vector_to_ebml(&ebml_out, &compositor->vector[s]);
          kr_ebml2_finish_element(&ebml_out, payload);
          kr_ebml2_finish_element(&ebml_out, response);
        }
      }
      address.path.subunit.compositor_subunit = KR_VIDEOPORT;
      i = 0;
      while ((path = kr_pool_iterate_active(compositor->path_pool, &i))) {
        address.id.number = i - 1;
        krad_radio_address_to_ebml2(&ebml_out, &response, &address);
        kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_INFO);
        kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        kr_compositor_path_to_ebml(&ebml_out, path);
        kr_ebml2_finish_element(&ebml_out, payload);
        kr_ebml2_finish_element(&ebml_out, response);
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SUBUNIT_INFO:
      address.path.unit = KR_COMPOSITOR;
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      address.path.subunit.compositor_subunit = numbers[0];
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      address.id.number = numbers[0];
      s = address.id.number;
      switch (address.path.subunit.compositor_subunit) {
        case KR_VIDEOPORT:
          /*
          if(compositor->path[s].subunit.active == 1) {
            krad_radio_address_to_ebml2(&ebml_out, &response, &compositor->path[s].subunit.address);
            kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_INFO);
            kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
            kr_compositor_path_to_ebml(&ebml_out, &compositor->path[s]);
            kr_ebml2_finish_element(&ebml_out, payload);
            kr_ebml2_finish_element(&ebml_out, response);
          }
          */
          break;
        case KR_SPRITE:
          if(compositor->sprite[s].subunit.active == 1) {
            krad_radio_address_to_ebml2(&ebml_out, &response, &compositor->sprite[s].subunit.address);
            kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_INFO);
            kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
            kr_sprite_to_ebml(&ebml_out, &compositor->sprite[s]);
            kr_ebml2_finish_element(&ebml_out, payload);
            kr_ebml2_finish_element(&ebml_out, response);
          }
          break;
        case KR_TEXT:
          if(compositor->text[s].subunit.active == 1) {
            krad_radio_address_to_ebml2(&ebml_out, &response, &compositor->text[s].subunit.address);
            kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_INFO);
            kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
            kr_text_to_ebml(&ebml_out, &compositor->text[s]);
            kr_ebml2_finish_element(&ebml_out, payload);
            kr_ebml2_finish_element(&ebml_out, response);
          }
          break;
        case KR_VECTOR:
          if(compositor->vector[s].subunit.active == 1) {
            krad_radio_address_to_ebml2(&ebml_out, &response, &compositor->vector[s].subunit.address);
            kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_INFO);
            kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
            kr_vector_to_ebml(&ebml_out, &compositor->vector[s]);
            kr_ebml2_finish_element(&ebml_out, payload);
            kr_ebml2_finish_element(&ebml_out, response);
          }
          break;
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_INFO:
      address.path.unit = KR_COMPOSITOR;
      address.path.subunit.compositor_subunit = KR_UNIT;
      krad_radio_address_to_ebml2(&ebml_out, &response, &address);
      kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
       EBML_ID_KRAD_UNIT_INFO);
      kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      kr_compositor_to_ebml(&ebml_out, compositor);
      kr_ebml2_finish_element(&ebml_out, payload);
      kr_ebml2_finish_element(&ebml_out, response);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_BACKGROUND:
      kr_ebml2_unpack_element_string (&ebml_in, &element, string, sizeof(string));
      kr_compositor_background_set(compositor, string);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_FRAME_RATE:
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[1]);
      printke("Krad Compositor: FIXME Set frame rate - %d / %d", numbers[0],
       numbers[1]);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_RESOLUTION:
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_uint32(&ebml_in, &element, &numbers[1]);
      kr_compositor_resolution_set(compositor, numbers[0], numbers[1]);
      break;
    default:
      return -1;
  }

  if (((ebml_out.pos > 0)
      || (command == EBML_ID_KRAD_COMPOSITOR_CMD_LIST_SUBUNITS))
      && (!krad_app_server_current_client_is_subscriber(app))) {
    krad_radio_pack_shipment_terminator(&ebml_out);
  }

  kr_io2_pulled(in, ebml_in.pos);
  kr_io2_advance(out, ebml_out.pos);

  return 0;
}
