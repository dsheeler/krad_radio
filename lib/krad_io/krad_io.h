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

#define KR_IO2_BUF_SZ 8192 * 8

typedef struct kr_io2_St kr_io2_t;

typedef enum {
  KRAD_IO_READONLY,
  KRAD_IO_WRITEONLY,
  //krad_EBML_IO_READWRITE,
} krad_io_mode_t;

struct kr_io2_St {
  int fd;
  krad_io_mode_t mode;
  size_t len;   // length of data in buffer
  size_t space; // space remaining in buffer
  size_t size;  // total size of buffer
  uint8_t *buf; // (output: buffer users position) (input: position to read data to)
  uint8_t *wr_buf; // position of output buffer to write out from
  uint8_t *rd_buf; // position in input buffer the user has read to (always behind *buf)
  uint8_t *buffer; // actual buffer
};

int kr_io2_restart(kr_io2_t *io);

kr_io2_t *kr_io2_create();
kr_io2_t *kr_io2_create_size(size_t size);
int kr_io2_destroy(kr_io2_t **io);
int kr_io2_set_fd(kr_io2_t *io, int fd);
int kr_io2_want_out(kr_io2_t *io); // buffer has data ready to be written out
int kr_io2_has_in(kr_io2_t *io); // buffer has data for buffer user to read
void kr_io2_advance(kr_io2_t *io, size_t bytes); // (output: notify that data has been packed into buffer for output) (input: N data has been read from socket and is ready for user)
void kr_io2_pack(kr_io2_t *io, void *buffer, size_t len); // pack data into an output buffer
int kr_io2_output(kr_io2_t *io); // write as much output buffer as possible
int kr_io2_sync(kr_io2_t *io); // temp file only kludge
void kr_io2_pulled(kr_io2_t *io, size_t bytes); // advance the users read pointer by N
size_t kr_io2_read(kr_io2_t *io); // actually read into buffer

#include "gen/krad_io_to_ebml.h"

#endif
