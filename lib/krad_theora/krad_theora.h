#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>

#ifndef KRAD_THEORA_H
#define KRAD_THEORA_H

#include <theora/theoradec.h>
#include <theora/theoraenc.h>

typedef struct krad_theora_encoder_St krad_theora_encoder_t;
typedef struct krad_theora_decoder_St krad_theora_decoder_t;

#include <libswscale/swscale.h>

#include "krad_system.h"
#include "krad_codec_header.h"

struct krad_theora_encoder_St {

  int width;
  int height;
  int quality;
  
  int color_depth;
  
  int update_config;  
  
  uint64_t frames;
  uint64_t bytes;

  int finish;

  th_enc_ctx *encoder;
  
  unsigned char *header[10];
  int header_len[10];
  int header_count;

  unsigned char *header_combined;
  int header_combined_size;
  int header_combined_pos;
  int demented;
  
  krad_codec_header_t krad_codec_header;  
  
  th_info  info;
  th_comment comment;
  ogg_packet packet;
  th_ycbcr_buffer ycbcr;
  
  int speed;
  unsigned int keyframe_distance;
  
  int keyframe_shift;
};

struct krad_theora_decoder_St {

  int width;
  int height;

  int color_depth;

  int flags;
  int frames;
  int quality;
  int dec_flags;

  int offset_y;
  int offset_x;

  th_info  info;
  th_comment comment;
  th_setup_info *setup_info;
  th_dec_ctx *decoder;
  th_ycbcr_buffer ycbcr;
  ogg_packet packet;
  ogg_int64_t granulepos;
  
  uint8_t *buf;
  unsigned char compressed_video_buffer[800000];

  unsigned char *frame_data;
};

/* encoder */

int krad_theora_encoder_quality_get (krad_theora_encoder_t *krad_theora);
void krad_theora_encoder_quality_set (krad_theora_encoder_t *krad_theora,
                                      int quality);
int krad_theora_encoder_write (krad_theora_encoder_t *krad_theora,
                               unsigned char **packet,
                               int *keyframe);
krad_theora_encoder_t *krad_theora_encoder_create (int width, int height,
                                                   int fps_numerator,
                                                   int fps_denominator,
                                                   int color_depth,
                                                   int quality);
void krad_theora_encoder_destroy (krad_theora_encoder_t *krad_theora);

/* decoder */

int krad_theora_test_headers (krad_codec_header_t *hdr);

void krad_theora_decoder_timecode (krad_theora_decoder_t *krad_theora,
                                   uint64_t *timecode);
void krad_theora_decoder_write (krad_theora_decoder_t *krad_theora);
void krad_theora_decoder_decode (krad_theora_decoder_t *krad_theora,
                                 void *buffer, int len);
krad_theora_decoder_t *
krad_theora_decoder_create (unsigned char *header1, int header1len,
                            unsigned char *header2, int header2len,
                            unsigned char *header3, int header3len);
void krad_theora_decoder_destroy (krad_theora_decoder_t *krad_theora);

#endif
