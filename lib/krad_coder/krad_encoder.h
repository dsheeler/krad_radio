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

#include "krad_ring.h"
#include "krad_resample_ring.h"

#include "krad_theora.h"
#include "krad_vpx.h"

#include "krad_vorbis.h"
#include "krad_flac.h"
#include "krad_opus.h"

#ifndef KRAD_ENCODER_H
#define KRAD_ENCODER_H

typedef struct kr_encoder2_St kr_encoder2_t;

int kr_encoder2_encode_direct (kr_encoder2_t *encoder,
                               kr_medium_t *medium,
                               kr_codeme_t *codeme);

kr_encoder2_t *kr_encoder_create_direct ();
int kr_encoder_destroy_direct (kr_encoder2_t **encoder);


#endif
