#include "krad_radio_interface.h"
#include "krad_radio_internal.h"

static void krad_radio_remote_rep_to_ebml (kr_remote_t *remote, kr_ebml2_t *ebml);

static void krad_radio_remote_rep_to_ebml (kr_remote_t *remote, kr_ebml2_t *ebml) {

  unsigned char *remote_loc;

  kr_ebml2_start_element (ebml, EBML_ID_KRAD_RADIO_REMOTE_STATUS, &remote_loc);
	kr_ebml2_pack_string (ebml, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, remote->interface);
	kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_REMOTE_PORT, remote->port);
	kr_ebml2_finish_element (ebml, remote_loc);
}

int krad_radio_broadcast_subunit_destroyed (krad_ipc_broadcaster_t *broadcaster, kr_address_t *address) {

  unsigned char buffer[256];
  krad_broadcast_msg_t *broadcast_msg;
  kr_ebml2_t ebml;
  unsigned char *message_loc;

  kr_ebml2_set_buffer ( &ebml, buffer, 256);

  krad_radio_address_to_ebml2 (&ebml, &message_loc, address);
  kr_ebml2_pack_int32 (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_RADIO_UNIT_DESTROYED);
  kr_ebml2_finish_element (&ebml, message_loc);

  broadcast_msg = krad_broadcast_msg_create (broadcaster, buffer, ebml.pos);

  if (broadcast_msg != NULL) {
    return krad_ipc_server_broadcaster_broadcast ( broadcaster, &broadcast_msg );
  }

  return -1;
}

int krad_radio_broadcast_subunit_created ( krad_ipc_broadcaster_t *broadcaster, kr_address_t *address, void *subunit_in ) {

  unsigned char buffer[4096];
  krad_broadcast_msg_t *broadcast_msg;
  kr_ebml2_t ebml;
  unsigned char *message_loc;
  unsigned char *payload_loc;
  krad_subunit_ptr_t subunit;
  kr_rep_actual_t rep;
  
  kr_ebml2_set_buffer ( &ebml, buffer, 4096 );
  
  krad_radio_address_to_ebml2 (&ebml, &message_loc, address);
  kr_ebml2_pack_int32 (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_CREATED);
  kr_ebml2_start_element (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload_loc);
  
  if ((address->path.unit == KR_MIXER) && (address->path.subunit.mixer_subunit == KR_PORTGROUP)) {
    subunit.portgroup = subunit_in;
    krad_mixer_portgroup_to_rep (subunit.portgroup, &rep.portgroup);
    krad_mixer_portgroup_rep_to_ebml2 (&rep.portgroup, &ebml);
  }
  
  if (address->path.unit == KR_COMPOSITOR) { 
    if (address->path.subunit.compositor_subunit == KR_SPRITE) {
      krad_compositor_sprite_to_ebml2 ( &ebml, subunit_in );
    }
    if (address->path.subunit.compositor_subunit == KR_TEXT) {
      krad_compositor_text_to_ebml2 ( &ebml, subunit_in );
    }
    if (address->path.subunit.compositor_subunit == KR_VECTOR) {
      krad_compositor_vector_to_ebml2 ( &ebml, subunit_in );
    }
  }

  if ((address->path.unit == KR_STATION) && (address->path.subunit.station_subunit == KR_REMOTE)) {
    subunit.remote = subunit_in;
    krad_radio_remote_rep_to_ebml (subunit.remote, &ebml);
  }
  
  kr_ebml2_finish_element (&ebml, payload_loc);
  kr_ebml2_finish_element (&ebml, message_loc);

  broadcast_msg = krad_broadcast_msg_create (broadcaster, buffer, ebml.pos);

  if (broadcast_msg != NULL) {
    return krad_ipc_server_broadcaster_broadcast ( broadcaster, &broadcast_msg );
  }

  return -1;
}

int krad_radio_broadcast_subunit_control (krad_ipc_broadcaster_t *broadcaster, kr_address_t *address_in, int control, float value, void *client) {
  return krad_radio_broadcast_subunit_update (broadcaster, address_in, control, KR_FLOAT, &value, client);
}

