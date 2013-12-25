#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
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

#include "krad_system.h"

#ifndef KRAD_UDP_H
#define KRAD_UDP_H

typedef struct krad_udp_St krad_udp_t;

typedef struct krad_subslice_St krad_subslice_t;
typedef struct krad_slicer_St krad_slicer_t;
typedef struct krad_rebuilder_St krad_rebuilder_t;
typedef struct krad_rebuilder_track_St krad_rebuilder_track_t;
#define KRAD_UDP_MAX_PAYOAD_SIZE 1300
#define KRAD_UDP_HEADER_SIZE 19

typedef enum {
  K_VP8 = 1,
  K_OPUS,
  K_AUX,
} krad_slice_track_type_t;

struct krad_subslice_St {
  uint8_t *data;
  int size;
  int fill;
  int track;
  int keyframe;
  uint32_t seq;
};

struct krad_slicer_St {
  int sd;
  uint8_t *data;
  int track_seq[3];
};

struct krad_rebuilder_track_St {
  krad_subslice_t *slices;
  int slice_count;
  int slice_position;
  int slice_read_position;
};

struct krad_rebuilder_St {
  krad_rebuilder_track_t tracks[2];
};

struct krad_udp_St {
  krad_slicer_t *slicer;
  krad_rebuilder_t *rebuilder;
  char *host;
  int port;
  char *mount;
  char *password;
};

krad_udp_t *kr_udp (char *host, int port, char *mount, char *password);
void kr_udp_destroy (krad_udp_t **udp);

krad_slicer_t *krad_slicer_create ();
void krad_slicer_destroy (krad_slicer_t *krad_slicer);
void krad_slicer_sendto (krad_slicer_t *krad_slicer, uint8_t *data,
      int size, int track, int keyframe, char *ip, int port);

krad_rebuilder_t *krad_rebuilder_create ();
void krad_rebuilder_destroy (krad_rebuilder_t *krad_rebuilder);
void krad_rebuilder_write (krad_rebuilder_t *krad_rebuilder, uint8_t *data,
      int length);
int krad_rebuilder_read_packet (krad_rebuilder_t *krad_rebuilder, uint8_t *data,
     int track, int *keyframe);
#endif
