#include "krad_radio_client.h"
#include "krad_radio_client_internal.h"
#include "krad_transponder_common.h"

static int kr_transponder_get_string_from_muxer (kr_muxer_t *muxer, char *string, int maxlen);
static int kr_transponder_get_string_from_demuxer (kr_demuxer_t *demuxer, char *string, int maxlen);
static int kr_transponder_get_string_from_encoder (kr_encoder_t *encoder, char *string, int maxlen);
static int kr_transponder_get_string_from_decoder (kr_decoder_t *decoder, char *string, int maxlen);

static int kr_transponder_crate_get_string_from_adapter (kr_crate_t *crate, char **string, int maxlen);

int kr_transponder_receiver_enable (kr_client_t *client, int port) {

  unsigned char *linker_command;
  unsigned char *enable_linker;
  uint16_t port_actual;

  if (!(kr_sys_port_valid (port))) {
    return -1;
  } else {
    port_actual = port;
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_LISTEN_ENABLE, &enable_linker);  

  kr_ebml2_pack_uint16 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port_actual);

  kr_ebml2_finish_element (client->ebml2, enable_linker);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
  
  return 1;
}

void kr_transponder_receiver_disable (kr_client_t *client) {

  unsigned char *linker_command;
  unsigned char *disable_linker;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_LISTEN_DISABLE, &disable_linker);
  kr_ebml2_finish_element (client->ebml2, disable_linker);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

int kr_transponder_transmitter_enable (kr_client_t *client, int port) {

  unsigned char *linker_command;
  unsigned char *enable_transmitter;
  uint16_t port_actual;

  if (!(kr_sys_port_valid (port))) {
    return -1;
  } else {
    port_actual = port;
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_TRANSMITTER_ENABLE, &enable_transmitter);  

  kr_ebml2_pack_uint16 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port_actual);

  kr_ebml2_finish_element (client->ebml2, enable_transmitter);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);

  return 1;
}

void kr_transponder_transmitter_disable (kr_client_t *client) {

  unsigned char *linker_command;
  unsigned char *disable_transmitter;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_TRANSMITTER_DISABLE, &disable_transmitter);
  kr_ebml2_finish_element (client->ebml2, disable_transmitter);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_info (kr_client_t *client) {

  unsigned char *command;
  unsigned char *info_command;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_GET_INFO, &info_command);
  kr_ebml2_finish_element (client->ebml2, info_command);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_transponder_adapters (kr_client_t *client) {

  unsigned char *linker_command;
  unsigned char *adapters;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_LIST_ADAPTERS, &adapters);

  kr_ebml2_finish_element (client->ebml2, adapters);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_subunit_list (kr_client_t *client) {

  unsigned char *linker_command;
  unsigned char *list_links;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_LIST, &list_links);
  kr_ebml2_finish_element (client->ebml2, list_links);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_subunit_destroy (kr_client_t *client, uint32_t number) {

  unsigned char *linker_command;
  unsigned char *destroy_link;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_DESTROY, &destroy_link);

  kr_ebml2_pack_uint32 (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER, number);

  kr_ebml2_finish_element (client->ebml2, destroy_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);

  kr_client_push (client);
}

void kr_transponder_subunit_update (kr_client_t *client, uint32_t number, uint32_t ebml_id, int newval) {

  unsigned char *linker_command;
  unsigned char *update_link;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_UPDATE, &update_link);
