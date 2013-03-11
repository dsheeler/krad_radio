#include "krad_radio_client.h"
#include "krad_radio_client_internal.h"
#include "krad_transponder_common.h"

static int kr_transponder_crate_get_string_from_adapter (kr_crate_t *crate, char **string, int maxlen);

void kr_transponder_receiver_enable (kr_client_t *client, int port) {

  unsigned char *linker_command;
  unsigned char *enable_linker;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_LISTEN_ENABLE, &enable_linker);  

  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port);

  kr_ebml2_finish_element (client->ebml2, enable_linker);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
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

void kr_transponder_transmitter_enable (kr_client_t *client, int port) {

  unsigned char *linker_command;
  unsigned char *enable_transmitter;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_TRANSMITTER_ENABLE, &enable_transmitter);  

  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port);

  kr_ebml2_finish_element (client->ebml2, enable_transmitter);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
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

void kr_transponder_capture (kr_client_t *client, krad_link_video_source_t video_source, char *device,
                   int width, int height, int fps_numerator, int fps_denominator,
                   krad_link_av_mode_t av_mode, char *audio_input, char *codec) {

  unsigned char *linker_command;
  unsigned char *create_link;
  unsigned char *link;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_CREATE_LINK, &create_link);

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK, &link);  
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_OPERATION_MODE, krad_link_operation_mode_to_string (CAPTURE));
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_VIDEO_SOURCE, krad_link_video_source_to_string (video_source));

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_CAPTURE_DEVICE, device);
  
  if (video_source == DECKLINK) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_CAPTURE_DECKLINK_AUDIO_INPUT, audio_input);
  }
  
  if (video_source == V4L2) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_CAPTURE_CODEC, codec);
  }  
  
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_VIDEO_WIDTH, width);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_VIDEO_HEIGHT, height);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_FPS_NUMERATOR, fps_numerator);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_FPS_DENOMINATOR, fps_denominator);

  kr_ebml2_finish_element (client->ebml2, link);

  kr_ebml2_finish_element (client->ebml2, create_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_play_remote (kr_client_t *client, char *host, int port, char *mount) {

  unsigned char *linker_command;
  unsigned char *create_link;
  unsigned char *link;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_CREATE_LINK, &create_link);

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK, &link);  
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_OPERATION_MODE, krad_link_operation_mode_to_string (PLAYBACK));
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_TRANSPORT_MODE, "tcp");
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_HOST, host);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_PORT, port);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_MOUNT, mount);

  kr_ebml2_finish_element (client->ebml2, link);

  kr_ebml2_finish_element (client->ebml2, create_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);

  kr_client_push (client);
}

