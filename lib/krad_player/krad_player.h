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

#include "kradimer.h"

#ifndef KRAD_PLAYER_H
#define KRAD_PLAYER_H

typedef enum {
  REVERSE,
  FORWARD
} kr_direction;

typedef enum {
  IDLE,
  CUED,
  PLAYING,
  PLAYERDESTROYING
//  PAUSING,
//  RESUMING,
//  LOOPING
} kr_player_playback_state;

typedef enum {
  PLAY,
  PAUSE,
//  STOP,
  SEEK,
  SETSPEED,
  SETDIR,
//  TICKLE,
  PLAYERDESTROY
} kr_player_cmd;

typedef struct kr_player kr_player;

char *kr_player_playback_stateo_string(kr_player_playback_state state);
char *kr_directiono_string(kr_direction direction);

void kr_player_destroy(kr_player **player);
kr_player *kr_player_create(char *station, char *url);
kr_player *kr_player_create_custom_cb(char *url);

float kr_player_speed_get(kr_player *player);
void kr_player_speed_set(kr_player *player, float speed);
kr_direction kr_player_direction_get(kr_player *player);
void kr_player_direction_set(kr_player *player, kr_direction direction);
int64 kr_player_position_get(kr_player *player);
kr_player_playback_state kr_player_playback_state_get(kr_player *player);

void kr_player_seek(kr_player *player, int64 position);
void kr_player_play(kr_player *player);
void kr_player_pause(kr_player *player);
void kr_player_stop(kr_player *player);


int krad_player_get_frame(kr_player *player, void *frame);


#endif
