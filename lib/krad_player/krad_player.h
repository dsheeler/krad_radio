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

#include "krad_player_common.h"

#include "krad_demuxer.h"
#include "krad_decoder.h"

#include "krad_ring.h"
#include "krad_resample_ring.h"

#ifndef KRAD_PLAYER_H
#define KRAD_PLAYER_H

typedef enum {
  REVERSE,
  FORWARD
} kr_direction_t;

typedef enum {
  IDLE,
  CUED,
  PLAYING,
  PLAYERDESTROYING
//  PAUSING,
//  RESUMING,
//  LOOPING
} kr_player_playback_state_t;

typedef enum {
  PLAY,
  PAUSE,
//  STOP,
  SEEK,
  SETSPEED,
  SETDIR,
//  TICKLE,
  PLAYERDESTROY
} kr_player_cmd_t;

typedef struct kr_player_St kr_player_t;

char *kr_player_playback_state_to_string (kr_player_playback_state_t state);
char *kr_direction_to_string (kr_direction_t direction);

void kr_player_destroy (kr_player_t **player);
kr_player_t *kr_player_create(char *station, char *url);
kr_player_t *kr_player_create_custom_cb(char *url);

float kr_player_speed_get (kr_player_t *player);
void kr_player_speed_set (kr_player_t *player, float speed);
kr_direction_t kr_player_direction_get (kr_player_t *player);
void kr_player_direction_set (kr_player_t *player, kr_direction_t direction);
int64_t kr_player_position_get (kr_player_t *player);
kr_player_playback_state_t kr_player_playback_state_get (kr_player_t *player);

void kr_player_seek (kr_player_t *player, int64_t position);
void kr_player_play (kr_player_t *player);
void kr_player_pause (kr_player_t *player);
void kr_player_stop (kr_player_t *player);

#endif
