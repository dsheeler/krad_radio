#include "krad_container.h"

static krad_container_type_t krad_container_select (char *string);

static krad_container_type_t krad_container_select (char *string) {

  if ((strstr(string, ".ogg")) ||
    (strstr(string, ".opus")) ||
    (strstr(string, ".Opus")) ||
    (strstr(string, ".OPUS")) ||
    (strstr(string, ".OGG")) ||
    (strstr(string, ".Ogg")) ||
    (strstr(string, ".oga")) ||
    (strstr(string, ".ogv")) ||
    (strstr(string, ".Oga")) ||    
    (strstr(string, ".OGV")))
  {
    return OGG;
  }

  if (strstr(string, ".y4m")) {
    return Y4MFILE;
  }

  if (strstr(string, ".flac")) {
    return NATIVEFLAC;
  }
  if (strstr(string, ".udp")) {
    return KUDP;
  }
  return MKV;
}

char *krad_container_select_mimetype (char *string) {

  if ((strstr(string, ".ogg")) ||
    (strstr(string, ".opus")) ||
    (strstr(string, ".Opus")) ||
    (strstr(string, ".OPUS")) ||
    (strstr(string, ".OGG")) ||
    (strstr(string, ".Ogg")) ||
    (strstr(string, ".oga")) ||
    (strstr(string, ".ogv")) ||
    (strstr(string, ".Oga")) ||    
    (strstr(string, ".OGV")))
  {
    return "application/ogg";
  }

  if (strstr(string, ".webm")) {
    return "video/webm";
  }
  if (strstr(string, ".y4m")) {
    return "video/y4m";
  }
  if (strstr(string, ".flac")) {
    return "audio/flac";
  }
  return "video/x-matroska";
}

int krad_container_track_count (krad_container_t *container) {
  if (container->type == OGG) {
    return krad_ogg_track_count ( container->ogg );
  } else {
    return kr_mkv_track_count ( container->mkv );
  }
}

krad_codec_t krad_container_track_codec (krad_container_t *container,
                                         uint32_t track) {
  if (container->type == OGG) {
    return krad_ogg_track_codec ( container->ogg, track );
  } else {
    return kr_mkv_track_codec ( container->mkv, track );
  }
}

int krad_container_track_header_size (krad_container_t *container,
                                      uint32_t track, uint32_t header) {
  if (container->type == OGG) {
    return krad_ogg_track_header_size ( container->ogg, track, header );
  } else {
    return kr_mkv_track_header_size ( container->mkv, track, header );    
  }
}

int krad_container_read_track_header (krad_container_t *container,
                                      uint8_t *buffer,
                                      uint32_t track, uint32_t header) {
  if (container->type == OGG) {
    return krad_ogg_read_track_header ( container->ogg, buffer, track, header );
  } else {
    return kr_mkv_read_track_header ( container->mkv, buffer, track, header );    
  }
}

int krad_container_track_changed (krad_container_t *container, uint32_t track) {
  if (container->type == OGG) {
    return krad_ogg_track_changed ( container->ogg, track );
  } else {
    return kr_mkv_track_changed ( container->mkv, track );    
  }
}

int krad_container_track_active (krad_container_t *container, uint32_t track) {
  if (container->type == OGG) {
    return krad_ogg_track_active ( container->ogg, track );
  } else {
    return kr_mkv_track_active ( container->mkv, track );    
  }
}

int krad_container_track_header_count (krad_container_t *ctr, uint32_t track) {
  if (ctr->type == OGG) {
    return krad_ogg_track_header_count ( ctr->ogg, track );
  } else {
    return kr_mkv_track_header_count ( ctr->mkv, track );    
  }
}

int krad_container_read_packet (krad_container_t *container, uint32_t *track,
                                uint64_t *timecode, uint8_t *buffer) {
  if (container->type == OGG) {
    return krad_ogg_read_packet ( container->ogg, track, timecode, buffer );
  } else {
    return kr_mkv_read_packet ( container->mkv, track, timecode, NULL, buffer );
  }
}

krad_container_t *krad_container_create_stream (char *host, uint32_t port,
                                                char *mount, char *password) {

  krad_container_t *container;
  krad_container_type_t type;
  
  type = krad_container_select (mount);

  if ((type != OGG) && (type != MKV) && (type != KUDP)) {
    return NULL;
  }
  
  container = calloc (1, sizeof(krad_container_t));

  container->type = type;

  if (container->type == OGG) {
    //container->ogg = krad_ogg_open_stream (host, port, mount, password);
    if (container->ogg == NULL) {
      free (container);
      return NULL;
    }
  }
  
  if (container->type == MKV) {
    container->mkv = kr_mkv_create_stream (host, port, mount, password);
    if (container->mkv == NULL) {
      free (container);
      return NULL;
    }
  }
  
  if (container->type == KUDP) {
    container->udp = kr_udp (host, port, mount, password);
    if (container->udp == NULL) {
      free (container);
      return NULL;
    }
  }  

  return container;
}

