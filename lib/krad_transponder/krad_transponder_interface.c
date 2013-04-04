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

void kr_opus_decoder_to_rep (krad_opus_t *decoder, kr_opus_decoder_t *rep) {

}

void kr_vorbis_decoder_to_rep (krad_vorbis_t *decoder, kr_vorbis_decoder_t *rep) {

}

void kr_flac_decoder_to_rep (krad_flac_t *decoder, kr_flac_decoder_t *rep) {

}

void kr_theora_decoder_to_rep (krad_theora_decoder_t *encoder, kr_theora_decoder_t *rep) {

}

void kr_vpx_decoder_to_rep (krad_vpx_decoder_t *encoder, kr_vpx_decoder_t *rep) {

}

void kr_mkv_demuxer_to_rep (kr_mkv_t *mkv, kr_mkv_muxer_t *rep) {

}

void kr_ogg_demuxer_to_rep (krad_ogg_t *ogg, kr_ogg_muxer_t *rep) {

}

void kr_mkv_muxer_to_rep (kr_mkv_t *mkv, kr_mkv_muxer_t *rep) {

}

void kr_ogg_muxer_to_rep (krad_ogg_t *ogg, kr_ogg_muxer_t *rep) {

}

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

int krad_transponder_subunit_to_rep ( krad_transponder_t *krad_transponder,
                                      int num,
                                      kr_transponder_subunit_t *tr ) {

  krad_link_t *link;

  link = kr_xpdr_get_link (krad_transponder->xpdr, num);

  if (link == NULL) {
    return 0;
  }

  memset (tr, 0, sizeof(kr_transponder_subunit_t));

  tr->type = link->type;

  if (tr->type == ENCODE) {
    tr->actual.encoder.codec = link->codec;
    
    if (krad_codec_is_audio (link->codec)) {
      //FIXME
	    tr->actual.encoder.av.audio.sample_rate = krad_transponder->krad_radio->krad_mixer->sample_rate;
	    tr->actual.encoder.av.audio.channels = link->channels;

	    if (link->codec == OPUS) {
	      kr_opus_encoder_to_rep (link->krad_opus, &tr->actual.encoder.av.audio.codec.opus);
	    }
	    if (link->codec == VORBIS) {
	      kr_vorbis_encoder_to_rep (link->krad_vorbis, &tr->actual.encoder.av.audio.codec.vorbis);
	    }
	    if (link->codec == FLAC) {
	      kr_flac_encoder_to_rep (link->krad_flac, &tr->actual.encoder.av.audio.codec.flac);
	    }
    }
    
    if (krad_codec_is_video (link->codec)) {
	    tr->actual.encoder.av.video.width = link->encoding_width;
	    tr->actual.encoder.av.video.height = link->encoding_height;
	    tr->actual.encoder.av.video.fps_numerator = link->encoding_fps_numerator;
	    tr->actual.encoder.av.video.fps_denominator = link->encoding_fps_denominator;
	    
	    if (link->codec == THEORA) {
	      kr_theora_encoder_to_rep (link->krad_theora_encoder, &tr->actual.encoder.av.video.codec.theora);
	      
	      if (link->krad_theora_encoder->color_depth == TH_PF_420) {
	        tr->actual.encoder.av.video.color_depth = 420;
	      }
	      if (link->krad_theora_encoder->color_depth == TH_PF_422) {
	        tr->actual.encoder.av.video.color_depth = 422;
	      }
	      if (link->krad_theora_encoder->color_depth == TH_PF_444) {
	        tr->actual.encoder.av.video.color_depth = 444;
	      }
	      tr->actual.encoder.av.video.bytes = link->krad_theora_encoder->bytes;
	      tr->actual.encoder.av.video.frames = link->krad_theora_encoder->frames;
	    }
	    if (link->codec == VP8) {
	      kr_vpx_encoder_to_rep (link->krad_vpx_encoder, &tr->actual.encoder.av.video.codec.vpx);
	      tr->actual.encoder.av.video.bytes = link->krad_vpx_encoder->bytes;
	      tr->actual.encoder.av.video.frames = link->krad_vpx_encoder->frames;
	    }
	    if (link->codec == KVHS) {
	      tr->actual.encoder.av.video.bytes = link->krad_vhs->bytes;
	      tr->actual.encoder.av.video.frames = link->krad_vhs->frames;
	    }
    }
  }

  if (tr->type == DECODE) {
    tr->actual.decoder.codec = link->codec;
  }  

  if (tr->type == RAWIN) {
    //videoport in
    //audioport in
    //decklink
    //x11
    //v4l2
  }

  if (tr->type == RAWOUT) {
    //videoport out
    //audioport out
    //wayland display
  }

  if (tr->type == MUX) {
    tr->actual.muxer.type = link->krad_container->type;
    if (tr->actual.muxer.type == OGG) {
      kr_ogg_muxer_to_rep (link->krad_container->ogg, &tr->actual.muxer.container.ogg);
    }
    if (tr->actual.muxer.type == MKV) {
      kr_mkv_muxer_to_rep (link->krad_container->mkv, &tr->actual.muxer.container.mkv);
    }
    if (tr->actual.muxer.type == TOGG) {
      
    }
    if (tr->actual.muxer.type == NATIVEFLAC) {
      
    }
    if (tr->actual.muxer.type == Y4MFILE) {
      
    }
  }
  
  if (tr->type == DEMUX) {
    tr->actual.demuxer.type = link->krad_container->type;
    if (tr->actual.muxer.type == OGG) {
      kr_ogg_muxer_to_rep (link->krad_container->ogg, &tr->actual.muxer.container.ogg);
    }
    if (tr->actual.muxer.type == MKV) {
      kr_mkv_muxer_to_rep (link->krad_container->mkv, &tr->actual.muxer.container.mkv);
    }
    if (tr->actual.muxer.type == TOGG) {
      
    }
    if (tr->actual.muxer.type == NATIVEFLAC) {
      
    }
    if (tr->actual.muxer.type == Y4MFILE) {
      
    }
  }
  
  return 1;
}

