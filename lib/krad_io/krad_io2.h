#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <limits.h>

#include "krad_system.h"

#ifndef KRAD_IO2_H
#define KRAD_IO2_H

#define KRAD_IO2_WR_BUF_SZ 8192

typedef struct krad_io2_St krad_io2_t;

typedef enum {
  //KRAD_IO2_READONLY,
  KRAD_IO2_WRITEONLY,
  //krad_EBML_IO_READWRITE,
} krad_io2_mode_t;

struct krad_io2_St {
  int fd;
  krad_io2_mode_t mode;


  unsigned char wr_buf[KRAD_IO2_WR_BUF_SZ];
  size_t wr_buf_pos;
};

krad_io2_t *krad_io2_create ();
int krad_io2_destroy (krad_io2_t **io2);
int krad_io2_want_out (krad_io_t *krad_io);
int krad_io2_pack (krad_io_t *krad_io, void *buffer, size_t length);
int krad_io2_write (krad_io_t *krad_io);

#endif
