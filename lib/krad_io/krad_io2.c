#include "krad_io2.h"

    // krad_io2_mode_t mode ?
kr_io2_t *kr_io2_create () {
  kr_io2_t *io;
  io = malloc (sizeof(kr_io2_t));
  io->buf = ebml->buffer;
  io->pos = 0;
  return io; 
}

int kr_io2_destroy (kr_io2_t **io) {
  if ((io != NULL) && (*io != NULL)) {
    free (*io);
    return 0;
  }
  return -1;
}

int krad_io2_want_out (krad_io_t *io) {
  if (io->pos > 0) {
    return 1;
  }
  return 0;
}

inline void kr_io2_advance (krad_io_t *io, size_t bytes) {
  io->pos += bytes;
  io->buf = io->buffer + io->pos;
}

inline void kr_io2_pack (krad_io_t *io, void *buffer, size_t len) {
  memcpy (io->buf, buffer, len);
  kr_io2_advance (io, len);
}

int krad_io2_write (krad_io_t *io) {

  int ret;
  
  ret = write (io->fd, buffer, io->pos);
  //fixme
}

int krad_io2_read (krad_io_t *io, //fixme) {

  int ret;
  
  ret = read (io->fd, buffer, io->pos);
  
}