/*
  kr_ebml2_pack_int8 (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER, number);
  kr_ebml2_pack_int32 (client->ebml2, ebml_id, newval);
*/
  kr_ebml2_finish_element (client->ebml2, update_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_subunit_update_str (kr_client_t *client, uint32_t number, uint32_t ebml_id, char *newval) {

  unsigned char *linker_command;
  unsigned char *update_link;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_UPDATE, &update_link);
/*
  kr_ebml2_pack_int8 (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER, number);
  kr_ebml2_pack_string (client->ebml2, ebml_id, newval);
*/
  kr_ebml2_finish_element (client->ebml2, update_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_subunit_create (kr_client_t *client, char *mode, char *option) {

  unsigned char *linker_command;
  unsigned char *create_link;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_CREATE, &create_link);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_OPERATION_MODE, mode);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_FILENAME, option);

  kr_ebml2_finish_element (client->ebml2, create_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

static int kr_transponder_crate_get_string_from_adapter (kr_crate_t *crate, char **string, int maxlen) {

  int len;

  len = 0;

  len += sprintf (*string + len, "Adapter: ");
  kr_ebml2_unpack_element_string (&crate->payload_ebml, NULL, *string + len, maxlen);

  return len; 
}

static void kr_ebml_to_transponder_rep (kr_ebml2_t *ebml, kr_transponder_t *transponder_rep) {
  kr_ebml2_unpack_element_uint16 (ebml, NULL, &transponder_rep->receiver_port);
  kr_ebml2_unpack_element_uint16 (ebml, NULL, &transponder_rep->transmitter_port);
}

static int kr_transponder_crate_get_string_from_transponder (kr_crate_t *crate, char **string, int maxlen) {

  kr_transponder_t transponder;
  int len;

  len = 0;

  kr_ebml_to_transponder_rep (&crate->payload_ebml, &transponder);

  len += sprintf (*string + len, "Transponder Status:\n");
  if (transponder.receiver_port == 0) {
    len += sprintf (*string + len, "Receiver Off\n");
  } else {
    len += sprintf (*string + len, "Receiver On: Port %u\n", transponder.receiver_port);
  }

  if (transponder.transmitter_port == 0) {
    len += sprintf (*string + len, "Transmitter Off");
  } else {
    len += sprintf (*string + len, "Transmitter On: Port %u", transponder.transmitter_port);
  }
  
  return len; 
}

static void kr_ebml_to_transponder_subunit_rep (kr_ebml2_t *ebml, kr_transponder_subunit_t *tr) {
  kr_ebml2_unpack_element_int32 (ebml, NULL, (int32_t *)&tr->type);
  kr_ebml2_unpack_element_data (ebml, NULL, &tr->actual, sizeof(tr->actual));  
}


static int kr_transponder_get_string_from_muxer (kr_muxer_t *muxer, char *string, int maxlen) {

  int len;
  
  len = 0;

  len += sprintf (string + len, "%s Muxer", kr_container_type_to_string (muxer->type));
  return len;
}

static int kr_transponder_get_string_from_demuxer (kr_demuxer_t *demuxer, char *string, int maxlen) {

  int len;
  
  len = 0;

  len += sprintf (string + len, "%s Demuxer", kr_container_type_to_string (demuxer->type));
  return len;
}

static int kr_transponder_get_string_from_decoder (kr_decoder_t *decoder, char *string, int maxlen) {

  int len;
  
  len = 0;

  len += sprintf (string + len, "%s Decoder", krad_codec_to_string (decoder->codec));
  return len;
}

static int kr_transponder_get_string_from_encoder (kr_encoder_t *encoder, char *string, int maxlen) {

  int len;
  
  len = 0;

  len += sprintf (string + len, "%s Encoder ", krad_codec_to_string (encoder->codec));
  
    
  if (krad_codec_is_audio (encoder->codec)) {
    len += sprintf (string + len, "%d Channels %dhz",
                    encoder->av.audio.channels,
                    encoder->av.audio.sample_rate);
    if (encoder->codec == FLAC) {
      len += sprintf (string + len, " Bit Depth: %d", encoder->av.audio.codec.flac.bit_depth);
    }
    if (encoder->codec == VORBIS) {
      len += sprintf (string + len, " Quality: %3.1f", encoder->av.audio.codec.vorbis.quality);
    }
    if (encoder->codec == OPUS) {
      if ((encoder->av.audio.codec.opus.bitrate % 1000) == 0) {
        len += sprintf (string + len, " %dKB/s", encoder->av.audio.codec.opus.bitrate / 1000);
      } else {
        len += sprintf (string + len, " %d", encoder->av.audio.codec.opus.bitrate);      
      }
      len += sprintf (string + len, " %s Signal", krad_opus_signal_to_nice_string(encoder->av.audio.codec.opus.signal));
      len += sprintf (string + len, " Complexity: %d", encoder->av.audio.codec.opus.complexity);
      len += sprintf (string + len, " Frame Size: %d", encoder->av.audio.codec.opus.frame_size);
      len += sprintf (string + len, " Bandwidth: %s", krad_opus_bandwidth_to_nice_string(encoder->av.audio.codec.opus.bandwidth));
    }
  }

  if (krad_codec_is_video (encoder->codec)) {
    len += sprintf (string + len, "%dx%d %d/%d",
                    encoder->av.video.width,
                    encoder->av.video.height,
                    encoder->av.video.fps_numerator,
                    encoder->av.video.fps_denominator);
    if (encoder->codec == THEORA) {
      len += sprintf (string + len, " %s", kr_color_depth_to_string (encoder->av.video.color_depth));
      len += sprintf (string + len, " Quality: %d Speed: %d", encoder->av.video.codec.theora.quality, encoder->av.video.codec.theora.speed);
      if (encoder->av.video.codec.theora.kf_distance != 0) {
        len += sprintf (string + len, " Forced KF Distance: %d", encoder->av.video.codec.theora.kf_distance);
      }      
    }
    if (encoder->codec == VP8) {
      len += sprintf (string + len, " Bitrate: %uKB/s Deadline: %"PRIu64"", encoder->av.video.codec.vpx.bitrate / 8, encoder->av.video.codec.vpx.deadline);
    }
    len += sprintf (string + len, " %"PRIu64" Frames", encoder->av.video.frames);
    if (encoder->av.video.bytes < 1000000000) {
      len += sprintf (string + len, " %"PRIu64" KB", encoder->av.video.bytes / 1000);
    } else {
      len += sprintf (string + len, " %"PRIu64" MB", encoder->av.video.bytes / 1000000);
    }
  }
  
  return len;
}

static int kr_transponder_crate_get_string_from_subunit (kr_crate_t *crate, char **string, int maxlen) {

  kr_transponder_subunit_t transponder_subunit;
  int len;

  len = 0;

  kr_ebml_to_transponder_subunit_rep (&crate->payload_ebml, &transponder_subunit);

  switch (transponder_subunit.type) {
    case ENCODE:
      len += kr_transponder_get_string_from_encoder (&transponder_subunit.actual.encoder, *string + len, maxlen - len);
      break;
    case DECODE:
      len += kr_transponder_get_string_from_decoder (&transponder_subunit.actual.decoder, *string + len, maxlen - len);
      break;
    case MUX:
      len += kr_transponder_get_string_from_muxer (&transponder_subunit.actual.muxer, *string + len, maxlen - len);
      break;
    case DEMUX:
      len += kr_transponder_get_string_from_demuxer (&transponder_subunit.actual.demuxer, *string + len, maxlen - len);
      break;
    default:
      len += sprintf (*string + len, "%s", kr_txpdr_subunit_type_to_string (transponder_subunit.type));
      break;
  }

  return len;
}

int kr_transponder_crate_to_string (kr_crate_t *crate, char **string) {

  if (crate->notice == EBML_ID_KRAD_UNIT_INFO) {
    *string = kr_response_alloc_string (crate->size * 16);
    return kr_transponder_crate_get_string_from_transponder (crate, string, crate->size * 16);
  }

  switch ( crate->address.path.subunit.transponder_subunit ) {
    case KR_ADAPTER:
      *string = kr_response_alloc_string (crate->size * 8);
      return kr_transponder_crate_get_string_from_adapter (crate, string, crate->size * 8);
    case KR_TRANSMITTER:
    case KR_RECEIVER:
    case KR_RAWIN:
    case KR_RAWOUT:
    case KR_DEMUXER:
    case KR_MUXER:
    case KR_ENCODER:
    case KR_DECODER:
      *string = kr_response_alloc_string (crate->size * 16);
      return kr_transponder_crate_get_string_from_subunit (crate, string, crate->size * 16);
  }
  
  return 0;
}
