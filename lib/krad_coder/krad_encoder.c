#include "krad_encoder.h"

typedef union {
  krad_vpx_encoder_t *vpx;
  krad_theora_encoder_t *theora;
  krad_flac_t *flac;
  krad_opus_t *opus;
  krad_vorbis_t *vorbis;
} kr_encoder_codec_state;

struct kr_encoder2_St {
  kr_encoder_codec_state enc;
  krad_codec_t codec;
};

void kr_encoder_destroy_instance_encoder (kr_encoder2_t *encoder) {

  switch (encoder->codec) {
    case OPUS:
      //krad_opus_encoder_destroy (encoder->enc.opus);
      break;
    case FLAC:
      //krad_flac_encoder_destroy (encoder->enc.flac);
      break;
    case VORBIS:
      krad_vorbis_encoder_destroy (&encoder->enc.vorbis);
      break;
    case KVHS:
      break;
    case VP8:
      krad_vpx_encoder_destroy (&encoder->enc.vpx);
      break;
    case THEORA:
      //krad_theora_encoder_destroy (encoder->enc.theora);
      break;
    default:
      break;
  }
  encoder->codec = NOCODEC;
}

void kr_encoder_create_instance_encoder (kr_encoder2_t *encoder) {

  encoder->codec = VORBIS;

  switch (encoder->codec) {
    case OPUS:
      //encoder->enc.opus = krad_opus_encoder_create (header);
      break;
    case FLAC:
      //encoder->enc.flac = krad_flac_encoder_create (header);
      break;
    case VORBIS:
      encoder->enc.vorbis = krad_vorbis_encoder_create (2, 44100, 0.5);
      break;
    case KVHS:
      break;
    case VP8:
      encoder->enc.vpx = krad_vpx_encoder_create (640, 360, 30, 1, 1000);
      break;
    case THEORA:
      //encoder->enc.theora = krad_theora_encoder_create (header);
      break;
    default:
      return;
  }
}

static int kr_encoder_check (kr_encoder2_t *encoder,
                             kr_codeme_t *codeme,
                             kr_medium_t *medium) {

  if (codeme->codec != encoder->codec) {
    if (encoder->codec != NOCODEC) {
      kr_encoder_destroy_instance_encoder (encoder);
    }
    if (codeme->codec != NOCODEC) {
      kr_encoder_create_instance_encoder (encoder);
    } else {
      return -1;
    }
  }

  return 0;
}

int kr_encoder_encode_direct (kr_encoder2_t *encoder,
                              kr_medium_t *medium,
                              kr_codeme_t *codeme) {
  int ret;

  ret = kr_encoder_check (encoder, codeme, medium);
  if (ret < 0) {
    return ret;
  }

  switch (codeme->codec) {
    case OPUS:
      //kr_opus_encode (encoder->enc.opus, codeme, medium);
      //krad_opus_encoder_write (encoder->enc.opus, kr_slice->data, kr_slice->size);
      //bytes = krad_opus_encoder_read (krad_link->krad_opus, c + 1, (char *)krad_link->au_audio, 120 * 4);
      break;
    case FLAC:
      //kr_flac_encode (encoder->enc.flac, codeme, medium);
      //len = krad_flac_encode (encoder->enc.flac, kr_slice->data, kr_slice->size, krad_link->au_samples);
      //krad_resample_ring_write (krad_link->krad_resample_ring[c], (unsigned char *)krad_link->au_samples[c], len * 4);
      break;
    case VORBIS:
      kr_vorbis_encode (encoder->enc.vorbis, codeme, medium);
      break;
    case KVHS:
      break;
    case VP8:
      //kr_vpx_encode (encoder->enc.vpx, codeme, medium);
      break;
    case THEORA:
      //kr_theora_encode (encoder->enc.theora, codeme, medium);
      break;
    default:
    return -1;
  }

  return 0;
}

int kr_encoder_destroy_direct (kr_encoder2_t **encoder) {
  if ((encoder != NULL) && (*encoder != NULL)) {
    kr_encoder_destroy_instance_encoder (*encoder);
    free (*encoder);
    *encoder = NULL;
    return 0;
  }
  return -1;
}

kr_encoder2_t *kr_encoder_create_direct () {
  kr_encoder2_t *encoder;
  encoder = calloc (1, sizeof(kr_encoder2_t));
  encoder->codec = NOCODEC;
  return encoder;
};
