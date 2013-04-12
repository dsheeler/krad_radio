#include "krad_player.h"

#define CTRLMP 0
#define VIDEOMP 1
#define AUDIOMP 2

typedef struct kr_player_msg_St kr_player_msg_t;

struct kr_player_msg_St {
  kr_player_cmd_t cmd;
  union {
    float real;
    int64_t integer;
  } param;
};

struct kr_player_St {
  pthread_t thread;
  kr_msgsys_t *msgsys;
  char *url;
  float speed;
  int64_t position;
  kr_player_state_t state;
  kr_direction_t direction;
  krad_container_t *input;
};

/* Private Functions */

static void *kr_player_thread (void *arg) {

  kr_player_t *player;
  kr_player_msg_t msg;
  int running;
  int ret;

  running = 1;
  player = (kr_player_t *)arg;


  /* open url
     open a/v ports?
     state in sense of active tracks,
     and auto decode video or audio or only?
  */
  
  player->input = krad_container_open_file (player->url,
                                            KRAD_IO_READONLY);

  if (player->input == NULL) {
    player->state = IDLE;
  } else {
    player->state = CUED;
    printf ("\nGot Track Count: %d\n",
            krad_container_track_count (player->input));
  }
  
  while (running) {
    /* printf ("Cycle!\n"); */
    ret = kr_msgsys_wait (player->msgsys, &msg);
    if (ret == 0) {
      break;
    }
    switch (msg.cmd) {
      case TICKLE:
        printf ("\nGot TICKLE command!\n");
        break;
      case DESTROY:
        printf ("\nGot DESTROY command!\n");
        running = 0;
        break;
      case PAUSE:
        printf ("\nGot PAUSE command!\n");
        if (player->input != NULL) {
          player->state = CUED;
        }
        break;
      case STOP:
        printf ("\nGot STOP command!\n");
        if (player->input != NULL) {
          player->state = CUED;
        }
        break;
      case PLAY:
        printf ("\nGot PLAY command!\n");
        if (player->input != NULL) {
          player->state = PLAYING;
        }
        break;
      case SEEK:
        printf ("\nGot SEEK %"PRIi64" command!\n", msg.param.integer);
        break;
      case SETSPEED:
        printf ("\nGot SETSPEED %0.3f command!\n", msg.param.real);
        player->speed = msg.param.real;
        break;
      case SETDIR:
        printf ("\nGot SETDIR %"PRIi64" command!\n", msg.param.integer);
        player->direction = msg.param.integer;
        break;    
    }
  }
  
  krad_container_destroy (&player->input);  

  return NULL;
}

/* Public Functions */

kr_player_t *kr_player_create (char *url) {

  kr_player_t *player;
  
  player = calloc (1, sizeof(kr_player_t));

  player->direction = FORWARD;
  player->speed = 100.0f;
  player->state = IDLE;
  player->url = strdup (url);
  player->msgsys = kr_msgsys_create (3, sizeof(kr_player_msg_t));
  
  if (pthread_create (&player->thread, NULL,
                      kr_player_thread, (void *)player)) {
    fprintf (stderr, "Can't Start thread\n");
    free (player);
    return NULL;
  }
  printf ("kr_player_create()!\n");
  return player;
}

void kr_player_destroy (kr_player_t **player) {
  kr_player_msg_t msg;
  if ((player != NULL) && (*player != NULL)) {
    msg.cmd = DESTROY;
    kr_msgsys_write ((*player)->msgsys, CTRLMP, &msg);
    pthread_join ((*player)->thread, NULL);
    kr_msgsys_destroy (&(*player)->msgsys);
    free ((*player)->url);
    free (*player);
    *player = NULL;
    printf ("kr_player_destroy()!\n");    
  }
}

char *kr_player_state_to_string (kr_player_state_t state) {
  switch (state) {
    case IDLE:
      return "IDLE";
    case CUED:
      return "CUED";
    case PLAYING:
      return "PLAYING";
    case PAUSING:
      return "PAUSING";
    case RESUMING:
      return "RESUMING";
    case LOOPING:
      return "LOOPING";
  }
  return "Unknown";
}

char *kr_direction_to_string (kr_direction_t direction) {
  switch (direction) {
    case FORWARD:
      return "Forward";
    case REVERSE:
      return "Reverse";
  }
  return "Unknown";
}

float kr_player_speed_get (kr_player_t *player) {
  return player->speed;
}

kr_direction_t kr_player_direction_get (kr_player_t *player) {
  return player->direction;
}

int64_t kr_player_position_get (kr_player_t *player) {
  return player->position;
}

kr_player_state_t kr_player_state_get (kr_player_t *player) {
  return player->state;
}

void kr_player_speed_set (kr_player_t *player, float speed) {
  kr_player_msg_t msg;
  msg.cmd = SETSPEED;
  msg.param.real = roundf (speed*1000.0f)/1000.0f;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_direction_set (kr_player_t *player, kr_direction_t direction) {
  kr_player_msg_t msg;
  msg.cmd = SETDIR;
  msg.param.integer = direction;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_seek (kr_player_t *player, int64_t position) {
  kr_player_msg_t msg;
  msg.cmd = SEEK;
  msg.param.integer = position;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_play (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.cmd = PLAY;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_pause (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.cmd = PAUSE;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_stop (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.cmd = STOP;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}
