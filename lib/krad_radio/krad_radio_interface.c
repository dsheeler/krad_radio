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

void krad_radio_pack_shipment_terminator (kr_ebml2_t *ebml) {

  kr_address_t address;
	unsigned char *response;

  memset (&address, 0, sizeof (kr_address_t));

  address.path.unit = KR_STATION;
  address.path.subunit.station_subunit = KR_STATION_UNIT;

  krad_radio_address_to_ebml2 (ebml, &response, &address);
  kr_ebml2_pack_uint32 ( ebml,
                         EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                         EBML_ID_KRAD_SHIPMENT_TERMINATOR);
  kr_ebml2_finish_element (ebml, response);
}

void *krad_radio_client_create (void *ptr) {

	krad_radio_t *krad_radio;
  krad_radio_client_t *krad_radio_client;
  
  krad_radio = (krad_radio_t *)ptr;  
  krad_radio_client = calloc (1, sizeof (krad_radio_client));
  krad_radio_client->krad_radio = krad_radio;

  return krad_radio_client;
}

void krad_radio_client_destroy (void *ptr) {
	krad_radio_client_t *krad_radio_client = (krad_radio_client_t *)ptr;
  free (krad_radio_client);
}

int validate_header ( kr_io2_t *in ) {

  kr_ebml2_t ebml;
  int ret;
  char doctype[32];
  uint32_t version;
  uint32_t read_version;

  kr_ebml2_set_buffer ( &ebml, in->rd_buf, in->len );

  ret = kr_ebml2_unpack_header (&ebml, doctype, 32, &version, &read_version);

  if (ret > 0) {
    if ((version == KRAD_IPC_DOCTYPE_VERSION) && (read_version == KRAD_IPC_DOCTYPE_READ_VERSION) &&
        (strlen(KRAD_IPC_CLIENT_DOCTYPE) == strlen(doctype)) &&
        (strncmp(doctype, KRAD_IPC_CLIENT_DOCTYPE, strlen(KRAD_IPC_CLIENT_DOCTYPE)) == 0)) {
        kr_io2_pulled (in, ret);
        return ret;
    } else {
      printke ("validate header mismatch %u %u %s \n", version, read_version, doctype);
    }
  } else {
    printke ("validate header err %d\n", ret);
  }

  return -1;
}

void pack_client_header (kr_io2_t *out) {

  kr_ebml2_t ebml;

  kr_ebml2_set_buffer ( &ebml, out->buf, out->space );

  kr_ebml2_pack_header (&ebml,
                        KRAD_IPC_SERVER_DOCTYPE,
                        KRAD_IPC_DOCTYPE_VERSION,
                        KRAD_IPC_DOCTYPE_READ_VERSION);

  kr_io2_advance (out, ebml.pos);
}

int validate_client ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client ) {

  int ret;
  
  ret = validate_header (in);
  if (ret > 0) {
    client->valid = 1;
    pack_client_header (out);
    return 1;
  } else {
    printke ("Invalid client!");
    return -1;
  }
}

uint32_t full_command (kr_io2_t *in) {

  kr_ebml2_t ebml;
  uint32_t element;
  uint64_t size;
  int ret;

  if (!(kr_io2_has_in (in))) {
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml, in->rd_buf, in->len );
  
  ret = kr_ebml2_unpack_id (&ebml, &element, &size);

  if (ret < 0) {
    printke ("full_command EBML ID Not found");
    return 0;
  }

  size += ebml.pos;

  if (in->len < size) {
    printke ("full_command Not Enough bytes..");
    return 0;
  }
  
  return element;
}