void krad_transponder_subunit_rep_to_ebml ( kr_ebml2_t *ebml, kr_transponder_subunit_t *tr ) {
  kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_TCP_PORT, tr->type);
  kr_ebml2_pack_data (ebml, EBML_ID_KRAD_RADIO_TCP_PORT, &tr->actual, sizeof(tr->actual));
}

void krad_transponder_subunit_address ( kr_transponder_subunit_t *tr, int num, kr_address_t *address) {
  address->path.unit = KR_TRANSPONDER;
  address->id.number = num;

  switch (tr->type) {
    case RAWIN:
      address->path.subunit.transponder_subunit = KR_RAWIN;
      break;
    case RAWOUT:
      address->path.subunit.transponder_subunit = KR_RAWOUT;
      break;
    case MUX:
      address->path.subunit.transponder_subunit = KR_MUXER;
      break;
    case ENCODE:
      address->path.subunit.transponder_subunit = KR_ENCODER;
      break;
    case DEMUX:
      address->path.subunit.transponder_subunit = KR_DEMUXER;
      break;
    case DECODE:
      address->path.subunit.transponder_subunit = KR_DECODER;
      break;
    default:
      address->path.subunit.transponder_subunit = 0;
  }
}

int krad_transponder_command ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client ) {

  int i;
  uint32_t num;
  int devices;
  krad_radio_t *krad_radio;
  krad_transponder_t *krad_transponder;
  kr_address_t address;
  kr_transponder_subunit_t transponder_subunit_rep;
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
  char string2[512];  
  uint16_t port;
  krad_link_t *link;
  
  link = NULL;  
  string[0] = '\0';
  string2[0] = '\0';  
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
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_LIST:
      num = kr_xpdr_count (krad_transponder->xpdr);
      for (i = 0; i < KRAD_TRANSPONDER_MAX_SUBUNITS; i++) {
        if (!krad_transponder_subunit_to_rep ( krad_transponder, i, &transponder_subunit_rep )) {
          continue;
        }
        krad_transponder_subunit_address ( &transponder_subunit_rep, i, &address );
        krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
        kr_ebml2_pack_uint32 ( &ebml_out,
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
        
        if (strstr(string2, "stream") != NULL) {
        //if (1) {
          strcpy (link->host, "europa.kradradio.com");
          link->port = 8080;
          if (strstr(string2, "mkv") != NULL) {
            strcpy (link->mount, "/krad_radio_streaming.mkv");
          } else {
            if (strstr(string2, "webm") != NULL) {
              strcpy (link->mount, "/krad_radio_streaming.webm");
            } else {
              strcpy (link->mount, "/krad_radio_streaming.ogg");
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
    case EBML_ID_KRAD_TRANSPONDER_CMD_LIST_ADAPTERS:

      address.path.unit = KR_TRANSPONDER;
      address.path.subunit.transponder_subunit = KR_ADAPTER;
#ifdef KR_LINUX
      devices = krad_v4l2_detect_devices ();

      for (i = 0; i < devices; i++) {
        if (krad_v4l2_get_device_filename (i, string) > 0) {
          address.id.number = i;
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

      for (i = 0; i < devices; i++) {
        if (krad_decklink_get_device_name (i, string) > 0) {
          address.id.number = i;
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

  if (((ebml_out.pos > 0) || (command == EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_LIST)) &&
       (!krad_ipc_server_current_client_is_subscriber (kr_ipc))) {
    krad_radio_pack_shipment_terminator (&ebml_out);
  }

  kr_io2_pulled (in, ebml_in.pos);
  kr_io2_advance (out, ebml_out.pos);
  
  return 0;
}
