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

#include "krad_player_common.h"

#include "krad_system.h"
#include "krad_container.h"
#include "krad_machine.h"

#include "krad_ring.h"
#include "krad_resample_ring.h"

#ifndef KRAD_DEMUXER_H
#define KRAD_DEMUXER_H

typedef enum {
  REV,
  FWD
} kr_ddir_t;

typedef enum {
  DMIDLE,
  DMCUED,
  DEMUXING
} kr_demuxer_state_t;

typedef enum {
  SETDMDIR,
  ROLL,
  DMSEEK,
  DMPAUSE,
  DEMUXERDESTROY
} kr_demuxer_cmd_t;

typedef struct kr_demuxer_St kr_demuxer_t;
typedef struct kr_demuxer_params_St kr_demuxer_params_t;

struct kr_demuxer_params_St {
  void *controller;
  char *url;
  //size_t msg_sz;
  void (*status_cb)(kr_demuxer_state_t, void *);
  int (*codeme_cb)(kr_codeme_t *, void *);  
};

void kr_demuxer_destroy (kr_demuxer_t **demuxer);
kr_demuxer_t *kr_demuxer_create (kr_demuxer_params_t *demuxer_params);

kr_ddir_t kr_demuxer_direction_get (kr_demuxer_t *demuxer);
void kr_demuxer_direction_set (kr_demuxer_t *demuxer, kr_ddir_t);
int64_t kr_demuxer_position_get (kr_demuxer_t *demuxer);
kr_demuxer_state_t kr_demuxer_state_get (kr_demuxer_t *demuxer);

void kr_demuxer_roll (kr_demuxer_t *demuxer);
void kr_demuxer_seek (kr_demuxer_t *demuxer, int64_t position);
void kr_demuxer_pause (kr_demuxer_t *demuxer);

#endif
