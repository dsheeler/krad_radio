#ifndef KRAD_CONTAINER_H
#define KRAD_CONTAINER_H

typedef struct krad_container_St krad_container_t;

#include "krad_mkv.h"
#include "krad_mkv_demux.h"
#include "krad_ogg.h"
#include "krad_transmitter.h"
#include "krad_file.h"
#include "krad_udp.h"

struct krad_container_St {
  krad_container_type_t type;
  krad_ogg_t *ogg;
  kr_mkv_t *mkv;
  krad_udp_t *udp;  
  kr_io2_t *raw;
  krad_transmission_t *transmission;
};

/* Create / Destroy */
void krad_container_destroy (krad_container_t **krad_container);
krad_container_t *krad_container_open_stream (char *host, int port,
                                              char *mount, char *password);
krad_container_t *krad_container_open_file (char *filename,
                                            krad_io_mode_t mode);
krad_container_t *
krad_container_open_transmission (krad_transmission_t *transmission);

/* Input */
int krad_container_track_count (krad_container_t *krad_container);
krad_codec_t krad_container_track_codec (krad_container_t *container,
                                         int track);
int krad_container_track_header_count (krad_container_t *container,
                                       int track);
int krad_container_track_header_size (krad_container_t *container,
                                      int track,
                                      int header);
int krad_container_read_track_header (krad_container_t *container,
                                      unsigned char *buffer,
                                      int track, int header);
int krad_container_track_active (krad_container_t *container, int track);
int krad_container_track_changed (krad_container_t *container, int track);
int krad_container_read_packet (krad_container_t *container, int *track,
                                uint64_t *timecode, unsigned char *buffer);


/* Output */
char *krad_container_select_mimetype (char *string);

int krad_container_raw_add_data (krad_container_t *container,
                                 unsigned char *buffer,
                                 int len);
int krad_container_add_video_track_with_private_data (krad_container_t *container,
                                                      krad_codec_header_t *krad_codec_header,
                                                      int fps_numerator, int fps_denominator,
                                                      int width, int height);
int krad_container_add_video_track (krad_container_t *container,
                                    krad_codec_t codec,
                                    int fps_numerator, int fps_denominator,
                                    int width, int height);
int krad_container_add_audio_track (krad_container_t *container,
                                    krad_codec_t codec,
                                    int sample_rate, int channels,
                                    krad_codec_header_t *krad_codec_header);
void krad_container_add_video (krad_container_t *container, int track,
                               unsigned char *buffer, int buffer_size,
                               int keyframe);
void krad_container_add_audio (krad_container_t *container, int track,
                               unsigned char *buffer, int buffer_size, 
                               int frames);

#endif
