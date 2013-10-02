#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef KRAD_STREAM_H
#define KRAD_STREAM_H

#include "krad_io.h"
#include "krad_base64.h"

typedef struct kr_stream kr_stream;

struct kr_stream {
  uint64_t position;
  int32_t sd;

  int32_t direction;
  int32_t half_ready;
  int32_t hle_pos;
  int32_t hle;
  int32_t hler;
  int32_t drain;
  int32_t connected;
  int32_t ready;

  char error_str[1024];

  int32_t err_no;

  char *host;
  int32_t port;
  char *mount;

  char *password;
  char *content_type;
};

/* Close SD only */
int32_t kr_stream_disconnect(kr_stream *stream);

/* Free stream but do not close SD */
int32_t kr_stream_free(kr_stream **stream);

/* Disconnect and then free stream */
int32_t kr_stream_destroy(kr_stream **stream);

//int32_t kr_stream_reconnect(krad_stream_t *stream);
//int32_t kr_stream_connected(krad_stream_t *stream);
//int32_t kr_stream_ready(krad_stream_t *stream);

int32_t kr_stream_handle_headers(kr_stream *stream);

ssize_t kr_stream_send(kr_stream *stream, void *buffer, size_t len);
ssize_t kr_stream_recv(kr_stream *stream, void *buffer, size_t len);

kr_stream *kr_stream_create(char *host, int32_t port, char *mount,
 char *content_type, char *password);

kr_stream *kr_stream_open(char *host, int32_t port, char *mount);

//krad_stream_t *kr_stream_accept (int32_t sd);

void kr_stream_i_am_a_blocking_subscripter(kr_stream *stream);

#endif