int krad_radio_broadcast_subunit_update (krad_ipc_broadcaster_t *broadcaster, kr_address_t *address_in, int control, int type, void *value, void *client) {

  unsigned char buffer[1024];
  krad_broadcast_msg_t *broadcast_msg;
  kr_ebml2_t ebml;
  unsigned char *message_loc;
  kr_address_t address;
  unsigned char *payload_loc;
  float *value_float;
  int *value_int;

  kr_ebml2_set_buffer ( &ebml, buffer, 1024);

  address.path.unit = address_in->path.unit;
  if (address.path.unit == KR_MIXER) {
    address.path.subunit.mixer_subunit = address_in->path.subunit.mixer_subunit;
    strncpy (address.id.name, address_in->id.name, sizeof (address.id.name));
  } else {
    if (address.path.unit == KR_COMPOSITOR) {
      address.path.subunit.compositor_subunit = address_in->path.subunit.compositor_subunit;
    } else {
      if (address.path.unit == KR_TRANSPONDER) {
        address.path.subunit.transponder_subunit = address_in->path.subunit.transponder_subunit;
      }
    }
    address.id.number = address_in->id.number;
  }
  
  address.sub_id = address_in->sub_id;
  address.sub_id2 = address_in->sub_id2;
  address.control.portgroup_control = control;

  krad_radio_address_to_ebml2 (&ebml, &message_loc, &address);
  kr_ebml2_pack_int32 (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_CONTROL);
  kr_ebml2_start_element (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload_loc);
  switch (type) {
    case KR_FLOAT:
      value_float = (float *)value;
      kr_ebml2_pack_float (&ebml, EBML_ID_KRAD_SUBUNIT_CONTROL, *value_float);
      break;
    case KR_STRING:
      kr_ebml2_pack_string (&ebml, EBML_ID_KRAD_SUBUNIT_CONTROL, value);
      break;
    case KR_INT32:
      value_int = (int *)value;
      kr_ebml2_pack_int32 (&ebml, EBML_ID_KRAD_SUBUNIT_CONTROL, *value_int);
      break;
  }

  kr_ebml2_finish_element (&ebml, payload_loc);
  kr_ebml2_finish_element (&ebml, message_loc);

  broadcast_msg = krad_broadcast_msg_create (broadcaster, buffer, ebml.pos);
  broadcast_msg->skip_client = client;

  if (broadcast_msg->skip_client != NULL) {
    //printk ("want to Goint to skip a client!!\n");
  }

  if (broadcast_msg != NULL) {
    return krad_ipc_server_broadcaster_broadcast ( broadcaster, &broadcast_msg );
  }

  return -1;
}

void krad_radio_send_shipment_terminator (krad_ipc_server_t *kr_ipc) {

  kr_address_t address;
	uint64_t response;

  memset (&address, 0, sizeof (kr_address_t));

  address.path.unit = KR_STATION;
  address.path.subunit.station_subunit = KR_STATION_UNIT;

  krad_ipc_server_response_start_with_address_and_type ( kr_ipc,
                                                         &address,
                                                         EBML_ID_KRAD_SHIPMENT_TERMINATOR,
                                                         &response);
  krad_ipc_server_response_finish ( kr_ipc, response );
}

int krad_radio_address_to_ebml (krad_ebml_t *krad_ebml, uint64_t *element_loc, kr_address_t *address) {

  switch (address->path.unit) {
    case KR_MIXER:
      krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_MIXER_MSG, element_loc);
      krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.mixer_subunit);
      if (address->path.subunit.mixer_subunit != KR_UNIT) {
        krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NAME, address->id.name);
        if (address->path.subunit.mixer_subunit == KR_EFFECT) {
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->sub_id);
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->sub_id2);
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_CONTROL, address->control.effect_control);
        }
        if (address->path.subunit.mixer_subunit == KR_PORTGROUP) {
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_CONTROL, address->control.portgroup_control);
        }
      }
      return 1;
    case KR_COMPOSITOR:
      krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_COMPOSITOR_MSG, element_loc);
      krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.compositor_subunit);
      if (address->path.subunit.compositor_subunit != KR_UNIT) {
        krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->id.number);
      }
      return 1;
    case KR_TRANSPONDER:
      krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_TRANSPONDER_MSG, element_loc);
      krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.transponder_subunit);
      if (address->path.subunit.transponder_subunit != KR_UNIT) {
        //if (address->path.subunit.transponder_subunit == KR_ADAPTER) {
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->id.number);
        //}
      }
      return 1;
    case KR_STATION:
      krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_RADIO_MSG, element_loc);
      krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.station_subunit);
      if (address->path.subunit.station_subunit != KR_UNIT) {
        if (address->path.subunit.station_subunit == KR_REMOTE) {
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->id.number);
        }
      }
      return 1;
  }
  
  return 0;
}

