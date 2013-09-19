#include "krad_compositor_interface.h"

void kr_compositor_path_info_to_ebml(kr_compositor_path_info *path, kr_ebml *ebml) {
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_COMPOSITOR_TEXT, path->sysname);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_X, path->type);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_X, path->controls.x);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->controls.y);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->controls.z);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->source_width);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->source_height);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->crop_x);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->crop_y);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->crop_width);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->crop_height);

  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->view.top_left.x);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->view.top_left.y);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->view.top_right.x);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->view.top_right.y);

  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->view.bottom_left.x);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->view.bottom_left.y);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->view.bottom_right.x);
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->view.bottom_right.y);

  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->controls.width);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, path->controls.height);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, path->controls.opacity);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, path->controls.rotation);
}

void kr_text_info_to_ebml(kr_text_info *text, kr_ebml *ebml) {
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

void kr_sprite_info_to_ebml(kr_sprite_info *sprite, kr_ebml *ebml) {
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

void kr_vector_info_to_ebml(kr_vector_info *vector, kr_ebml *ebml) {
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_X, vector->type);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.width);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.height);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_X, vector->controls.x);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.y);
  kr_ebml2_pack_int32(ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.z);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, vector->controls.opacity);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, vector->controls.rotation);
}

int kr_compositor_path_to_info(kr_compositor_path *videopath, kr_compositor_path_info *videopath_rep) {

  if ((videopath == NULL) || (videopath_rep == NULL)) {
    return 0;
  }

  strncpy (videopath_rep->sysname, videopath->sysname, sizeof(videopath_rep->sysname));
  videopath_rep->type = videopath->type;
  videopath_rep->controls.x = videopath->subunit.x;
  videopath_rep->controls.y = videopath->subunit.y;
  videopath_rep->controls.z = videopath->subunit.z;
  videopath_rep->source_width = videopath->source_width;
  videopath_rep->source_height = videopath->source_height;
  videopath_rep->crop_x = videopath->crop_x;
  videopath_rep->crop_y = videopath->crop_y;
  videopath_rep->crop_width = videopath->crop_width;
  videopath_rep->crop_height = videopath->crop_height;
  videopath_rep->view.top_left.x = videopath->view.top_left.x;
  videopath_rep->view.top_left.y = videopath->view.top_left.y;
  videopath_rep->view.top_right.x = videopath->view.top_right.x;
  videopath_rep->view.top_right.y = videopath->view.top_right.y;
  videopath_rep->view.bottom_left.x = videopath->view.bottom_left.x;
  videopath_rep->view.bottom_left.y = videopath->view.bottom_left.y;
  videopath_rep->view.bottom_right.x = videopath->view.bottom_right.x;
  videopath_rep->view.bottom_right.y = videopath->view.bottom_right.y;
  videopath_rep->controls.width = videopath->subunit.width;
  videopath_rep->controls.height = videopath->subunit.height;
  videopath_rep->controls.rotation = videopath->subunit.rotation;
  videopath_rep->controls.opacity = videopath->subunit.opacity;

  return 1;
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
  kr_sprite_info_to_ebml(&sprite_rep, ebml);
}

void kr_text_to_ebml(kr_ebml *ebml, kr_text *text) {
  kr_text_info text_rep;
  kr_text_to_info(text, &text_rep);
  kr_text_info_to_ebml(&text_rep, ebml);
}

void kr_vector_to_ebml(kr_ebml *ebml, kr_vector *vector) {
  kr_vector_info vector_rep;
  kr_vector_to_info(vector, &vector_rep);
  kr_vector_info_to_ebml(&vector_rep, ebml);
}

void kr_compositor_path_to_ebml(kr_ebml *ebml, kr_compositor_path *path) {
  kr_compositor_path_info videopath_rep;
  kr_compositor_path_to_info(path, &videopath_rep);
  kr_compositor_path_info_to_ebml(&videopath_rep, ebml);
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

  address.path.unit = KR_MIXER;
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
      address.path.subunit.mixer_subunit = KR_PORTGROUP;
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
          if(compositor->path[s].subunit.active == 1) {
            krad_radio_address_to_ebml2(&ebml_out, &response, &compositor->path[s].subunit.address);
            kr_ebml2_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_INFO);
            kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
            kr_compositor_path_to_ebml(&ebml_out, &compositor->path[s]);
            kr_ebml2_finish_element(&ebml_out, payload);
            kr_ebml2_finish_element(&ebml_out, response);
          }
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
