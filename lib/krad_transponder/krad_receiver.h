#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>

#include "krad_system.h"

#ifndef KRAD_RECEIVER_H
#define KRAD_RECEIVER_H

typedef struct krad_receiver_St krad_receiver_t;
typedef struct krad_receiver_client_St krad_receiver_client_t;

struct krad_receiver_St {
  unsigned char *buffer;
  int port;
  int sd;
  struct sockaddr_in local_address;
  int listening;
  int stop_listening;
  pthread_t listening_thread;
};

struct krad_receiver_client_St {

  krad_receiver_t *krad_receiver;
  pthread_t client_thread;

  char in_buffer[1024];
  char out_buffer[1024];
  char mount[256];
  char content_type[256];
  int got_mount;
  int got_content_type;

  int in_buffer_pos;
  int out_buffer_pos;
  
  int sd;
  int ret;
  int wrote;
};

void krad_receiver_stop_listening (krad_receiver_t *receiver);
int krad_receiver_listen_on (krad_receiver_t *receiver, uint16_t port);

void krad_receiver_destroy (krad_receiver_t *receiver);
krad_receiver_t *krad_receiver_create ();

#endif
