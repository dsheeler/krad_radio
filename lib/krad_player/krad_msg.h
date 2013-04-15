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

#ifndef KRAD_MSG_H
#define KRAD_MSG_H

typedef struct kr_msg_St kr_msg_t;
typedef struct kr_msgpair_St kr_msgpair_t;
typedef struct kr_msgsys_St kr_msgsys_t;

struct kr_msg_St {
  void *ptr;
  /*int refs; */
};

struct kr_msgpair_St {
  /*
    ringbuf
    int poolsize;
    kr_msg_t **pool;
  */
  kr_msgsys_t *msgsys;
  int fd[2];
};

struct kr_msgsys_St {
  kr_msgpair_t **msgpairs;
  struct pollfd *pollfds;
  uint32_t msgpairs_count;
  size_t msg_sz;
};

int kr_msgsys_write (kr_msgsys_t *msgsys, uint32_t msgpair, void *msgin);
int kr_msgsys_wait (kr_msgsys_t *msgsys, void *msg);
int kr_msgsys_get_fd (kr_msgsys_t *msgsys, uint32_t msgpair);
int kr_msgsys_read (kr_msgsys_t *msgsys, uint32_t msgpair, void *msg);
kr_msgsys_t *kr_msgsys_create (uint32_t count, size_t msg_sz);
void kr_msgsys_destroy (kr_msgsys_t **msgsys);

#endif
