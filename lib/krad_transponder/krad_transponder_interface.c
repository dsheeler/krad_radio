#include "krad_transponder_interface.h"

static void kr_transponder_info_to_ebml(kr_ebml *ebml, kr_xpdr_info *info) {
  kr_ebml_pack_uint16(ebml, EBML_ID_KRAD_RADIO_TCP_PORT, info->active_paths);
}

static void transponder_info_ebml(kr_ebml *ebml, kr_transponder *xpdr) {
  kr_transponder_info info;
  //memset(&info, 0, sizeof (kr_transponder_info));
  kr_transponder_get_info(xpdr, &info);
  kr_transponder_info_to_ebml(ebml, &info);
}

/*
void kr_opus_encoder_to_rep (krad_opus_t *encoder, kr_opus_encoder_t *rep) {
  rep->bitrate = krad_opus_get_bitrate (encoder);
  rep->complexity = krad_opus_get_complexity (encoder);
  rep->frame_size = krad_opus_get_frame_size (encoder);
  rep->signal = krad_opus_get_signal (encoder);
  rep->bandwidth = krad_opus_get_bandwidth (encoder);
}

void kr_vorbis_encoder_to_rep (krad_vorbis_t *encoder, kr_vorbis_encoder_t *rep) {
  rep->quality = encoder->quality;
}

void kr_flac_encoder_to_rep (krad_flac_t *encoder, kr_flac_encoder_t *rep) {
  rep->bit_depth = encoder->bit_depth;
}

void kr_theora_encoder_to_rep (krad_theora_encoder_t *encoder, kr_theora_encoder_t *rep) {
  rep->kf_distance = encoder->keyframe_distance;
  rep->speed = encoder->speed;
  rep->quality = encoder->quality;
}

void kr_vpx_encoder_to_rep (krad_vpx_encoder_t *encoder, kr_vpx_encoder_t *rep) {
  rep->bitrate = encoder->bitrate;
  rep->deadline = encoder->deadline;
}
*/

