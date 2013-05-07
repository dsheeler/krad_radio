#include "krad_encoder.h"
#include "kr_client.h"

#include "krad_player_common.h"

typedef struct kr_encoder_msg_St kr_encoder_msg_t;

/*
static void kr_encoder_start (void *actual);
static int kr_encoder_process (void *msgin, void *actual);
static void kr_encoder_destroy_actual (void *actual);
*/

struct kr_encoder_msg_St {
  kr_encoder_cmd_t cmd;
  union {
    float real;
    int64_t integer;
  } param;
};

typedef union {
  krad_vhs_t *kvhs;
	krad_vpx_encoder_t *vpx;
  krad_theora_encoder_t *theora;
  krad_flac_t *flac;
  krad_opus_t *opus;
  krad_vorbis_t *vorbis;
} kr_encoder_codec_state;

struct kr_encoder2_St {
  kr_encoder_state_t state;
  kr_machine_t *machine;  
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
      //krad_vhs_encoder_destroy (encoder->enc.kvhs);
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
      //encoder->enc.kvhs = krad_vhs_create_encoder ();
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
      //kr_vhs_encode (encoder->enc.kvhs, codeme, medium);
      break;
    case VP8:
      kr_vpx_encode (encoder->enc.vpx, codeme, medium);
      break;
    case THEORA:
      //kr_theora_encode (encoder->enc.theora, codeme, medium);
      break;
    default:
    return -1;
  }

  return 0;
}

/* Private Functions */
/*
static int kr_encoder_process (void *msgin, void *actual) {

  kr_encoder2_t *encoder;
  kr_encoder_msg_t *msg;

  msg = (kr_encoder_msg_t *)msgin;
  encoder = (kr_encoder2_t *)actual;

  printf ("kr_encoder_process cmd %p\n", encoder);

  //printf ("kr_encoder_process cmd %d\n", msg->cmd);

  switch (msg->cmd) {
    case DOENCODE:
      printf ("Got ENCODE command!\n");
      break;
    case ENCODERDESTROY:
      printf ("Got ENCODERDESTROY command!\n");
      return 0;
  }
  
  //printf ("kr_encoder_process done\n");  
  
  return 1;
}

static void kr_encoder_destroy_actual (void *actual) {

  kr_encoder2_t *encoder;

  encoder = (kr_encoder2_t *)actual;

  printf ("kr_encoder_destroy_actual cmd %p\n", encoder);
}

static void kr_encoder_start (void *actual) {

  kr_encoder2_t *encoder;

  encoder = (kr_encoder2_t *)actual;

  encoder->state = ENIDLE;
  printf ("kr_encoder_start()!\n");
}
*/
/* Public Functions */
/*
void kr_encoder_destroy (kr_encoder_t **encoder) {
//  kr_encoder_msg_t msg;
  if ((encoder != NULL) && (*encoder != NULL)) {
    printf ("kr_encoder_destroy()!\n");
//   msg.cmd = ENCODERDESTROY;
//    krad_machine_msg ((*encoder)->machine, &msg);
//    krad_machine_destroy (&(*encoder)->machine);
    free (*encoder);
    *encoder = NULL;
  }
}

kr_encoder_t *kr_encoder_create () {
  
  kr_encoder_t *encoder;
  //kr_machine_params_t machine_params;

  encoder = calloc (1, sizeof(kr_encoder2_t));

  encoder->codec = NOCODEC;

  machine_params.actual = encoder;
  machine_params.msg_sz = sizeof (kr_encoder_msg_t);
  machine_params.start = kr_encoder_start;
  machine_params.process = kr_encoder_process;
  machine_params.destroy = kr_encoder_destroy_actual;

  encoder->machine = krad_machine_create (&machine_params);

  return encoder;
};

kr_encoder_state_t kr_encoder_state_get (kr_encoder2_t *encoder) {
  return encoder->state;
}
*/

void kr_encoder_destroy_direct (kr_encoder2_t **encoder) {
  if ((encoder != NULL) && (*encoder != NULL)) {
    kr_encoder_destroy_instance_encoder (*encoder);
    free (*encoder);
    *encoder = NULL;
  }
}

kr_encoder2_t *kr_encoder_create_direct () {
  kr_encoder2_t *encoder;
  encoder = calloc (1, sizeof(kr_encoder2_t));
  encoder->codec = NOCODEC;
  return encoder;
};
