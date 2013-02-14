#include "krad_compositor_interface.h"

void krad_compositor_sprite_to_ebml ( krad_ipc_server_t *krad_ipc, krad_sprite_t *sprite ) {
  krad_sprite_rep_t sprite_rep;
  krad_sprite_to_rep (sprite, &sprite_rep);
  krad_compositor_sprite_rep_to_ebml (&sprite_rep, krad_ipc->current_client->krad_ebml2);
}

int krad_compositor_handler ( krad_compositor_t *krad_compositor, krad_ipc_server_t *krad_ipc ) {
  
  uint32_t ebml_id;
  
  uint32_t command;
  uint64_t ebml_data_size;
  kr_address_t address;
  uint64_t response;
  uint64_t info_loc;
  uint64_t payload_loc;
  
  uint64_t numbers[4];    
  char string[256];
  
  int type;
  
  int p;
  int s;
  int sd1;
  int sd2;
      
  type = 0;
  sd1 = 0;
  sd2 = 0;  
  p = 0;
  s = 0;
  string[0] = '\0';
  
  krad_ipc_server_read_command ( krad_ipc, &command, &ebml_data_size);

  switch ( command ) {
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_SUBUNIT:
      //krad_sprite_rep = krad_compositor_ebml_to_new_krad_sprite_rep (krad_ipc->current_client->krad_ebml, numbers);
      //krad_compositor_set_sprite (krad_compositor, krad_sprite_rep);
      //kr_compositor_sprite_rep_destroy (krad_sprite_rep);
      break;  
    case EBML_ID_KRAD_COMPOSITOR_CMD_REMOVE_SUBUNIT:
      address.path.unit = KR_COMPOSITOR;
      krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);
      numbers[0] = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
      address.path.subunit.compositor_subunit = numbers[0];
      krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);
      numbers[0] = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
      address.id.number = numbers[0];
      krad_compositor_subunit_destroy (krad_compositor, &address);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_ADD_SUBUNIT:
      krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);
      numbers[0] = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
      type = numbers[0];
      krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);
      krad_ebml_read_string (krad_ipc->current_client->krad_ebml, string, ebml_data_size);
      krad_compositor_subunit_create (krad_compositor, type, string);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_LIST_SUBUNITS:
      for (s = 0; s < KC_MAX_SPRITES; s++) {
        if (krad_compositor->sprite[s].subunit.active == 1) {
          krad_ipc_server_response_start_with_address_and_type ( krad_ipc,
                                                                 &krad_compositor->sprite[s].subunit.address,
                                                                 EBML_ID_KRAD_SUBUNIT_INFO,
                                                                 &response);
          krad_ipc_server_payload_start ( krad_ipc, &payload_loc);
          krad_compositor_sprite_to_ebml ( krad_ipc, &krad_compositor->sprite[s]);
          krad_ipc_server_payload_finish ( krad_ipc, payload_loc );
          krad_ipc_server_response_finish ( krad_ipc, response );
        }
      }
      return 1;
    case EBML_ID_KRAD_COMPOSITOR_CMD_INFO:
      krad_ipc_server_response_start_with_address_and_type ( krad_ipc,
                                                             &krad_compositor->address,
                                                             EBML_ID_KRAD_UNIT_INFO,
                                                             &response);

      krad_ipc_server_payload_start ( krad_ipc, &payload_loc);
      krad_ipc_server_response_start ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_INFO, &info_loc);
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_WIDTH, krad_compositor->width);
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_HEIGHT, krad_compositor->height);  
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_FPS_NUMERATOR,
                                       krad_compositor->fps_numerator);
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_FPS_DENOMINATOR,
                                       krad_compositor->fps_denominator);
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_SPRITE_COUNT,
                                       krad_compositor->active_sprites);
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_TEXT_COUNT,
                                       krad_compositor->active_texts);
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_VECTOR_COUNT,
                                       krad_compositor->active_vectors);
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_PORT_COUNT,
                                       krad_compositor->active_input_ports);
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_PORT_COUNT,
                                       krad_compositor->active_output_ports);
      krad_ipc_server_respond_number ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_PORT_COUNT,
                                       krad_compositor->frames);
      if (krad_compositor_get_background_name (krad_compositor, string)) {
        krad_ipc_server_respond_string ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_LAST_SNAPSHOT_NAME, string);
      } else {
        krad_ipc_server_respond_string ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_LAST_SNAPSHOT_NAME, "");
      }
      krad_compositor_get_last_snapshot_name (krad_compositor, string);
      krad_ipc_server_respond_string ( krad_ipc, EBML_ID_KRAD_COMPOSITOR_LAST_SNAPSHOT_NAME, string);
      krad_ipc_server_response_finish ( krad_ipc, info_loc);
      krad_ipc_server_payload_finish ( krad_ipc, payload_loc );
      krad_ipc_server_response_finish ( krad_ipc, response );
      return 1;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SNAPSHOT:
      krad_compositor->snapshot++;
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SNAPSHOT_JPEG:
      krad_compositor->snapshot_jpeg++;
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_FRAME_RATE:
      krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);
      numbers[0] = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
      krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);
      numbers[1] = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
      krad_compositor_set_frame_rate (krad_compositor, numbers[0], numbers[1]);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_RESOLUTION:
      krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);
      numbers[0] = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
      krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);
      numbers[1] = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
      krad_compositor_update_resolution (krad_compositor, numbers[0], numbers[1]);
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_SET_BACKGROUND:
      krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);  
      krad_ebml_read_string (krad_ipc->current_client->krad_ebml, string, ebml_data_size);
      krad_compositor_set_background (krad_compositor, string);
      break;      
    case EBML_ID_KRAD_COMPOSITOR_CMD_CLOSE_DISPLAY:
      krad_compositor_close_display (krad_compositor);    
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_OPEN_DISPLAY:
      krad_compositor_open_display (krad_compositor);
      break;    
    case EBML_ID_KRAD_COMPOSITOR_CMD_LOCAL_VIDEOPORT_DESTROY:
      for (p = 0; p < KC_MAX_PORTS; p++) {
        if (krad_compositor->port[p].local == 1) {
          krad_compositor_port_destroy (krad_compositor, &krad_compositor->port[p]);
          break;
        }
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_CMD_LOCAL_VIDEOPORT_CREATE:
      sd1 = krad_ipc_server_recvfd (krad_ipc->current_client);
      sd2 = krad_ipc_server_recvfd (krad_ipc->current_client);
      printk ("VIDEOPORT_CREATE Got FD's %d and %d\n", sd1, sd2);
      krad_compositor_local_port_create (krad_compositor, "localport", INPUT, sd1, sd2);
      break;
  }
  return 0;
}

