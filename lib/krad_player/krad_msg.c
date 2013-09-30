#include "krad_msg.h"

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

static kr_msg_t *kr_player_alloc_user_msg (kr_msgpair_t *msgpair) {
  /* printf ("Msg Alloc!\n"); */
  if (msgpair == NULL) {
    return NULL;
  }
  return calloc (1, msgpair->msgsys->msg_sz);
}

static void kr_player_free_user_msg (kr_msgpair_t *msgpair, void **usermsg) {
  if (msgpair == NULL) {
    return;
  }
  if ((usermsg != NULL) && (*usermsg != NULL)) {
    free (*usermsg);
    *usermsg = NULL;
    /* printf ("Msg Free!\n"); */
  }
}

static int kr_msg_read (kr_msgpair_t *msgpair, void *msgout) {

  int ret;
  kr_msg_t *msg;

  ret = read (msgpair->fd[1], &msg, sizeof(kr_msg_t *));

  if (ret != sizeof(kr_msg_t *)) {
    fprintf (stderr, "Oh dear! read %d", ret);
    exit (1);
  }

  memcpy (msgout, msg->ptr, msgpair->msgsys->msg_sz);
  //*msgout = msg->ptr;
  kr_player_free_user_msg (msgpair, &msg->ptr);
  kr_player_free_msg (msgpair, &msg);
  /* printf ("Msg Get!\n"); */
  return ret;
}

static void kr_msgpair_destroy (kr_msgpair_t **msgpair) {
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

static kr_msgpair_t *kr_msgpair_create (kr_msgsys_t *msgsys) {

  kr_msgpair_t *msgpair;

  msgpair = calloc (1, sizeof (kr_msgpair_t));
  msgpair->msgsys = msgsys;
  if (socketpair (AF_UNIX, SOCK_STREAM, 0, msgpair->fd)) {
    fprintf (stderr, "Can't socketpair errno: %d\n", errno);
    kr_msgpair_destroy (&msgpair);
    return NULL;
  }
  return msgpair;
}

static void kr_msgpairs_destroy (kr_msgsys_t *msgsys) {

  int i;

  if (msgsys->msgpairs != NULL) {
    for (i = 0; i < msgsys->msgpairs_count; i++) {
      kr_msgpair_destroy (&msgsys->msgpairs[i]);
    }
    free (msgsys->msgpairs);
  }
}

static kr_msgpair_t **kr_msgpairs_create (kr_msgsys_t *msgsys) {

  int i;
  kr_msgpair_t **msgpairs;

  msgpairs = calloc (msgsys->msgpairs_count, sizeof (kr_msgpair_t *));

  for (i = 0; i < msgsys->msgpairs_count; i++) {
    msgpairs[i] = kr_msgpair_create (msgsys);
    if (msgpairs[i] == NULL) {
      kr_msgpairs_destroy (msgsys);
      return NULL;
    }
  }
  return msgpairs;
}

int kr_msgsys_wait (kr_msgsys_t *msgsys, void *msg) {

  int n;
  int ret;

  for (n = 0; n < msgsys->msgpairs_count; n++) {
    msgsys->pollfds[n].fd = msgsys->msgpairs[n]->fd[1];
    msgsys->pollfds[n].events = POLLIN;
  }

  n = poll (msgsys->pollfds, msgsys->msgpairs_count, -1);

  if (n < 1) {
    return 0;
  } else {
    for (n = 0; n < msgsys->msgpairs_count; n++) {
      if (msgsys->pollfds[n].revents == POLLIN) {
        ret = kr_msg_read (msgsys->msgpairs[n], msg);
        return ret;
      }
      if (msgsys->pollfds[n].revents == POLLHUP) {
        return 0;
      }
    }
  }

  return 0;
}

int kr_msgsys_write (kr_msgsys_t *msgsys, uint32_t msgpair, void *msgin) {

  int ret;
  kr_msg_t *msg;
  void *msgcpy;

  msg = kr_player_alloc_msg (msgsys->msgpairs[msgpair]);
  msgcpy = kr_player_alloc_user_msg (msgsys->msgpairs[msgpair]);

  memcpy (msgcpy, msgin, msgsys->msg_sz);
  msg->ptr = msgcpy;

  ret = write (msgsys->msgpairs[msgpair]->fd[0], &msg, sizeof(kr_msg_t *));

  if (ret != sizeof(kr_msg_t *)) {
    fprintf (stderr, "Oh dear! write %d", ret);
    exit (1);
  }
  /* printf ("Msg send!\n"); */
  return ret;
}

int kr_msgsys_get_fd (kr_msgsys_t *msgsys, uint32_t msgpair) {
  return msgsys->msgpairs[msgpair]->fd[1];
}

int kr_msgsys_read (kr_msgsys_t *msgsys, uint32_t msgpair, void *msg) {
  return kr_msg_read (msgsys->msgpairs[msgpair], msg);
}

void kr_msgsys_destroy (kr_msgsys_t **msgsys) {

  if ((msgsys != NULL) && (*msgsys != NULL)) {
    kr_msgpairs_destroy (*msgsys);
    free ((*msgsys)->pollfds);
    free (*msgsys);
    *msgsys = NULL;
  }
}

kr_msgsys_t *kr_msgsys_create (uint32_t count, size_t msg_sz) {

  kr_msgsys_t *msgsys;

  msgsys = calloc (1, sizeof (kr_msgsys_t));
  msgsys->msgpairs_count = count;
  msgsys->msg_sz = msg_sz;
  msgsys->pollfds = calloc (msgsys->msgpairs_count, sizeof(struct pollfd));
  msgsys->msgpairs = kr_msgpairs_create (msgsys);
  return msgsys;
}
