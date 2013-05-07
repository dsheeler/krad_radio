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

#include <vpx/vpx_encoder.h>
#include <vpx/vp8cx.h>
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>

#include "krad_system.h"

#include "krad_coder_common.h"

typedef struct krad_vpx_encoder_St krad_vpx_encoder_t;
typedef struct krad_vpx_decoder_St krad_vpx_decoder_t;

struct krad_vpx_encoder_St {
  int width;
  int height;
  
  int fps_numerator;
  int fps_denominator;

  int bitrate;
  int min_quantizer;
  int max_quantizer;

  int update_config;
  vpx_codec_ctx_t encoder;
  vpx_codec_enc_cfg_t cfg;
  vpx_image_t *image;
  vpx_codec_err_t  res;
  vpx_codec_iter_t iter;
  const vpx_codec_cx_pkt_t *pkt;

  int flags;
  uint64_t bytes;
  unsigned long frames;
  unsigned int frames_since_keyframe;  
  unsigned long deadline;
};

struct krad_vpx_decoder_St {
  int width;
  int height;

  int flags;
  int frames;
  int dec_flags;

  vpx_codec_err_t  res;
  vpx_codec_ctx_t decoder;
  vp8_postproc_cfg_t ppcfg;
  vpx_codec_dec_cfg_t cfg;
  vpx_codec_stream_info_t stream_info;
  vpx_codec_iter_t iter;
  vpx_image_t *img;

  uint8_t *buf;
  unsigned char compressed_video_buffer[800000];
  unsigned char *frame_data;
};

void krad_vpx_encoder_set_kf_max_dist (krad_vpx_encoder_t *vpx,
                                       uint32_t max_dist);

int krad_vpx_encoder_min_quantizer_get (krad_vpx_encoder_t *vpx);
void krad_vpx_encoder_min_quantizer_set (krad_vpx_encoder_t *vpx,
                                         int min_quantizer);
int krad_vpx_encoder_max_quantizer_get (krad_vpx_encoder_t *vpx);
void krad_vpx_encoder_max_quantizer_set (krad_vpx_encoder_t *vpx,
                                         int max_quantizer);
int krad_vpx_encoder_bitrate_get (krad_vpx_encoder_t *vpx);
void krad_vpx_encoder_bitrate_set (krad_vpx_encoder_t *vpx,
                                   int bitrate);
void krad_vpx_encoder_deadline_set (krad_vpx_encoder_t *vpx,
                                    int deadline);
int krad_vpx_encoder_deadline_get (krad_vpx_encoder_t *vpx);

void krad_vpx_encoder_print_config (krad_vpx_encoder_t *vpx);

void krad_vpx_encoder_finish (krad_vpx_encoder_t *vpx);
void krad_vpx_encoder_config_set (krad_vpx_encoder_t *vpx,
                                  vpx_codec_enc_cfg_t *cfg);
krad_vpx_encoder_t *krad_vpx_encoder_create (int width, int height,
                                             int fps_numerator,
                                             int fps_denominator,
                                             int bitrate);
int32_t krad_vpx_encoder_destroy (krad_vpx_encoder_t **vpx);

int krad_vpx_encoder_write (krad_vpx_encoder_t *vpx,
                            unsigned char **packet,
                            int *keyframe);
void krad_vpx_encoder_want_keyframe (krad_vpx_encoder_t *vpx);

int32_t kr_vpx_encode (krad_vpx_encoder_t *vpx,
                       kr_codeme_t *codeme,
                       kr_medium_t *medium);

int32_t kr_vpx_decode (krad_vpx_decoder_t *vpx,
                       kr_medium_t *medium,
                       kr_codeme_t *codeme);

krad_vpx_decoder_t *krad_vpx_decoder_create ();
int32_t krad_vpx_decoder_destroy (krad_vpx_decoder_t **vpx);
void krad_vpx_decoder_decode (krad_vpx_decoder_t *vpx,
                              void *buffer,
                              int len);
                              
void krad_vpx_decoder_decode_again (krad_vpx_decoder_t *vpx);
