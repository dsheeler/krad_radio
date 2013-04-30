#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <arpa/inet.h>

#ifndef KRAD_TRANSMITTER_H
#define KRAD_TRANSMITTER_H

#ifdef KR_LINUX
#include <sys/epoll.h>
#endif

#ifdef KRAD_RADIO
#include "krad_radio_version.h"
#define KRAD_TRANSMITTER_SERVER APPVERSION
#else
#define KRAD_TRANSMITTER_SERVER "Krad Transmitter 21"
#endif

#include "krad_system.h"
#include "krad_ring.h"

#define DEFAULT_MAX_RECEIVERS_PER_TRANSMISSION 128
#define DEFAULT_MAX_TRANSMISSIONS 32
#define TOTAL_RECEIVERS DEFAULT_MAX_RECEIVERS_PER_TRANSMISSION * DEFAULT_MAX_TRANSMISSIONS

#define KRAD_TRANSMITTER_MAXEVENTS 64
#define DEFAULT_RING_SIZE 10000000

typedef struct krad_transmitter_St krad_transmitter_t;
typedef struct krad_transmission_St krad_transmission_t;
typedef struct krad_transmitter_St kr_transmitter_t;
typedef struct krad_transmission_St kr_transmission_t;

krad_transmission_t *krad_transmitter_transmission_create (krad_transmitter_t *transmitter, char *mount, char *content_type);
void krad_transmitter_transmission_destroy (krad_transmission_t *transmission);

#define kr_transmission_create krad_transmitter_transmission_create
#define kr_transmission_destroy krad_transmitter_transmission_destroy

int krad_transmitter_transmission_set_header (krad_transmission_t *transmission, uint8_t *buffer, int length);
void krad_transmitter_transmission_add_header (krad_transmission_t *transmission, uint8_t *buffer, int length);

int krad_transmitter_transmission_add_data (krad_transmission_t *transmission, uint8_t *buffer, int length);
int krad_transmitter_transmission_add_data_sync (krad_transmission_t *transmission, uint8_t *buffer, int length);
int krad_transmitter_transmission_add_data_opt (krad_transmission_t *transmission, uint8_t *buffer, int length, int sync);

int krad_transmitter_listen_on (krad_transmitter_t *transmitter, uint16_t port);
void krad_transmitter_stop_listening (krad_transmitter_t *transmitter);

krad_transmitter_t *krad_transmitter_create ();
void krad_transmitter_destroy (krad_transmitter_t *transmitter);

#endif