int krad_radio_handler ( void *ptr ) {

	krad_radio_t *krad_radio = (krad_radio_t *)ptr;
	krad_ipc_server_t *kr_ipc;
	
	kr_ipc = krad_radio->remote.krad_ipc;
	
	int ret;
	uint32_t ebml_id;	
	uint32_t command;
	uint64_t ebml_data_size;
	uint64_t response;
  uint64_t payload_loc;
  kr_address_t address;
  kr_remote_t remote;
	int i;
	uint64_t numbers[10];
	krad_tags_t *krad_tags;
	
	char string1[512];	
	char string2[512];
	char string3[512];	
	
	krad_tags = NULL;
	i = 0;
	command = 0;
	ebml_id = 0;
	ebml_data_size = 0;
	response = 0;
	ret = 0;

	string1[0] = '\0';
	string2[0] = '\0';
	string3[0] = '\0';	
	
	char *tag_item = string1;	
	char *tag_name = string2;
	char *tag_value = string3;

	krad_ipc_server_read_command ( kr_ipc, &command, &ebml_data_size);

	switch ( command ) {
		
		case EBML_ID_KRAD_MIXER_CMD:
			ret = krad_mixer_handler ( krad_radio->krad_mixer, kr_ipc );
      break;			
		case EBML_ID_KRAD_COMPOSITOR_CMD:
			ret = krad_compositor_handler ( krad_radio->krad_compositor, kr_ipc );		
      break;				
		case EBML_ID_KRAD_TRANSPONDER_CMD:
			ret = krad_transponder_handler ( krad_radio->krad_transponder, kr_ipc );
      break;
		case EBML_ID_KRAD_RADIO_CMD:
			return krad_radio_handler ( ptr );
		case EBML_ID_KRAD_RADIO_CMD_LIST_TAGS:
			//printk ("LIST_TAGS");
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			if (ebml_id != EBML_ID_KRAD_RADIO_TAG_ITEM) {
				failfast ("missing item");
			}
			krad_ebml_read_string (kr_ipc->current_client->krad_ebml, tag_item, ebml_data_size);
			//printk ("Get Tags for %s", tag_item);
			if (strncmp("station", tag_item, 7) == 0) {
				krad_tags = krad_radio->krad_tags;
			} else {
			//	krad_tags = krad_radio_find_tags_for_item ( krad_radio, tag_item );
			}
			if (krad_tags != NULL) {
        i = 0;
        address.path.unit = KR_STATION;
			  address.path.subunit.station_subunit = KR_TAGS;
        krad_ipc_server_response_start_with_address_and_type ( kr_ipc,
                                                               &address,
                                                               EBML_ID_KRAD_SUBUNIT_INFO,
                                                               &response);
        krad_ipc_server_payload_start ( kr_ipc, &payload_loc);

				while (krad_tags_get_next_tag ( krad_tags, &i, &tag_name, &tag_value)) {
					krad_ipc_server_response_add_tag ( kr_ipc, tag_item, tag_name, tag_value);
				}
        krad_ipc_server_payload_finish ( kr_ipc, payload_loc );
        krad_ipc_server_response_finish ( kr_ipc, response );
			} else {
				printke ("Could not find %s", tag_item);
			}
			ret = 1;
			break;
		case EBML_ID_KRAD_RADIO_CMD_SET_TAG:
			krad_ipc_server_read_tag ( kr_ipc, &tag_item, &tag_name, &tag_value );
			if (strncmp("station", tag_item, 7) == 0) {
				krad_tags_set_tag ( krad_radio->krad_tags, tag_name, tag_value);
			} else {
				krad_tags = krad_radio_find_tags_for_item ( krad_radio, tag_item );
				if (krad_tags != NULL) {
					krad_tags_set_tag ( krad_tags, tag_name, tag_value);
					printk ("Set Tag %s on %s to %s", tag_name, tag_item, tag_value);
				} else {
					printke ("Could not find %s", tag_item);
				}
			}
			//krad_ipc_server_broadcast_tag ( kr_ipc, tag_item, tag_name, tag_value);
			break;
		case EBML_ID_KRAD_RADIO_CMD_GET_TAG:
			krad_ipc_server_read_tag ( kr_ipc, &tag_item, &tag_name, &tag_value );
			//printk ("Get Tag %s - %s - %s", tag_item, tag_name, tag_value);
			if (strncmp("station", tag_item, 7) == 0) {
				tag_value = krad_tags_get_tag (krad_radio->krad_tags, tag_name);
			} else {
				//krad_tags = krad_radio_find_tags_for_item ( krad_radio, tag_item );
				//if (krad_tags != NULL) {
				//	tag_value = krad_tags_get_tag ( krad_tags, tag_name );
				//	//printk ("Got Tag %s on %s - %s", tag_name, tag_item, tag_value);
				//} else {
				//	printke ("Could not find %s", tag_item);
				//}
			}
			
			if (strlen(tag_value)) {
			  address.path.unit = KR_STATION;
			  address.path.subunit.station_subunit = KR_TAGS;
        krad_ipc_server_response_start_with_address_and_type ( kr_ipc,
                                                               &address,
                                                               EBML_ID_KRAD_SUBUNIT_INFO,
                                                               &response);
        krad_ipc_server_payload_start ( kr_ipc, &payload_loc);
				krad_ipc_server_response_add_tag ( kr_ipc, tag_item, tag_name, tag_value);
        krad_ipc_server_payload_finish ( kr_ipc, payload_loc );
        krad_ipc_server_response_finish ( kr_ipc, response );
			}
			ret = 1;
			break;
		case EBML_ID_KRAD_RADIO_CMD_GET_REMOTE_STATUS:
			address.path.unit = KR_STATION;
			address.path.subunit.station_subunit = KR_REMOTE;
      for (i = 0; i < MAX_REMOTES; i++) {
        if (kr_ipc->tcp_port[i]) {
          address.id.number = i;
          krad_ipc_server_response_start_with_address_and_type ( kr_ipc,
                                                                 &address,
                                                                 EBML_ID_KRAD_SUBUNIT_INFO,
                                                                 &response);
          krad_ipc_server_payload_start ( kr_ipc, &payload_loc);
			    krad_ipc_server_respond_string ( kr_ipc, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, kr_ipc->tcp_interface[i]);
			    krad_ipc_server_respond_number ( kr_ipc, EBML_ID_KRAD_RADIO_REMOTE_PORT, kr_ipc->tcp_port[i]);
          krad_ipc_server_payload_finish ( kr_ipc, payload_loc );
          krad_ipc_server_response_finish ( kr_ipc, response );
        }
      }
			ret = 1;
			break;
		case EBML_ID_KRAD_RADIO_CMD_REMOTE_ENABLE:
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			krad_ebml_read_string (kr_ipc->current_client->krad_ebml, remote.interface, ebml_data_size);
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
		  remote.port = krad_ebml_read_number ( kr_ipc->current_client->krad_ebml, ebml_data_size);
			if (krad_ipc_server_enable_remote (kr_ipc, remote.interface, remote.port)) {
			  //FIXME this is wrong in the case of adapter based matches with multiple ips
			  // perhaps switch to callback based create broadcast?
			  address.path.unit = KR_STATION;
			  address.path.subunit.station_subunit = KR_REMOTE;
        for (i = 0; i < MAX_REMOTES; i++) {
          if (kr_ipc->tcp_port[i] == remote.port) {
            address.id.number = i;
            break;
          }
        }
        krad_radio_broadcast_subunit_created ( kr_ipc->ipc_broadcaster, &address, (void *)&remote);
			}
			return 0;
		case EBML_ID_KRAD_RADIO_CMD_REMOTE_DISABLE:
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			krad_ebml_read_string (kr_ipc->current_client->krad_ebml, string1, ebml_data_size);
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			numbers[0] = krad_ebml_read_number ( kr_ipc->current_client->krad_ebml, ebml_data_size);
			while (1) {
        i = krad_ipc_server_disable_remote (kr_ipc, string1, numbers[0]);
        if (i < 0) {
          break;
        }
        address.path.unit = KR_STATION;
        address.path.subunit.mixer_subunit = KR_REMOTE;
        address.id.number = i;
        krad_radio_broadcast_subunit_destroyed (kr_ipc->ipc_broadcaster, &address);
			}
			return 0;
		case EBML_ID_KRAD_RADIO_CMD_OSC_ENABLE:
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			numbers[0] = krad_ebml_read_number ( kr_ipc->current_client->krad_ebml, ebml_data_size);
			krad_osc_listen (krad_radio->remote.krad_osc, numbers[0]);
			return 0;
		case EBML_ID_KRAD_RADIO_CMD_OSC_DISABLE:
			krad_osc_stop_listening (krad_radio->remote.krad_osc);
			return 0;
		case EBML_ID_KRAD_RADIO_CMD_WEB_ENABLE:
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			numbers[0] = krad_ebml_read_number ( kr_ipc->current_client->krad_ebml, ebml_data_size);
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);
			numbers[1] = krad_ebml_read_number ( kr_ipc->current_client->krad_ebml, ebml_data_size);
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			krad_ebml_read_string (kr_ipc->current_client->krad_ebml, string1, ebml_data_size);
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			krad_ebml_read_string (kr_ipc->current_client->krad_ebml, string2, ebml_data_size);
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			krad_ebml_read_string (kr_ipc->current_client->krad_ebml, string3, ebml_data_size);
			// Remove existing if existing (ie. I am changing the port)
			if (krad_radio->remote.krad_http != NULL) {
				krad_http_server_destroy (krad_radio->remote.krad_http);
			}
			if (krad_radio->remote.krad_websocket != NULL) {
				krad_websocket_server_destroy (krad_radio->remote.krad_websocket);
			}
			krad_radio->remote.krad_http = krad_http_server_create (numbers[0], numbers[1],
                                                              string1, string2, string3 );
      krad_radio->remote.krad_websocket = krad_websocket_server_create ( krad_radio->sysname, numbers[1] );
			return 0;
		case EBML_ID_KRAD_RADIO_CMD_WEB_DISABLE:
			krad_http_server_destroy (krad_radio->remote.krad_http);
      krad_websocket_server_destroy (krad_radio->remote.krad_websocket);
			krad_radio->remote.krad_http = NULL;
			krad_radio->remote.krad_websocket = NULL;
			return 0;
		case EBML_ID_KRAD_RADIO_CMD_GET_SYSTEM_INFO:
			address.path.unit = KR_STATION;
			address.path.subunit.station_subunit = KR_STATION_UNIT;
      krad_ipc_server_response_start_with_address_and_type ( kr_ipc,
                                                             &address,
                                                             EBML_ID_KRAD_UNIT_INFO,
                                                             &response);

      krad_ipc_server_payload_start ( kr_ipc, &payload_loc);
			krad_ipc_server_respond_string ( kr_ipc, EBML_ID_KRAD_RADIO_SYSTEM_INFO, krad_system_info());
			krad_ipc_server_respond_string ( kr_ipc, EBML_ID_KRAD_RADIO_LOGNAME, krad_radio->log.filename);
			krad_ipc_server_respond_number ( kr_ipc, EBML_ID_KRAD_RADIO_UPTIME, krad_system_daemon_uptime());
			krad_ipc_server_respond_number ( kr_ipc, EBML_ID_KRAD_RADIO_SYSTEM_CPU_USAGE, krad_system_get_cpu_usage());
      krad_ipc_server_payload_finish ( kr_ipc, payload_loc );
      krad_ipc_server_response_finish ( kr_ipc, response );
			ret = 1;
			break;
		case EBML_ID_KRAD_RADIO_CMD_SET_DIR:
			krad_ebml_read_element ( kr_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			krad_ebml_read_string (kr_ipc->current_client->krad_ebml, string1, ebml_data_size);
			if (strlen(string1)) {
				krad_radio_set_dir ( krad_radio, string1 );
			}
			return 0;
		case EBML_ID_KRAD_RADIO_CMD_BROADCAST_SUBSCRIBE:
			numbers[0] = krad_ebml_read_number ( kr_ipc->current_client->krad_ebml, ebml_data_size);
			krad_ipc_server_add_client_to_broadcast ( kr_ipc, numbers[0] );
			return 0;
		default:
			printke ("Krad Radio Command Unknown! %u", command);
			return 0;
	}

  if (ret && (!krad_ipc_server_current_client_is_subscriber (kr_ipc))) {
    krad_radio_send_shipment_terminator (kr_ipc);
  }

	return ret;
}
