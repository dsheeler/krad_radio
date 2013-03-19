#include "krad_transponder_interface.h"

void krad_transponder_to_rep ( krad_transponder_t *krad_transponder, kr_transponder_t *transponder_rep ) {
  transponder_rep->receiver_port = krad_transponder->krad_receiver->port;
  transponder_rep->transmitter_port = krad_transponder->krad_transmitter->port;
}

void krad_transponder_rep_to_ebml ( kr_ebml2_t *ebml, kr_transponder_t *transponder_rep ) {
  kr_ebml2_pack_uint16 (ebml, EBML_ID_KRAD_RADIO_TCP_PORT, transponder_rep->receiver_port);
  kr_ebml2_pack_uint16 (ebml, EBML_ID_KRAD_RADIO_TCP_PORT, transponder_rep->transmitter_port);
}

void krad_transponder_to_ebml ( kr_ebml2_t *ebml, krad_transponder_t *krad_transponder ) {
  kr_transponder_t transponder_rep;
  memset (&transponder_rep, 0, sizeof (kr_transponder_t));
  krad_transponder_to_rep (krad_transponder, &transponder_rep);
  krad_transponder_rep_to_ebml (ebml, &transponder_rep);
}

int krad_transponder_command ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client ) {

	int k;
	int devices;
  krad_radio_t *krad_radio;
  krad_transponder_t *krad_transponder;
  kr_address_t address;
  krad_ipc_server_t *kr_ipc;
  unsigned char *response;
  unsigned char *payload;
  kr_ebml2_t ebml_in;
  kr_ebml2_t ebml_out;
  uint32_t command;
  uint32_t element;
  uint64_t size;
  int ret;
  char string[512];
  uint16_t port;
  
	string[0] = '\0';
	k = 0;
  port = 0;
  krad_radio = client->krad_radio;
  krad_transponder = krad_radio->krad_transponder;
  kr_ipc = krad_radio->remote.krad_ipc;

  if (!(kr_io2_has_in (in))) {
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_in, in->rd_buf, in->len );

  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if ((ret < 0) || (command != EBML_ID_KRAD_TRANSPONDER_CMD)) {
    printke ("krad_mixer_command invalid EBML ID Not found");
    return 0;
  }

  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if (ret < 0) {
    printke ("krad_mixer_command EBML ID Not found");
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_out, out->buf, out->space );

	switch ( command ) {
	
		case EBML_ID_KRAD_TRANSPONDER_CMD_GET_INFO:
      krad_radio_address_to_ebml2 (&ebml_out, &response, &krad_transponder->address);
      kr_ebml2_pack_uint32 ( &ebml_out,
                             EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                             EBML_ID_KRAD_UNIT_INFO);
      kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      krad_transponder_to_ebml (&ebml_out, krad_transponder);
      kr_ebml2_finish_element (&ebml_out, payload);
      kr_ebml2_finish_element (&ebml_out, response);
		  break;
/*
		case EBML_ID_KRAD_TRANSPONDER_CMD_LIST_LINKS:
			//printk ("krad transponder handler! LIST_LINKS");
			
			krad_Xtransponder_list ( krad_transponder->krad_Xtransponder );
			
			krad_ipc_server_response_start ( krad_ipc, EBML_ID_KRAD_TRANSPONDER_MSG, &response);

			
			for (k = 0; k < KRAD_TRANSPONDER_MAX_LINKS; k++) {
				if (krad_transponder->krad_link[k] != NULL) {
					printk ("Link %d Active: %s", k, krad_transponder->krad_link[k]->mount);
					krad_transponder_link_to_ebml ( krad_ipc->current_client, krad_transponder->krad_link[k]);
				}
			}
			

			krad_ipc_server_response_finish ( krad_ipc, response );	
						
			break;
			
		case EBML_ID_KRAD_TRANSPONDER_CMD_CREATE_LINK:
			for (k = 0; k < KRAD_TRANSPONDER_MAX_LINKS; k++) {
				if (krad_transponder->krad_link[k] == NULL) {

					krad_transponder->krad_link[k] = krad_link_prepare (k);
					krad_link = krad_transponder->krad_link[k];
					krad_link->link_num = k;
					krad_link->krad_radio = krad_transponder->krad_radio;
					krad_link->krad_transponder = krad_transponder;
					krad_transponder_ebml_to_link ( krad_ipc, krad_link );
					krad_link_start (krad_link);
				
				  *//*
					if ((krad_link->operation_mode == TRANSMIT) || (krad_link->operation_mode == RECORD)) {
						if (krad_link_wait_codec_init (krad_link) == 0) {
							krad_transponder_broadcast_link_created ( krad_ipc, krad_link );
						}
					} else {
						krad_transponder_broadcast_link_created ( krad_ipc, krad_link );
					}
          *//*
					break;
				}
			}
			break;
		case EBML_ID_KRAD_TRANSPONDER_CMD_DESTROY_LINK:
			tinyint = krad_ipc_server_read_number (krad_ipc, ebml_data_size);
			k = tinyint;
			//printk ("krad transponder handler! DESTROY_LINK: %d %u", k, tinyint);
			
			if (krad_transponder->krad_link[k] != NULL) {
				krad_link_destroy (krad_transponder->krad_link[k]);
				krad_transponder->krad_link[k] = NULL;
			}
			*//*
			krad_ipc_server_simple_number_broadcast ( krad_ipc,
													  EBML_ID_KRAD_TRANSPONDER_MSG,
													  EBML_ID_KRAD_TRANSPONDER_LINK_DESTROYED,
													  EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER,
											 		  k);			
			*//*
			break;
		case EBML_ID_KRAD_TRANSPONDER_CMD_UPDATE_LINK:
			//printk ("krad transponder handler! UPDATE_LINK");

			krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	
			
			if (ebml_id == EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER) {
			
				tinyint = krad_ipc_server_read_number (krad_ipc, ebml_data_size);
				k = tinyint;
				//printk ("krad transponder handler! UPDATE_LINK: %d %u", k, tinyint);
			
				if (krad_transponder->krad_link[k] != NULL) {

					krad_ebml_read_element (krad_ipc->current_client->krad_ebml, &ebml_id, &ebml_data_size);	

					if (krad_transponder->krad_link[k]->audio_codec == OPUS) {

						*//*
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_OPUS_APPLICATION) {
							krad_ebml_read_string (krad_ipc->current_client->krad_ebml, string, ebml_data_size);
							if (strncmp(string, "OPUS_APPLICATION_VOIP", 21) == 0) {
								krad_opus_set_application (krad_transponder->krad_link[k]->krad_opus, OPUS_APPLICATION_VOIP);
							}
							if (strncmp(string, "OPUS_APPLICATION_AUDIO", 22) == 0) {
								krad_opus_set_application (krad_transponder->krad_link[k]->krad_opus, OPUS_APPLICATION_AUDIO);
							}
							if (strncmp(string, "OPUS_APPLICATION_RESTRICTED_LOWDELAY", 36) == 0) {
								krad_opus_set_application (krad_transponder->krad_link[k]->krad_opus, OPUS_APPLICATION_RESTRICTED_LOWDELAY);
							}
						}
						*//*
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_OPUS_SIGNAL) {
							krad_ebml_read_string (krad_ipc->current_client->krad_ebml, string, ebml_data_size);

							krad_opus_set_signal (krad_transponder->krad_link[k]->krad_opus, 
													krad_opus_string_to_signal(string));
						}
						
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_OPUS_BANDWIDTH) {
							
							krad_ebml_read_string (krad_ipc->current_client->krad_ebml, string, ebml_data_size);
							
							krad_opus_set_bandwidth (krad_transponder->krad_link[k]->krad_opus, 
													krad_opus_string_to_bandwidth(string));
						}						

						if (ebml_id == EBML_ID_KRAD_LINK_LINK_OPUS_BITRATE) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
					
							if ((bigint >= 500) && (bigint <= 512000)) {
								krad_opus_set_bitrate (krad_transponder->krad_link[k]->krad_opus, bigint);
							}
						}
						
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_OPUS_COMPLEXITY) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
					
							if ((bigint >= 0) && (bigint <= 10)) {
								krad_opus_set_complexity (krad_transponder->krad_link[k]->krad_opus, bigint);
							}
						}						
				
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_OPUS_FRAME_SIZE) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
							if ((bigint == 120) || (bigint == 240) || (bigint == 480) || (bigint == 960) || (bigint == 1920) || (bigint == 2880)) {
								krad_opus_set_frame_size (krad_transponder->krad_link[k]->krad_opus, bigint);
							}
						}
						
						//FIXME verify ogg container
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_OGG_MAX_PACKETS_PER_PAGE) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
							if ((bigint > 0) && (bigint < 200)) {					
								krad_ogg_set_max_packets_per_page (krad_transponder->krad_link[k]->krad_container->krad_ogg, bigint);
							}
						}
					}
					
					if (krad_transponder->krad_link[k]->video_codec == THEORA) {
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_THEORA_QUALITY) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
							krad_theora_encoder_quality_set (krad_transponder->krad_link[k]->krad_theora_encoder, bigint);
						}
					}				
					
					if (krad_transponder->krad_link[k]->video_codec == VP8) {
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_VP8_BITRATE) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
					
							if (bigint > 0) {
								krad_vpx_encoder_bitrate_set (krad_transponder->krad_link[k]->krad_vpx_encoder, bigint);
							}
						}
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_VP8_MIN_QUANTIZER) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
					
							if (bigint > 0) {
								krad_vpx_encoder_min_quantizer_set (krad_transponder->krad_link[k]->krad_vpx_encoder, bigint);
							}
						}
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_VP8_MAX_QUANTIZER) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
					
							if (bigint > 0) {
								krad_vpx_encoder_max_quantizer_set (krad_transponder->krad_link[k]->krad_vpx_encoder, bigint);
							}
						}
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_VP8_DEADLINE) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
					
							if (bigint > 0) {
								krad_vpx_encoder_deadline_set (krad_transponder->krad_link[k]->krad_vpx_encoder, bigint);
							}
						}												
						if (ebml_id == EBML_ID_KRAD_LINK_LINK_VP8_FORCE_KEYFRAME) {
							bigint = krad_ebml_read_number (krad_ipc->current_client->krad_ebml, ebml_data_size);
					
							if (bigint > 0) {
								krad_vpx_encoder_want_keyframe (krad_transponder->krad_link[k]->krad_vpx_encoder);
							}
						}
					}
          *//*
					if ((ebml_id == EBML_ID_KRAD_LINK_LINK_OPUS_BANDWIDTH) || (ebml_id == EBML_ID_KRAD_LINK_LINK_OPUS_SIGNAL)) {

						krad_ipc_server_advanced_string_broadcast ( krad_ipc,
																  EBML_ID_KRAD_TRANSPONDER_MSG,
																  EBML_ID_KRAD_TRANSPONDER_LINK_UPDATED,
																  EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER,
														 		  k,
														 		  ebml_id,
														 		  string);

					} else {
						krad_ipc_server_advanced_number_broadcast ( krad_ipc,
																  EBML_ID_KRAD_TRANSPONDER_MSG,
																  EBML_ID_KRAD_TRANSPONDER_LINK_UPDATED,
																  EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER,
														 		  k,
														 		  ebml_id,
														 		  bigint);
					}
					*//*
				}
			}
			
			break;
*/
    case EBML_ID_KRAD_TRANSPONDER_CMD_LIST_ADAPTERS:

			address.path.unit = KR_TRANSPONDER;
			address.path.subunit.transponder_subunit = KR_ADAPTER;
#ifdef KR_LINUX
			devices = krad_v4l2_detect_devices ();

			for (k = 0; k < devices; k++) {
				if (krad_v4l2_get_device_filename (k, string) > 0) {
          address.id.number = k;
          krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
          kr_ebml2_pack_uint32 ( &ebml_out,
                                 EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                 EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          kr_ebml2_pack_string (&ebml_out, EBML_ID_KRAD_TRANSPONDER_V4L2_DEVICE_FILENAME, string);
          kr_ebml2_finish_element (&ebml_out, payload);
          kr_ebml2_finish_element (&ebml_out, response);
        }
      }
#endif
			devices = krad_decklink_detect_devices();

			for (k = 0; k < devices; k++) {
				if (krad_decklink_get_device_name (k, string) > 0) {
          address.id.number = k;
          krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
          kr_ebml2_pack_uint32 ( &ebml_out,
                                 EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                 EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          kr_ebml2_pack_string (&ebml_out, EBML_ID_KRAD_TRANSPONDER_DECKLINK_DEVICE_NAME, string);
          kr_ebml2_finish_element (&ebml_out, payload);
          kr_ebml2_finish_element (&ebml_out, response);
        }
      }
      break;
		case EBML_ID_KRAD_TRANSPONDER_CMD_LISTEN_ENABLE:
      kr_ebml2_unpack_element_uint16 (&ebml_in, &element, &port);
			krad_receiver_listen_on (krad_transponder->krad_receiver, port);
			break;
		case EBML_ID_KRAD_TRANSPONDER_CMD_LISTEN_DISABLE:
			krad_receiver_stop_listening (krad_transponder->krad_receiver);
			break;
		case EBML_ID_KRAD_TRANSPONDER_CMD_TRANSMITTER_ENABLE:
      kr_ebml2_unpack_element_uint16 (&ebml_in, &element, &port);
			krad_transmitter_listen_on (krad_transponder->krad_transmitter, port);
			break;
		case EBML_ID_KRAD_TRANSPONDER_CMD_TRANSMITTER_DISABLE:
			krad_transmitter_stop_listening (krad_transponder->krad_transmitter);
			break;			
    default:
      return -1;    
  }

  if ((ebml_out.pos > 0) && (!krad_ipc_server_current_client_is_subscriber (kr_ipc))) {
    krad_radio_pack_shipment_terminator (&ebml_out);
  }

  kr_io2_pulled (in, ebml_in.pos);
  kr_io2_advance (out, ebml_out.pos);
  
  return 0;
}
