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
    return RAW;
  }

  if (strstr(string, ".flac")) {
    return RAW;
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
                                         int track) {
  if (container->type == OGG) {
    return krad_ogg_track_codec ( container->ogg, track );
  } else {
    return kr_mkv_track_codec ( container->mkv, track );
  }
}

int krad_container_track_header_size (krad_container_t *container,
                                      int track, int header) {
  if (container->type == OGG) {
    return krad_ogg_track_header_size ( container->ogg, track, header );
  } else {
    return kr_mkv_track_header_size ( container->mkv, track, header );    
  }
}

int krad_container_read_track_header (krad_container_t *container,
                                      unsigned char *buffer,
                                      int track, int header) {
  if (container->type == OGG) {
    return krad_ogg_read_track_header ( container->ogg, buffer, track, header );
  } else {
    return kr_mkv_read_track_header ( container->mkv, buffer, track, header );    
  }
}

int krad_container_track_changed (krad_container_t *container, int track) {
  if (container->type == OGG) {
    return krad_ogg_track_changed ( container->ogg, track );
  } else {
    return kr_mkv_track_changed ( container->mkv, track );    
  }
}

int krad_container_track_active (krad_container_t *container, int track) {
  if (container->type == OGG) {
    return krad_ogg_track_active ( container->ogg, track );
  } else {
    return kr_mkv_track_active ( container->mkv, track );    
  }
}

int krad_container_track_header_count (krad_container_t *container, int track) {
  if (container->type == OGG) {
    return krad_ogg_track_header_count ( container->ogg, track );
  } else {
    return kr_mkv_track_header_count ( container->mkv, track );    
  }
}

int krad_container_read_packet (krad_container_t *container, int *track,
                                uint64_t *timecode, unsigned char *buffer) {
  if (container->type == OGG) {
    return krad_ogg_read_packet ( container->ogg, track, timecode, buffer );
  } else {
    return kr_mkv_read_packet ( container->mkv, track, timecode, buffer );
  }
}

krad_container_t *krad_container_open_stream (char *host, int port,
                                              char *mount, char *password) {

  krad_container_t *container;
  krad_container_type_t type;
  
  type = krad_container_select (mount);

  if (type == RAW) {
    return NULL;
  }
  
  container = calloc (1, sizeof(krad_container_t));

  container->type = type;

  if (container->type == OGG) {
    container->ogg = krad_ogg_open_stream (host, port, mount, password);
  }
  
  if (container->type == MKV) {
    container->mkv = kr_mkv_stream (host, port, mount, password);
  }

  return container;
}

kr_io2_t *kr_container_raw_create (char *filename) {
  
  kr_io2_t *io;
  int flags;
  int fd;
  
  flags = O_WRONLY | O_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  
  if (file_exists(filename)) {
    return NULL;
  }
  
  fd = open ( filename, flags );
  
  if (fd < 0) {
    return NULL;
  }
  
  io = kr_io2_create_size (65535);
  kr_io2_set_fd (io, fd);

  return io;
}

kr_io2_t *kr_container_raw_open (char *filename) {
  
  kr_io2_t *io;
  int flags;
  int fd;
  
  flags = O_RDONLY;
  
  if (!file_exists(filename)) {
    return NULL;
  }
  
  fd = open ( filename, flags );
  
  if (fd < 0) {
    return NULL;
  }
  
  io = kr_io2_create_size (65535);
  kr_io2_set_fd (io, fd);

  kr_io2_read (io);

  printk ("read %zu bytes", io->len);

  return io;
}

krad_container_t *krad_container_open_file (char *filename,
                                            krad_io_mode_t mode) {

  krad_container_t *container;
  
  container = calloc (1, sizeof(krad_container_t));

  container->type = krad_container_select (filename);

  if (container->type == OGG) {
    container->ogg = krad_ogg_open_file (filename, mode);
  }
  if (container->type == MKV) {
    if (mode == KRAD_IO_WRITEONLY) {
      container->mkv = kr_mkv_create_file (filename);
    }
    if (mode == KRAD_IO_READONLY) {
      container->mkv = kr_mkv_open_file (filename);
    }
  }
  if (container->type == RAW) {
    if (mode == KRAD_IO_WRITEONLY) {
      container->raw = kr_container_raw_create (filename);
    }
    if (mode == KRAD_IO_READONLY) {
      container->raw = kr_container_raw_open (filename);
    }
  }  

  return container;
}

