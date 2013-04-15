#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>

#include "krad_system.h"
#include "krad_container.h"
#include "krad_machine.h"

#include "krad_ring.h"
#include "krad_resample_ring.h"

#include "krad_vorbis.h"
#include "krad_flac.h"
#include "krad_opus.h"

#ifndef KRAD_DECODER_H
#define KRAD_DECODER_H

typedef enum {
  DEIDLE,
  DECODING
} kr_decoder_state_t;

typedef enum {
  DODECODE,
  DECODERDESTROY
} kr_decoder_cmd_t;

typedef struct kr_decoder_St kr_decoder_t;

void kr_decoder_destroy (kr_decoder_t **decoder);
kr_decoder_t *kr_decoder_create ();

kr_decoder_state_t kr_decoder_state_get (kr_decoder_t *decoder);

/*
float kr_decoder_speed_get (kr_decoder_t *decoder);
void kr_decoder_speed_set (kr_decoder_t *decoder, float speed);

int64_t kr_decoder_position_get (kr_decoder_t *decoder);



void kr_decoder_seek (kr_decoder_t *decoder, int64_t position);
void kr_decoder_play (kr_decoder_t *decoder);
void kr_decoder_pause (kr_decoder_t *decoder);
void kr_decoder_stop (kr_decoder_t *decoder);
*/
#endif
