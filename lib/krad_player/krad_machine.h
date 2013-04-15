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
#include "krad_msg.h"

#ifndef KRAD_MACHINE_H
#define KRAD_MACHINE_H

typedef struct kr_machine_St kr_machine_t;
typedef struct kr_machine_params_St kr_machine_params_t;

struct kr_machine_params_St {
  void *actual;
  size_t msg_sz;
  void (*start)(void *);
  int32_t (*process)(void *, void *);
  void (*destroy)(void *);
};

struct kr_machine_St {
  pthread_t thread;
  kr_msgsys_t *msgsys;
  kr_machine_params_t params;
};

kr_machine_t *krad_machine_create (kr_machine_params_t *params);
void krad_machine_destroy (kr_machine_t **machine);

void krad_machine_msg (kr_machine_t *machine, void *msg);
void krad_machine_msg2 (kr_machine_t *machine, void *msg);
#endif
