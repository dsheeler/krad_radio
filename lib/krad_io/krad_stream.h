#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef KRAD_STREAM_H
#define KRAD_STREAM_H

#include "krad_io2.h"

typedef struct krad_stream_St krad_stream_t;

struct krad_stream_St {
  int32_t sd;
  uint64_t position;
  int32_t readable;
  int32_t writable;

  char *host;
  int32_t port;
  char *mount;

  char *password;
  char *content_type;
};

/* Close SD only */
int kr_stream_disconnect (krad_stream_t *stream);

/* Free stream but do not close SD */
int kr_stream_free (krad_stream_t **stream);

/* Disconnect and then free stream */
int kr_stream_destroy (krad_stream_t **stream);

int kr_stream_reconnect (krad_stream_t *stream);

int kr_stream_send (krad_stream_t *stream, void *buffer, size_t len);
int kr_stream_recv (krad_stream_t *stream, void *buffer, size_t len);

krad_stream_t *kr_stream_create (char *host, int port,
                                 char *mount, char *content_type,
                                 char *password);

krad_stream_t *kr_stream_open (char *host, int port, char *mount);

#endif