void kr_transponder_play (kr_client_t *client, char *path) {

  unsigned char *linker_command;
  unsigned char *create_link;
  unsigned char *link;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_CREATE_LINK, &create_link);

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK, &link);  
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_OPERATION_MODE, krad_link_operation_mode_to_string (PLAYBACK));
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_TRANSPORT_MODE, "filesystem");
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_FILENAME, path);
  kr_ebml2_finish_element (client->ebml2, link);

  kr_ebml2_finish_element (client->ebml2, create_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_transmit (kr_client_t *client, krad_link_av_mode_t av_mode,
                  char *host, int port, char *mount, char *password, char *codecs,
                  int video_width, int video_height, int video_bitrate, char *audio_bitrate) {

  unsigned char *linker_command;
  unsigned char *create_link;
  unsigned char *link;
  int passthru;  
  
  krad_codec_t audio_codec;
  krad_codec_t video_codec;
  
  int audio_bitrate_int;
  float vorbis_quality;
  
  audio_codec = VORBIS;
  video_codec = VP8;
  passthru = 0;

  if (codecs != NULL) {
    audio_codec = krad_string_to_audio_codec (codecs);
    video_codec = krad_string_to_video_codec (codecs);
    if (strstr(codecs, "pass") != NULL) {
      passthru = 1;
    }
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_CREATE_LINK, &create_link);

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK, &link);  
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_OPERATION_MODE, krad_link_operation_mode_to_string (TRANSMIT));
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_AV_MODE, krad_link_av_mode_to_string (av_mode));

  if ((av_mode == VIDEO_ONLY) || (av_mode == AUDIO_AND_VIDEO)) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_VIDEO_CODEC, krad_codec_to_string (video_codec));
    
    if (video_codec == VP8) {
      if (video_bitrate == 0) {
        video_bitrate = 92 * 8;
      }
    }
    
    if (video_codec == THEORA) {
      if ((video_width % 16) || (video_height % 16)) {
        video_width = 0;
        video_height = 0;
      }
      if (video_bitrate == 0) {
        video_bitrate = 31;
      }
    }
    
    if ((video_codec == MJPEG) || (video_codec == H264)) {
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_USE_PASSTHRU_CODEC, passthru);
    }
    
    kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_VIDEO_WIDTH, video_width);
    kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_VIDEO_HEIGHT, video_height);
    
    if ((video_codec == VP8) || (video_codec == H264)) {
      if (video_bitrate < 100) {
        video_bitrate = 100;
      }
      
      if (video_bitrate > 10000) {
        video_bitrate = 10000;
      }
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_VP8_BITRATE, video_bitrate);  
    }
    
    if (video_codec == THEORA) {
    
      if (video_bitrate < 0) {
        video_bitrate = 0;
      }
      
      if (video_bitrate > 63) {
        video_bitrate = 63;
      }
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_THEORA_QUALITY, video_bitrate);  
    }  
  }

  if ((av_mode == AUDIO_ONLY) || (av_mode == AUDIO_AND_VIDEO)) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_AUDIO_CODEC, krad_codec_to_string (audio_codec));
    if (audio_codec == FLAC) {
      if (atoi(audio_bitrate) == 24) {
        kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_FLAC_BIT_DEPTH, 24);
      } else {
        kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_FLAC_BIT_DEPTH, 16);
      }
    }
    if (audio_codec == VORBIS) {
      vorbis_quality = atof(audio_bitrate);
      if (vorbis_quality == 0.0) {
        vorbis_quality = 0.4;
      }
      if (vorbis_quality > 0.8) {
        vorbis_quality = 0.8;
      } 
      if (vorbis_quality < 0.2) {
        vorbis_quality = 0.2;
      }
    
      kr_ebml2_pack_float (client->ebml2, EBML_ID_KRAD_LINK_LINK_VORBIS_QUALITY, vorbis_quality);
    }
    if (audio_codec == OPUS) {
      audio_bitrate_int = atoi(audio_bitrate);
      if (audio_bitrate_int == 0) {
        audio_bitrate_int = 132000;
      } 
      if (audio_bitrate_int < 5000) {
        audio_bitrate_int = 5000;
      }
      if (audio_bitrate_int > 320000) {
        audio_bitrate_int = 320000;
      }
      
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_OPUS_BITRATE, audio_bitrate_int);
    }
  }
  
  if (strcmp(password, "udp") == 0) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_TRANSPORT_MODE, "udp");
  } else {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_TRANSPORT_MODE, "tcp");
  }

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_HOST, host);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_PORT, port);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_MOUNT, mount);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_PASSWORD, password);  
  
  kr_ebml2_finish_element (client->ebml2, link);

  kr_ebml2_finish_element (client->ebml2, create_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_record (kr_client_t *client, krad_link_av_mode_t av_mode, char *filename, char *codecs,
                            int video_width, int video_height, int video_bitrate, char *audio_bitrate) {

  unsigned char *linker_command;
  unsigned char *create_link;
  unsigned char *link;
  int passthru;
  
  krad_codec_t audio_codec;
  krad_codec_t video_codec;
  
  int audio_bitrate_int;
  float vorbis_quality;
  
  audio_codec = VORBIS;
  video_codec = VP8;
  passthru = 0;
    
  if (codecs != NULL) {
    audio_codec = krad_string_to_audio_codec (codecs);
    video_codec = krad_string_to_video_codec (codecs);
    if (strstr(codecs, "pass") != NULL) {
      passthru = 1;
    }
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_CREATE_LINK, &create_link);

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK, &link);  
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_OPERATION_MODE, krad_link_operation_mode_to_string (RECORD));
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_AV_MODE, krad_link_av_mode_to_string (av_mode));

  if ((av_mode == VIDEO_ONLY) || (av_mode == AUDIO_AND_VIDEO)) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_VIDEO_CODEC, krad_codec_to_string (video_codec));
    
    if (video_codec == VP8) {
      if (video_bitrate == 0) {
        video_bitrate = 140 * 8;
      }
    }
    
    if (video_codec == THEORA) {
      if (video_bitrate == 0) {
        video_bitrate = 41;
      }
    }
    
    if ((video_codec == MJPEG) || (video_codec == H264)) {
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_USE_PASSTHRU_CODEC, passthru);
    }

    kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_VIDEO_WIDTH, video_width);
    kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_VIDEO_HEIGHT, video_height);
    
    if ((video_codec == VP8) || (video_codec == H264)) {
      if (video_bitrate < 100) {
        video_bitrate = 100;
      }
      
      if (video_bitrate > 10000) {
        video_bitrate = 10000;
      }
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_VP8_BITRATE, video_bitrate);  
    }
    
    if (video_codec == THEORA) {
    
      if (video_bitrate < 0) {
        video_bitrate = 0;
      }
      
      if (video_bitrate > 63) {
        video_bitrate = 63;
      }
    
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_THEORA_QUALITY, video_bitrate);  
    }  
    
  }

  if ((av_mode == AUDIO_ONLY) || (av_mode == AUDIO_AND_VIDEO)) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_AUDIO_CODEC, krad_codec_to_string (audio_codec));
    if (audio_codec == FLAC) {
      if (atoi(audio_bitrate) == 24) {
        kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_FLAC_BIT_DEPTH, 24);
      } else {
        kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_FLAC_BIT_DEPTH, 16);
      }
    }
    if (audio_codec == VORBIS) {
      vorbis_quality = atof(audio_bitrate);
      if (vorbis_quality == 0.0) {
        vorbis_quality = 0.4;
      }
      if (vorbis_quality > 0.8) {
        vorbis_quality = 0.8;
      } 
      if (vorbis_quality < 0.2) {
        vorbis_quality = 0.2;
      }
    
      kr_ebml2_pack_float (client->ebml2, EBML_ID_KRAD_LINK_LINK_VORBIS_QUALITY, vorbis_quality);
    }
    if (audio_codec == OPUS) {
      audio_bitrate_int = atoi(audio_bitrate);
      if (audio_bitrate_int == 0) {
        audio_bitrate_int = 132000;
      } 
      if (audio_bitrate_int < 5000) {
        audio_bitrate_int = 5000;
      }
      if (audio_bitrate_int > 320000) {
        audio_bitrate_int = 320000;
      }
      
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_LINK_LINK_OPUS_BITRATE, audio_bitrate_int);
    }
  }

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_FILENAME, filename);
  
  kr_ebml2_finish_element (client->ebml2, link);

  kr_ebml2_finish_element (client->ebml2, create_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
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

void kr_transponder_list (kr_client_t *client) {

  unsigned char *linker_command;
  unsigned char *list_links;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_LIST_LINKS, &list_links);
  kr_ebml2_finish_element (client->ebml2, list_links);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_destroy (kr_client_t *client, int number) {

  unsigned char *linker_command;
  unsigned char *destroy_link;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_DESTROY_LINK, &destroy_link);

  kr_ebml2_pack_int8 (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER, number);

  kr_ebml2_finish_element (client->ebml2, destroy_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);

  kr_client_push (client);
}