int krad_radio_client_command ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client ) {

	krad_radio_t *krad_radio;
	krad_radio = client->krad_radio;
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
  char string1[512];
  char string2[512];
  char string3[512];      
	uint32_t numbers[10];

  if (!(kr_io2_has_in (in))) {
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_in, in->rd_buf, in->len );
  
  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if ((ret < 0) || (command != EBML_ID_KRAD_RADIO_CMD)) {
    printke ("krad_radio_client_command invalid EBML ID Not found");
    return 0;
  }
  
  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if (ret < 0) {
    printke ("krad_radio_client_command EBML ID Not found");
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_out, out->buf, out->space );

	switch ( command ) {
		case EBML_ID_KRAD_RADIO_CMD_GET_SYSTEM_INFO:
      address.path.unit = KR_STATION;
      address.path.subunit.station_subunit = KR_STATION_UNIT;

      krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
      kr_ebml2_pack_uint32 ( &ebml_out,
                             EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                             EBML_ID_KRAD_UNIT_INFO);

      kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      kr_ebml2_pack_string ( &ebml_out, EBML_ID_KRAD_RADIO_SYSTEM_INFO, krad_system_info());
      kr_ebml2_pack_string ( &ebml_out, EBML_ID_KRAD_RADIO_LOGNAME, krad_radio->log.filename);
      kr_ebml2_pack_uint64 ( &ebml_out, EBML_ID_KRAD_RADIO_UPTIME, krad_system_daemon_uptime());
      kr_ebml2_pack_uint32 ( &ebml_out, EBML_ID_KRAD_RADIO_SYSTEM_CPU_USAGE, krad_system_get_cpu_usage());
      kr_ebml2_finish_element (&ebml_out, payload);
      kr_ebml2_finish_element (&ebml_out, response);
      break;
      /*
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
			break;
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
			break;
	  */
		case EBML_ID_KRAD_RADIO_CMD_OSC_ENABLE:
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
			krad_osc_listen (krad_radio->remote.krad_osc, numbers[0]);
			break;
		case EBML_ID_KRAD_RADIO_CMD_OSC_DISABLE:
			krad_osc_stop_listening (krad_radio->remote.krad_osc);
			break;
		case EBML_ID_KRAD_RADIO_CMD_WEB_ENABLE:
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[1]);
      kr_ebml2_unpack_element_string (&ebml_in, &element, string1, sizeof(string1));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string2, sizeof(string2));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string3, sizeof(string3));
			if (krad_radio->remote.krad_http != NULL) {
				krad_http_server_destroy (krad_radio->remote.krad_http);
			}
			if (krad_radio->remote.krad_websocket != NULL) {
				krad_websocket_server_destroy (krad_radio->remote.krad_websocket);
			}
			krad_radio->remote.krad_http = krad_http_server_create (numbers[0], numbers[1],
                                                              string1, string2, string3 );
      krad_radio->remote.krad_websocket = krad_websocket_server_create ( krad_radio->sysname, numbers[1] );
			break;
		case EBML_ID_KRAD_RADIO_CMD_WEB_DISABLE:
			krad_http_server_destroy (krad_radio->remote.krad_http);
      krad_websocket_server_destroy (krad_radio->remote.krad_websocket);
			krad_radio->remote.krad_http = NULL;
			krad_radio->remote.krad_websocket = NULL;
			break;
		case EBML_ID_KRAD_RADIO_CMD_SET_DIR:
      kr_ebml2_unpack_element_string (&ebml_in, &element, string, sizeof(string1));
			if (strlen(string)) {
				krad_radio_set_dir ( krad_radio, string );
			}
			break;
		/*
		case EBML_ID_KRAD_RADIO_CMD_BROADCAST_SUBSCRIBE:
			numbers[0] = krad_ebml_read_number ( kr_ipc->current_client->krad_ebml, ebml_data_size);
			krad_ipc_server_add_client_to_broadcast ( kr_ipc, numbers[0] );
			break;
		default:
			printke ("Krad Radio Command Unknown! %u", command);
			return -1;
			*/
	}

  //if (ret && (!krad_ipc_server_current_client_is_subscriber (kr_ipc))) {
    krad_radio_pack_shipment_terminator (&ebml_out);
  //}
  
  kr_io2_advance (out, ebml_out.pos);

  return 0;
}

int krad_radio_client_handler ( kr_io2_t *in, kr_io2_t *out, void *ptr ) {

	krad_radio_client_t *client;
	//krad_radio_t *krad_radio;
  int ret;
  uint32_t command;

  client = (krad_radio_client_t *)ptr;
	//krad_radio = client->krad_radio;

  if (!client->valid) {
    ret = validate_client (in, out, client);
    if (ret != 1) {
      return -1;
    }
  }

  //FIXME dont forget to loop here incase we get mroe than 1 command

  command = full_command (in);
  if (command == 0) {
    return 0;
  }  
    

  printk ("we have a full command la de da its %zu bytes", in->len);


  if (command == EBML_ID_KRAD_RADIO_CMD) {
    ret = krad_radio_client_command (in, out, client);
    if (ret != 0) {
      return -1;
    }
  }


/*
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
			return krad_radio_client_handler ( ptr );
			
			
			


	return ret;
	
*/

  return 0;	
}