int kr_transponder_cmd(kr_io2_t *in, kr_io2_t *out, kr_radio_client *client) {

//  int i;
//  int devices;
  kr_radio *radio;
  kr_transponder *transponder;
  kr_address address;
  kr_app_server *app;
  unsigned char *response;
  unsigned char *payload;
  kr_ebml2_t ebml_in;
  kr_ebml2_t ebml_out;
  uint32_t command;
  uint32_t element;
  uint64_t size;
  int ret;
  uint16_t port;
/*  char string[512];
  char string2[512];

  string[0] = '\0';
  string2[0] = '\0';
*/
  port = 0;
  radio = client->krad_radio;
  transponder = radio->transponder;
  app = radio->app;

  if (!(kr_io2_has_in(in))) {
    return 0;
  }

  kr_ebml2_set_buffer(&ebml_in, in->rd_buf, in->len);

  ret = kr_ebml2_unpack_id(&ebml_in, &command, &size);
  if ((ret < 0) || (command != EBML_ID_KRAD_TRANSPONDER_CMD)) {
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
    case EBML_ID_KRAD_TRANSPONDER_CMD_GET_INFO:
      address.path.unit = KR_TRANSPONDER;
      address.path.subunit.transponder_subunit = KR_UNIT;
      krad_radio_address_to_ebml2(&ebml_out, &response, &address);
      kr_ebml_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
       EBML_ID_KRAD_UNIT_INFO);
      kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      transponder_info_ebml(&ebml_out, transponder);
      kr_ebml2_finish_element(&ebml_out, payload);
      kr_ebml2_finish_element(&ebml_out, response);
      break;
/*
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_LIST:
      num = kr_xpdr_count (krad_transponder->xpdr);
      for (i = 0; i < KRAD_TRANSPONDER_MAX_SUBUNITS; i++) {
        if (!krad_transponder_subunit_to_rep ( krad_transponder, i, &transponder_subunit_rep )) {
          continue;
        }
        krad_transponder_subunit_address ( &transponder_subunit_rep, i, &address );
        krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
        kr_ebml_pack_uint32 ( &ebml_out,
                               EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                               EBML_ID_KRAD_SUBUNIT_INFO);
        kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        krad_transponder_subunit_rep_to_ebml ( &ebml_out, &transponder_subunit_rep );
        kr_ebml2_finish_element (&ebml_out, payload);
        kr_ebml2_finish_element (&ebml_out, response);
      }
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_DESTROY:
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &num);
      if (krad_transponder->krad_link[num] != NULL) {
        krad_link_destroy (krad_transponder->krad_link[num]);
        krad_transponder->krad_link[num] = NULL;
        break;
      }
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_UPDATE:
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_CREATE:

      kr_ebml2_unpack_element_string (&ebml_in, &element, string, sizeof(string));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string2, sizeof(string2));

      if (kr_txpdr_string_to_subunit_type(string) == FAILURE) {
        break;
      }

      for (i = 0; i < KRAD_TRANSPONDER_MAX_SUBUNITS; i++) {
        if (krad_transponder->krad_link[i] == NULL) {
          krad_transponder->krad_link[i] = krad_link_prepare (i);
          link = krad_transponder->krad_link[i];
          link->link_num = i;
          link->krad_radio = krad_transponder->krad_radio;
          link->krad_transponder = krad_transponder;
          link->type = kr_txpdr_string_to_subunit_type (string);
          break;
        }
      }

      if (link == NULL) {
        break;
      }

      if (link->type == DEMUX) {
        printk ("demuxx!");
        link->transport_mode = FILESYSTEM;
        sprintf (link->input, "%s", string2);
      }

      if (link->type == DECODE) {
        printk ("decode!!");
        //link->av_mode = VIDEO_ONLY;
        link->av_mode = AUDIO_ONLY;
        sprintf (link->input, "%s", string2);
      }

      if (link->type == MUX) {

        sprintf (link->input, "%s", string2);

        if (strstr(string2, ".udp") != NULL) {
          if (strstr(string2, ".udpre") != NULL) {
              link->port = 44000;
              strcpy (link->host, "50.17.250.78");
           } else {
            link->port = 3400;
            strcpy (link->host, "127.0.0.1");
           }
            strcpy (link->mount, "/krad.udp");
            strcpy (link->password, "firefox");
        } else {
          if (strstr(string2, "stream") != NULL) {
          //if (1) {
            strcpy (link->host, "europa.kradradio.com");
            link->port = 8008;
            if (strstr(string2, "mkv") != NULL) {
              snprintf (link->mount, sizeof(link->mount),
                        "/krad_radio_%s.mkv",
                        link->krad_radio->sysname);
            } else {
              if (strstr(string2, "webm") != NULL) {
                snprintf (link->mount, sizeof(link->mount),
                          "/krad_radio_%s.webm",
                          link->krad_radio->sysname);
              } else {
                snprintf (link->mount, sizeof(link->mount),
                          "/krad_radio_%s.ogg",
                          link->krad_radio->sysname);
              }
            }
            strcpy (link->password, "firefox");
          } else {
            if (strstr(string2, "ogg") != NULL) {
              sprintf (link->output,
                       "%s/kr_test_%"PRIu64".ogg", getenv ("HOME"), krad_unixtime ());
            } else {
              sprintf (link->output,
                       "%s/kr_test_%"PRIu64".webm", getenv ("HOME"), krad_unixtime ());
            }
          }
        }
      }

      if (link->type == ENCODE) {
        if (string2[0] == 'v') {
          link->av_mode = VIDEO_ONLY;
          link->codec = THEORA;
          if (string2[1] == 'v') {
            link->codec = KVHS;
          } else {
            if (string2[1] == 't') {
             link->codec = THEORA;
            } else {
             link->codec = VP8;
            }
          }
        } else {
          link->av_mode = AUDIO_ONLY;
          if (string2[1] == 'o') {
            link->codec = OPUS;
          } else {
            if (string2[1] == 'f') {
             link->codec = FLAC;
            } else {
             link->codec = VORBIS;
            }
          }
        }
      }
      if (link->type == RAWOUT) {
        link->av_mode = VIDEO_ONLY;
      }

      if (link->type == RAWIN) {
        link->av_mode = VIDEO_ONLY;
        link->video_source = krad_link_string_to_video_source (string2);
        if (link->video_source == NOVIDEO) {
          free (link);
          krad_transponder->krad_link[i] = NULL;
          break;
        }
      }
      krad_link_start (link);
      break;
  */
    case EBML_ID_KRAD_TRANSPONDER_CMD_LIST_ADAPTERS:
      address.path.unit = KR_TRANSPONDER;
      address.path.subunit.transponder_subunit = KR_ADAPTER;
      /*
#ifdef KR_LINUX
      devices = krad_v4l2_detect_devices();
      for (i = 0; i < devices; i++) {
        if (krad_v4l2_get_device_filename(i, string) > 0) {
          address.id.number = i;
          krad_radio_address_to_ebml2(&ebml_out, &response, &address);
          kr_ebml_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
           EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD,
           &payload);
          kr_ebml_pack_string(&ebml_out,
           EBML_ID_KRAD_TRANSPONDER_V4L2_DEVICE_FILENAME, string);
          kr_ebml2_finish_element(&ebml_out, payload);
          kr_ebml2_finish_element(&ebml_out, response);
        }
      }
#endif
      devices = krad_decklink_detect_devices();
      for (i = 0; i < devices; i++) {
        if (krad_decklink_get_device_name(i, string) > 0) {
          address.id.number = i;
          krad_radio_address_to_ebml2(&ebml_out, &response, &address);
          kr_ebml_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
           EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD,
           &payload);
          kr_ebml_pack_string(&ebml_out,
           EBML_ID_KRAD_TRANSPONDER_DECKLINK_DEVICE_NAME, string);
          kr_ebml2_finish_element(&ebml_out, payload);
          kr_ebml2_finish_element(&ebml_out, response);
        }
      }
      */
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_LISTEN_ENABLE:
      kr_ebml2_unpack_element_uint16(&ebml_in, &element, &port);
      //krad_receiver_listen_on (krad_transponder->krad_receiver, port);
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_LISTEN_DISABLE:
      //krad_receiver_stop_listening (krad_transponder->krad_receiver);
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_TRANSMITTER_ENABLE:
      kr_ebml2_unpack_element_uint16(&ebml_in, &element, &port);
      //krad_transmitter_listen_on(krad_transponder->krad_transmitter, port);
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_TRANSMITTER_DISABLE:
      //krad_transmitter_stop_listening(krad_transponder->krad_transmitter);
      break;
    default:
      return -1;
  }

  if (((ebml_out.pos > 0) ||
       (command == EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_LIST)) &&
       (!krad_app_server_current_client_is_subscriber(app))) {
    krad_radio_pack_shipment_terminator(&ebml_out);
  }
  kr_io2_pulled(in, ebml_in.pos);
  kr_io2_advance(out, ebml_out.pos);
  return 0;
}