void kr_transponder_update (kr_client_t *client, int number, uint32_t ebml_id, int newval) {

  unsigned char *linker_command;
  unsigned char *update_link;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_UPDATE_LINK, &update_link);

  kr_ebml2_pack_int8 (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER, number);
  kr_ebml2_pack_int32 (client->ebml2, ebml_id, newval);

  kr_ebml2_finish_element (client->ebml2, update_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_update_str (kr_client_t *client, int number, uint32_t ebml_id, char *newval) {

  unsigned char *linker_command;
  unsigned char *update_link;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_UPDATE_LINK, &update_link);

  kr_ebml2_pack_int8 (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER, number);
  kr_ebml2_pack_string (client->ebml2, ebml_id, newval);

  kr_ebml2_finish_element (client->ebml2, update_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

int krad_link_rep_to_string (krad_link_rep_t *krad_link, char *text) {

  int pos;
  
  pos = 0;
  
  pos += sprintf (text + pos, "%s %s", 
          krad_link_operation_mode_to_string (krad_link->operation_mode),
          krad_link_av_mode_to_string (krad_link->av_mode));

  if ((krad_link->operation_mode == RECORD) || (krad_link->operation_mode == TRANSMIT)) {

    if ((krad_link->operation_mode == TRANSMIT) && (krad_link->transport_mode == UDP)) {
      pos += sprintf (text + pos, " %s", krad_link_transport_mode_to_string (krad_link->transport_mode));
    }

    if (krad_link->operation_mode == TRANSMIT) {
      pos += sprintf (text + pos, " to %s:%d%s", krad_link->host, krad_link->port, krad_link->mount);
    }

    if (krad_link->operation_mode == RECORD) {
      pos += sprintf (text + pos, " to %s", krad_link->filename);
    }
        
    if ((krad_link->av_mode == VIDEO_ONLY) || (krad_link->av_mode == AUDIO_AND_VIDEO)) {

      pos += sprintf (text + pos, " Video -");

      pos += sprintf (text + pos, " %dx%d %d/%d %d",
              krad_link->width, krad_link->height,
              krad_link->fps_numerator, krad_link->fps_denominator,
              krad_link->color_depth);

      pos += sprintf (text + pos, " Codec: %s", krad_codec_to_string (krad_link->video_codec));

      if (krad_link->video_codec == THEORA) {
        pos += sprintf (text + pos, " Quality: %d", krad_link->theora_quality); 
      }  
  
      if (krad_link->video_codec == VP8) {
        pos += sprintf (text + pos, " Bitrate: %d Min Quantizer: %d Max Quantizer: %d Deadline: %d",
                krad_link->vp8_bitrate, krad_link->vp8_min_quantizer,
                krad_link->vp8_max_quantizer, krad_link->vp8_deadline);
      }
    }

    if ((krad_link->av_mode == AUDIO_ONLY) || (krad_link->av_mode == AUDIO_AND_VIDEO)) {
      pos += sprintf (text + pos, " Audio -");
      pos += sprintf (text + pos, " Sample Rate: %d", krad_link->audio_sample_rate);
      pos += sprintf (text + pos, " Channels: %d", krad_link->audio_channels);
      pos += sprintf (text + pos, " Codec: %s", krad_codec_to_string (krad_link->audio_codec));
            
      if (krad_link->audio_codec == FLAC) {
        pos += sprintf (text + pos, " Bit Depth: %d", krad_link->flac_bit_depth);
      }    

      if (krad_link->audio_codec == VORBIS) {
        pos += sprintf (text + pos, " Quality: %.1f", krad_link->vorbis_quality); 
      }    

      if (krad_link->audio_codec == OPUS) {
        pos += sprintf (text + pos, " Complexity: %d Bitrate: %d Frame Size: %d Signal: %s Bandwidth: %s", krad_link->opus_complexity,
                krad_link->opus_bitrate, krad_link->opus_frame_size, krad_opus_signal_to_string (krad_link->opus_signal),
                krad_opus_bandwidth_to_string (krad_link->opus_bandwidth));

      }
    }
  }

  if (krad_link->operation_mode == RECEIVE) {
    if ((krad_link->transport_mode == UDP) || (krad_link->transport_mode == TCP)) {
      pos += sprintf (text + pos, " Port %d", krad_link->port);
    }
  }
  
  if (krad_link->operation_mode == PLAYBACK) {
    if (krad_link->transport_mode == FILESYSTEM) {
      pos += sprintf (text + pos, " File %s", krad_link->filename);
    }

    if (krad_link->transport_mode == TCP) {
      pos += sprintf (text + pos, " %s:%d%s",
              krad_link->host, krad_link->port, krad_link->mount);
    }
  }
  
  if (krad_link->operation_mode == CAPTURE) {
    pos += sprintf (text + pos, " from %s", krad_link_video_source_to_string (krad_link->video_source));
    pos += sprintf (text + pos, " with device %s", krad_link->video_device);    

    if ((krad_link->av_mode == VIDEO_ONLY) || (krad_link->av_mode == AUDIO_AND_VIDEO)) {

      pos += sprintf (text + pos, " at ");

      //pos += sprintf (text + pos, " %dx%d %d/%d %d",
      //        krad_link->width, krad_link->height,
      //        krad_link->fps_numerator, krad_link->fps_denominator,
      //        krad_link->color_depth);

      pos += sprintf (text + pos, " %dx%d %d/%d",
              krad_link->width, krad_link->height,
              krad_link->fps_numerator, krad_link->fps_denominator);

    }

  }

  return pos;
}

static int kr_transponder_crate_get_string_from_adapter (kr_crate_t *crate, char **string, int maxlen) {

  int len;

  len = 0;

  len += sprintf (*string + len, "Adapter: ");
  kr_ebml2_unpack_element_string (&crate->payload_ebml, NULL, *string + len, maxlen);

  return len; 
}

int kr_transponder_crate_to_string (kr_crate_t *crate, char **string) {

  switch ( crate->address.path.subunit.transponder_subunit ) {
    case KR_ADAPTER:
      *string = kr_response_alloc_string (crate->size * 8);
      return kr_transponder_crate_get_string_from_adapter (crate, string, crate->size * 8);
      
    case KR_TRANSMITTER:
      break;
  
    case KR_RECEIVER:
      break;
    case KR_DEMUXER:
      break;  
    case KR_MUXER:
      break;
    case KR_ENCODER:
      break;
    case KR_DECODER:
      break;
  }
  
  return 0;  
}

