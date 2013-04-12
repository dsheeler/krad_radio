#include "kr_player.h"

typedef struct kr_player_msg_St kr_player_msg_t;

struct kr_player_msg_St {
  kr_player_cmd_t cmd;
  union {
    float real;
    int64_t integer;
  } param;
};

struct kr_player_track_St {
  int yay;
};

struct kr_player_St {
  kr_playback_state_t state;
  kr_direction_t direction;
  kr_player_track_t *tracks;
  float speed;
  int64_t position;
  pthread_t thread;
  int fd[2];
};

/* Private Functions */

static kr_player_msg_t *kr_player_alloc_msg (kr_player_t *player) {
  //printf ("Msg Alloc!\n");
  return calloc (1, sizeof(kr_player_msg_t));
}

static void kr_player_free_msg (kr_player_t *player, kr_player_msg_t **msg) {
  if ((msg != NULL) && (*msg != NULL)) {
    free (*msg);
    *msg = NULL;
    //printf ("Msg Free!\n");
  }
}

static int kr_player_msg_send (kr_player_t *player, kr_player_msg_t *msgin) {
  
  int ret;
  kr_player_msg_t *msg;
  
  msg = kr_player_alloc_msg (player);
  
  memcpy (msg, msgin, sizeof(kr_player_msg_t));
        
  ret = write (player->fd[0], &msg, sizeof(kr_player_msg_t *));    

  if (ret != sizeof(kr_player_msg_t *)) {
    fprintf (stderr, "Oh dear! write %d", ret);
    exit (1);
  }
  //printf ("Msg send!\n");  
  return ret;
}

static int kr_player_msg_get (kr_player_t *player, kr_player_msg_t *msgout) {

  int ret;
  kr_player_msg_t *msg;

  ret = read (player->fd[1], &msg, sizeof(kr_player_msg_t *));    

  if (ret != sizeof(kr_player_msg_t *)) {
    fprintf (stderr, "Oh dear! read %d", ret);
    exit (1);
  }

  memcpy (msgout, msg, sizeof(kr_player_msg_t));

  kr_player_free_msg (player, &msg);
  //printf ("Msg Get!\n");
  return ret;
}

static int kr_player_msg_poll (kr_player_t *player, kr_player_msg_t *msg) {

  struct pollfd pollfds[2];
  int n;

  n = 0;
  pollfds[n].fd = player->fd[1];
  pollfds[n].events = POLLIN;
  n++;

  n = poll (pollfds, n, -1);

  if (n < 1) {
    return 0;
  } else {
    if (pollfds[0].revents == POLLIN) {
      return kr_player_msg_get (player, msg);
    }
  }

  return 0;
}

static void *kr_player_thread (void *arg) {

  kr_player_t *player;
  kr_player_msg_t msg;
  int running;
  int ret;

  running = 1;
  player = (kr_player_t *)arg;

  while (running) {
    //printf ("Cycle!\n");
    ret = kr_player_msg_poll (player, &msg);
    if (ret == 0) {
      break;
    }
    switch (msg.cmd) {
      case TICKLE:
        printf ("Got TICKLE command!\n");
        break;
      case DESTROY:
        printf ("Got DESTROY command!\n");
        running = 0;
        break;
      case PAUSE:
        printf ("Got PAUSE command!\n");
        break;
      case STOP:
        printf ("Got STOP command!\n");
        break;
      case CUE:
        printf ("Got CUE command!\n");
        break;
      case PLAY:
        printf ("Got PLAY command!\n");
        break;
      case SEEK:
        printf ("Got SEEK %"PRIi64" command!\n", msg.param.integer);
        break;
      case SETSPEED:
        printf ("Got SETSPEED %f command!\n", msg.param.real);
        break;
      case SETDIR:
        printf ("Got SETDIR %"PRIi64" command!\n", msg.param.integer);
        break;    
    }
  }

  return NULL;
}

/* Public Functions */

kr_player_t *kr_player_create () {

  kr_player_t *player;
  
  player = calloc (1, sizeof(kr_player_t));

  if (socketpair (AF_UNIX, SOCK_STREAM, 0, player->fd)) {
    fprintf (stderr, "Can't socketpair errno: %d\n", errno);
    free (player);
    return NULL;
  }
  
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
    kr_player_msg_send (*player, &msg);
    pthread_join ((*player)->thread, NULL);
    close ((*player)->fd[0]);
    close ((*player)->fd[1]);
    free (*player);
    *player = NULL;
    printf ("kr_player_destroy()!\n");    
  }
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

kr_playback_state_t kr_player_playback_state_get (kr_player_t *player) {
  return player->state;
}

void kr_player_speed_set (kr_player_t *player, float speed) {
  kr_player_msg_t msg;
  msg.cmd = SETSPEED;
  msg.param.real = speed;
  kr_player_msg_send (player, &msg);
}

void kr_player_direction_set (kr_player_t *player, kr_direction_t direction) {
  kr_player_msg_t msg;
  msg.cmd = SETDIR;
  msg.param.integer = direction;
  kr_player_msg_send (player, &msg);
}

void kr_player_seek (kr_player_t *player, int64_t position) {
  kr_player_msg_t msg;
  msg.cmd = SEEK;
  msg.param.integer = position;
  kr_player_msg_send (player, &msg);
}

void kr_player_play (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.cmd = PLAY;
  kr_player_msg_send (player, &msg);
}

void kr_player_pause (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.cmd = PAUSE;
  kr_player_msg_send (player, &msg);
}

void kr_player_stop (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.cmd = STOP;
  kr_player_msg_send (player, &msg);
}
