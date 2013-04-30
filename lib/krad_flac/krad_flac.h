#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include <FLAC/stream_encoder.h>
#include <FLAC/stream_decoder.h>

#include "krad_radio_version.h"
#include "krad_system.h"
#include "krad_codec_header.h"

#ifndef KRAD_FLAC_H
#define KRAD_FLAC_H

#define KRAD_FLAC_MINIMAL_HEADER_SIZE 42
#define KRAD_FLAC_STREAMINFO_BLOCK_SIZE 38
#define KRAD_FLAC_MARKER "fLaC"
#define KRAD_DEFAULT_FLAC_FRAME_SIZE 4096

typedef struct krad_flac_St krad_flac_t;

struct krad_flac_St {

  FLAC__StreamEncoder *encoder;
  FLAC__StreamDecoder *decoder;
  
  int sample_rate;
  int bit_depth;
  int channels;

  int test_fd;

  int finished;

  unsigned char min_header[42];
  unsigned char streaminfo_block[38];
  
  unsigned char header[256];
  int header_size;
  
  uint8_t *comment_header;
  
  int have_min_header;
  int streaminfo_rewrite;
  krad_codec_header_t krad_codec_header;

  unsigned long long total_frames_input;
  unsigned long long total_frames;
  unsigned long long total_bytes;
  
  int bytes;
  uint8_t *encode_buffer;
  int int32_samples[8192 * 4];
  
  uint8_t *decode_buffer;
  int decode_buffer_len;
  int decode_buffer_pos;
  
  int frames;
  
  float **output_buffer;
};

/* Encoding */

int krad_flac_encode (krad_flac_t *flac,
                      float *audio, int frames,
                      uint8_t *encode_buffer);
int krad_flac_encoder_frames_remaining (krad_flac_t *flac);
int krad_flac_encoder_read_header (krad_flac_t *flac,
                                   uint8_t *buffer);
int krad_flac_encoder_read_min_header (krad_flac_t *flac,
                                       uint8_t *buffer);
int krad_flac_encoder_finish (krad_flac_t *flac,
                              uint8_t *encode_buffer);
void krad_flac_encoder_destroy (krad_flac_t *flac);
krad_flac_t *krad_flac_encoder_create (int channels,
                                       int sample_rate,
                                       int bit_depth);

/* Decoding */

int krad_flac_decoder_test (uint8_t *header, int len);

int krad_flac_decode (krad_flac_t *flac,
                      uint8_t *buffer, int len,
                      float **audio);
krad_flac_t *krad_flac_decoder_create (krad_codec_header_t *header);
void krad_flac_decoder_destroy (krad_flac_t *flac);

#endif
