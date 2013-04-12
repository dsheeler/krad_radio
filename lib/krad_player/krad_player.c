#include "krad_player.h"

#define CTRLMP 0
#define VIDEOMP 1
#define AUDIOMP 2

typedef struct kr_msg_St kr_msg_t;
typedef struct kr_msgpair_St kr_msgpair_t;

struct kr_msg_St {
  kr_player_cmd_t cmd;
  union {
    float real;
    int64_t integer;
  } param;
};

struct kr_msgpair_St {
  /*
    ringbuf
    int poolsize;
    kr_msg_t **pool;
  */
  int fd[2];
};

struct kr_player_St {
  pthread_t thread;
  kr_msgpair_t **msgpair;
  char *url;
  float speed;
  int64_t position;
  kr_player_state_t state;
  kr_direction_t direction;
  krad_container_t *input;
};

/* Private Functions */

static kr_msg_t *kr_player_alloc_msg (kr_msgpair_t *msgpair) {
  /* printf ("Msg Alloc!\n"); */
  if (msgpair == NULL) {
    return NULL;
  }
  return calloc (1, sizeof(kr_msg_t));
}

static void kr_player_free_msg (kr_msgpair_t *msgpair, kr_msg_t **msg) {
  if (msgpair == NULL) {
    return;
  }
  if ((msg != NULL) && (*msg != NULL)) {
    free (*msg);
    *msg = NULL;
    /* printf ("Msg Free!\n"); */
  }
}

void kr_msgpair_destroy (kr_msgpair_t **msgpair) {
  if ((msgpair != NULL) && (*msgpair != NULL)) {
    if ((*msgpair)->fd[0] > -1) {
      close ((*msgpair)->fd[0]);
      (*msgpair)->fd[0] = 0;
    }
    if ((*msgpair)->fd[1] > -1) {
      close ((*msgpair)->fd[1]);
      (*msgpair)->fd[1] = 0;
    }
    free (*msgpair);
    *msgpair = NULL;
  }
}

void kr_msgpairs_destroy (kr_msgpair_t **msgpairs, int count) {

  int i;
  
  if ((msgpairs != NULL) && (*msgpairs != NULL)) {
    for (i = 0; i < count; i++) {
      kr_msgpair_destroy (&msgpairs[i]);
    }
    free (msgpairs);
  }
}

kr_msgpair_t *kr_msgpair_create () {

  kr_msgpair_t *msgpair;

  msgpair = calloc (1, sizeof (kr_msgpair_t));

  if (socketpair (AF_UNIX, SOCK_STREAM, 0, msgpair->fd)) {
    fprintf (stderr, "Can't socketpair errno: %d\n", errno);
    kr_msgpair_destroy (&msgpair);
    return NULL;
  }
  return msgpair;
}

kr_msgpair_t **kr_msgpairs_create (int count) {

  int i;
  kr_msgpair_t **msgpairs;
  
  msgpairs = calloc (count, sizeof (kr_msgpair_t *));
  
  for (i = 0; i < count; i++) {
    msgpairs[i] = kr_msgpair_create ();
    if (msgpairs[i] == NULL) {
      kr_msgpairs_destroy (msgpairs, count);
      return NULL;
    }
  }
  return msgpairs;
}

static int kr_msg_write (kr_msgpair_t *msgpair, kr_msg_t *msgin) {
  
  int ret;
  kr_msg_t *msg;
  
  msg = kr_player_alloc_msg (msgpair);
  
  memcpy (msg, msgin, sizeof(kr_msg_t));
        
  ret = write (msgpair->fd[0], &msg, sizeof(kr_msg_t *));    

  if (ret != sizeof(kr_msg_t *)) {
    fprintf (stderr, "Oh dear! write %d", ret);
    exit (1);
  }
  /* printf ("Msg send!\n"); */
  return ret;
}

static int kr_msg_read (kr_msgpair_t *msgpair, kr_msg_t *msgout) {

  int ret;
  kr_msg_t *msg;

  ret = read (msgpair->fd[1], &msg, sizeof(kr_msg_t *));    

  if (ret != sizeof(kr_msg_t *)) {
    fprintf (stderr, "Oh dear! read %d", ret);
    exit (1);
  }

  memcpy (msgout, msg, sizeof(kr_msg_t));

  kr_player_free_msg (msgpair, &msg);
  /* printf ("Msg Get!\n"); */
  return ret;
}

static int kr_msg_wait (kr_player_t *player, kr_msg_t *msg) {

  struct pollfd pollfds[4];
  int n;

  n = 0;
  pollfds[n].fd = player->msgpair[n]->fd[1];
  pollfds[n].events = POLLIN;
  n++;

  n = poll (pollfds, n, -1);

  if (n < 1) {
    return 0;
  } else {
    if (pollfds[0].revents == POLLIN) {
      return kr_msg_read (player->msgpair[0], msg);
    }
  }

  return 0;
}

static void *kr_player_thread (void *arg) {

  kr_player_t *player;
  kr_msg_t msg;
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

  while (running) {
    /* printf ("Cycle!\n"); */
    ret = kr_msg_wait (player, &msg);
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
        printf ("Got SETSPEED %0.3f command!\n", msg.param.real);
        player->speed = msg.param.real;
        break;
      case SETDIR:
        printf ("Got SETDIR %"PRIi64" command!\n", msg.param.integer);
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
  player->msgpair = kr_msgpairs_create (3);
  
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
  kr_msg_t msg;
  if ((player != NULL) && (*player != NULL)) {
    msg.cmd = DESTROY;
    kr_msg_write ((*player)->msgpair[CTRLMP], &msg);
    pthread_join ((*player)->thread, NULL);
    kr_msgpairs_destroy ((*player)->msgpair, 3);
    free ((*player)->url);
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

kr_player_state_t kr_player_state_get (kr_player_t *player) {
  return player->state;
}

void kr_player_speed_set (kr_player_t *player, float speed) {
  kr_msg_t msg;
  msg.cmd = SETSPEED;
  msg.param.real = roundf (speed*1000.0f)/1000.0f;
  kr_msg_write (player->msgpair[CTRLMP], &msg);
}

void kr_player_direction_set (kr_player_t *player, kr_direction_t direction) {
  kr_msg_t msg;
  msg.cmd = SETDIR;
  msg.param.integer = direction;
  kr_msg_write (player->msgpair[CTRLMP], &msg);
}

void kr_player_seek (kr_player_t *player, int64_t position) {
  kr_msg_t msg;
  msg.cmd = SEEK;
  msg.param.integer = position;
  kr_msg_write (player->msgpair[CTRLMP], &msg);
}

void kr_player_play (kr_player_t *player) {
  kr_msg_t msg;
  msg.cmd = PLAY;
  kr_msg_write (player->msgpair[CTRLMP], &msg);
}

void kr_player_pause (kr_player_t *player) {
  kr_msg_t msg;
  msg.cmd = PAUSE;
  kr_msg_write (player->msgpair[CTRLMP], &msg);
}

void kr_player_stop (kr_player_t *player) {
  kr_msg_t msg;
  msg.cmd = STOP;
  kr_msg_write (player->msgpair[CTRLMP], &msg);
}
