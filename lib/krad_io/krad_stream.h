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

int kr_stream_connect (char *host, int port, char *mount, char *password);
kr_io2_t *kr_stream (char *host, int port, char *mount, char *password);

#endif
