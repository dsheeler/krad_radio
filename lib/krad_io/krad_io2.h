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

#ifndef KR_IO2_H
#define KR_IO2_H

#define KR_IO2_BUF_SZ 8192

typedef struct kr_io2_St kr_io2_t;

typedef enum {
  KRAD_IO_READONLY,
  KRAD_IO_WRITEONLY,
  //krad_EBML_IO_READWRITE,
} krad_io_mode_t;

struct kr_io2_St {
  int fd;
  krad_io_mode_t mode;
  //size_t pos;   // position in buffer
  size_t len;   // length of data in buffer
  size_t space; // space remaining in buffer
  size_t size;  // total size of buffer
  uint8_t *buf;
  uint8_t *rd_buf;
  uint8_t *buffer;
};

int kr_io2_restart (kr_io2_t *io);

kr_io2_t *kr_io2_create ();
kr_io2_t *kr_io2_create_size (size_t size);
int kr_io2_destroy (kr_io2_t **io);
int kr_io2_set_fd (kr_io2_t *io, int fd);
int kr_io2_want_out (kr_io2_t *io);
int kr_io2_has_in (kr_io2_t *io);
void kr_io2_advance (kr_io2_t *io, size_t bytes);
void kr_io2_pack (kr_io2_t *io, void *buffer, size_t len);
size_t kr_io2_write (kr_io2_t *io);
int kr_io2_flush (kr_io2_t *io);
int kr_io2_sync (kr_io2_t *io);
void kr_io2_pulled (kr_io2_t *io, size_t bytes);
size_t kr_io2_read (kr_io2_t *io);

#endif
