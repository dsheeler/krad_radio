#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>

#include <vorbis/vorbisenc.h>

#include "krad_radio_version.h"
#include "krad_system.h"
#include "krad_ring.h"
#include "krad_codec_header.h"

#include "krad_player_common.h"

typedef struct krad_vorbis_St krad_vorbis_t;

struct krad_vorbis_St {
  uint32_t channels;
  uint32_t sample_rate;

  float quality;

  uint64_t frames;

  vorbis_dsp_state vdsp;
  vorbis_block vblock;
  vorbis_info vinfo;
  vorbis_comment vc;

  int32_t small_blocksz;
  int32_t large_blocksz;

  krad_codec_header_t header;

  uint8_t hdrdata[8192];

  char state_str_custom[256];
  char *state_str;
  int error;
};

int krad_vorbis_encoder_finish (krad_vorbis_t *vorbis);

int32_t krad_vorbis_encoder_destroy (krad_vorbis_t **vorbis);

krad_vorbis_t *krad_vorbis_encoder_create (int channels,
                                           int sample_rate,
                                           float quality);

int32_t kr_vorbis_encode (krad_vorbis_t *vorbis,
                          kr_codeme_t *codeme,
                          kr_medium_t *medium);

int krad_vorbis_test_headers (krad_codec_header_t *hdr);

int32_t krad_vorbis_decoder_destroy (krad_vorbis_t **vorbis);

krad_vorbis_t *krad_vorbis_decoder_create (kr_codec_hdr_t *header);

int32_t kr_vorbis_decode (krad_vorbis_t *vorbis,
                          kr_medium_t *medium,
                          kr_codeme_t *codeme);