krad_container_t *krad_container_open_file (char *filename,
                                            krad_io_mode_t mode) {

  krad_container_t *container;
  
  container = calloc (1, sizeof(krad_container_t));

  container->type = krad_container_select (filename);

  if (container->type == OGG) {
    container->ogg = krad_ogg_open_file (filename, mode);
    if (container->ogg == NULL) {
      free (container);
      return NULL;
    }
  } else {
    if (container->type == MKV) {
      if (mode == KRAD_IO_WRITEONLY) {
        container->mkv = kr_mkv_create_file (filename);
      }
      if (mode == KRAD_IO_READONLY) {
        container->mkv = kr_mkv_open_file (filename);
      }
      if (container->mkv == NULL) {
        free (container);
        return NULL;
      }    
    } else {
      if ((container->type == NATIVEFLAC) || (container->type == Y4MFILE)) {
        if (mode == KRAD_IO_WRITEONLY) {
          container->raw = kr_file_create (filename);
        }
        if (mode == KRAD_IO_READONLY) {
          container->raw = kr_file_open (filename);
        }
        if (container->raw == NULL) {
          free (container);
          return NULL;
        } else {
          container->rawio = kr_io2_create ();
          kr_io2_set_fd (container->rawio, container->raw->fd);
        }
      }
    }
  }
  return container;
}

krad_container_t *
krad_container_open_transmission (krad_transmission_t *transmission) {

  failfast ("temp disabled transission");  
  return NULL;
/*

  krad_container_t *container;
  krad_container_type_t type;

  type = krad_container_select (transmission->sysname);

  if ((type != OGG) && (type != MKV)) {
    return NULL;
  }
  
  container = calloc(1, sizeof(krad_container_t));

  container->type = type;

  if (container->type == OGG) {
    container->ogg = krad_ogg_open_transmission (transmission);
  }

  if (container->type == MKV) {
    //container->mkv = kr_mkv_open_transmission (transmission);
  }

  return container;
  */
}

void krad_container_destroy (krad_container_t **container) {
  if ((container != NULL) && (*container != NULL)) {
    if ((*container)->type == OGG) {
      krad_ogg_destroy ((*container)->ogg);
    }
    if ((*container)->type == MKV) {
      kr_mkv_destroy (&(*container)->mkv);
    }
    if ((*container)->type == KUDP) {
      kr_udp_destroy (&(*container)->udp);
    }
    if (((*container)->type == NATIVEFLAC) || ((*container)->type == Y4MFILE)) {
      if ((*container)->raw) {
        kr_io2_destroy (&((*container)->rawio));
        kr_file_close (&((*container)->raw));
      }
    }
    free (*container);
    *container = NULL;
  }
}  

int krad_container_raw_add_data (krad_container_t *container,
                                 uint8_t *buffer,
                                 int len) {
  if ((container->type == NATIVEFLAC) || (container->type == Y4MFILE)) {
    kr_io2_pack (container->rawio, buffer, len);
    kr_io2_flush (container->rawio);
    return 0;
  } else {
    return -1;
  }
}

int krad_container_add_video_track_with_private_data (krad_container_t *container,
                                                      kr_codec_hdr_t *header,
                                                      int fps_numerator,
                                                      int fps_denominator,
                                                      int width, int height) {
/*
  if (container->type == OGG) {
    return krad_ogg_add_video_track_with_private_data (container->ogg,
                                                       header->codec,
                                                       fps_numerator,
                                                       fps_denominator,
                                                       width, height,
                                                       header->data[0],
                                                       header->sz[0],
                                                       header->count);
  } 

  if (container->type == MKV) {
    return kr_mkv_add_video_track_with_private_data (container->mkv, krad_codec_header->codec,
                                                     fps_numerator, fps_denominator, width, height,
                                                     krad_codec_header->header_combined,
                                                     krad_codec_header->header_combined_size);
  }
  */
  return -1;
}

int krad_container_add_video_track (krad_container_t *container,
                                    krad_codec_t codec, 
                                    int fps_numerator, int fps_denominator,
                                    int width, int height) {
                  
  if (container->type == OGG) {
    return krad_ogg_add_video_track (container->ogg, codec, fps_numerator,
                                     fps_denominator, width, height);
  } 

  if (container->type == MKV) {
    return kr_mkv_add_video_track (container->mkv, codec,
                                   fps_numerator, fps_denominator,
                                   width, height);
  }
  return -1;
}

int krad_container_add_audio_track (krad_container_t *container,
                                    krad_codec_t codec,
                                    int sample_rate, int channels, 
                                    krad_codec_header_t *krad_codec_header) {
/*
  if (container->type == OGG) {
    return krad_ogg_add_audio_track (container->ogg, codec,
                                     sample_rate, channels, 
                                     krad_codec_header->header,
                                     krad_codec_header->header_size,
                                     krad_codec_header->header_count);
  }
  
  if (container->type == MKV) {
    return kr_mkv_add_audio_track (container->mkv, codec,
                                   sample_rate, channels, 
                                   krad_codec_header->header_combined,
                                   krad_codec_header->header_combined_size);
  }
  */
  return -1;
}

void krad_container_add_video (krad_container_t *container, int track,
                               uint8_t *buffer, int buffer_size,
                               int keyframe) {

  if (container->type == OGG) {
    krad_ogg_add_video (container->ogg, track, buffer, buffer_size, keyframe);
  }
  if (container->type == MKV) {
    kr_mkv_add_video (container->mkv, track, buffer, buffer_size, keyframe);
  }
  if (container->type == KUDP) {
    krad_slicer_sendto (container->udp->slicer, buffer, buffer_size, 1, keyframe,
                        container->udp->host, container->udp->port);
  }
}

void krad_container_add_audio (krad_container_t *container, int track,
                               uint8_t *buffer, int buffer_size,
                               int frames) {
  if (container->type == OGG) {
    krad_ogg_add_audio (container->ogg, track, buffer, buffer_size, frames);
  }
  if (container->type == MKV) {
    kr_mkv_add_audio (container->mkv, track, buffer, buffer_size, frames);
  }
  if (container->type == KUDP) {
    krad_slicer_sendto (container->udp->slicer, buffer, buffer_size, 2, 0,
                        container->udp->host, container->udp->port);
  }
}