krad_container_t *
krad_container_open_transmission (krad_transmission_t *transmission) {

  krad_container_t *container;
  krad_container_type_t type;
  
  type = krad_container_select (transmission->sysname);

  if (type == RAW) {
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
}

void krad_container_destroy (krad_container_t **container) {
  if ((container != NULL) && (*container != NULL)) {
    if ((*container)->type == OGG) {
      krad_ogg_destroy ((*container)->ogg);
    }
    if ((*container)->type == MKV) {
      kr_mkv_destroy (&(*container)->mkv);
    }
    if ((*container)->type == RAW) {
      if ((*container)->raw) {
        kr_io2_destroy (&((*container)->raw));
      }
    }
    free (*container);
    *container = NULL;
  }
}  

int krad_container_add_video_track_with_private_data (krad_container_t *container,
                                                      krad_codec_header_t *krad_codec_header,
                                                      int fps_numerator, int fps_denominator,
                                                      int width, int height) {
  if (container->type == RAW) {
    kr_io2_pack (container->raw,
                 krad_codec_header->header_combined,
                 krad_codec_header->header_combined_size);
  }

  if (container->type == OGG) {
    return krad_ogg_add_video_track_with_private_data (container->ogg,
                                                       krad_codec_header->codec,
                                                       fps_numerator, fps_denominator,
                                                       width, height, krad_codec_header->header,
                                                       krad_codec_header->header_size,
                                                       krad_codec_header->header_count);
  } else {
    return kr_mkv_add_video_track_with_private_data (container->mkv, krad_codec_header->codec,
                                                     fps_numerator, fps_denominator, width, height,
                                                     krad_codec_header->header_combined,
                                                     krad_codec_header->header_combined_size);
  }
}

int krad_container_add_video_track (krad_container_t *container,
                                    krad_codec_t codec, 
                                    int fps_numerator, int fps_denominator,
                                    int width, int height) {
                  
  if (container->type == OGG) {
    return krad_ogg_add_video_track (container->ogg, codec, fps_numerator,
                                     fps_denominator, width, height);
  } else {
    return kr_mkv_add_video_track (container->mkv, codec,
                                   fps_numerator, fps_denominator,
                                   width, height);
  }
}

int krad_container_add_audio_track (krad_container_t *container,
                                    krad_codec_t codec,
                                    int sample_rate, int channels, 
                                    krad_codec_header_t *krad_codec_header) {

  if (container->type == RAW) {
    kr_io2_pack (container->raw,
                 krad_codec_header->header_combined,
                 krad_codec_header->header_combined_size);
    kr_io2_flush (container->raw);
  }

  if (container->type == OGG) {
    return krad_ogg_add_audio_track (container->ogg, codec,
                                     sample_rate, channels, 
                                     krad_codec_header->header,
                                     krad_codec_header->header_size,
                                     krad_codec_header->header_count);
  } else {
    return kr_mkv_add_audio_track (container->mkv, codec,
                                   sample_rate, channels, 
                                   krad_codec_header->header_combined,
                                   krad_codec_header->header_combined_size);
  }
}

void krad_container_add_video (krad_container_t *container, int track,
                               unsigned char *buffer, int buffer_size,
                               int keyframe) {

  if (container->type == RAW) {
    kr_io2_pack (container->raw, buffer, buffer_size);
    kr_io2_flush (container->raw);
  }

  if (container->type == OGG) {
    krad_ogg_add_video (container->ogg, track, buffer, buffer_size, keyframe);
  } else {
    kr_mkv_add_video (container->mkv, track, buffer, buffer_size, keyframe);
  }
}

void krad_container_add_audio (krad_container_t *container, int track,
                               unsigned char *buffer, int buffer_size,
                               int frames) {

  if (container->type == RAW) {
    kr_io2_pack (container->raw, buffer, buffer_size);
    kr_io2_flush (container->raw);
  }

  if (container->type == OGG) {
    krad_ogg_add_audio (container->ogg, track, buffer, buffer_size, frames);
  } else {
    kr_mkv_add_audio (container->mkv, track, buffer, buffer_size, frames);
  }
}

