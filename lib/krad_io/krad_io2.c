#include "krad_io2.h"

// kr_io2_mode_t mode ?
kr_io2_t *kr_io2_create () {
  kr_io2_t *io;
  io = malloc (sizeof(kr_io2_t));
  io->fd = -1;
  io->buf = io->buffer;
  io->pos = 0;
  io->len = 0;
  io->space = KR_IO2_BUF_SZ;
  io->size = KR_IO2_BUF_SZ;
  return io; 
}

int kr_io2_destroy (kr_io2_t **io) {
  if ((io != NULL) && (*io != NULL)) {
    free (*io);
    return 0;
  }
  return -1;
}

int kr_io2_set_fd (kr_io2_t *io, int fd) {
  if (fd < 0) {
    return -1;
  }
  io->fd = fd;
  return 0;
}

int kr_io2_want_out (kr_io2_t *io) {
  if (io->pos > 0) {
    return 1;
  }
  return 0;
}

inline void kr_io2_advance (kr_io2_t *io, size_t bytes) {
  io->pos += bytes;
  io->len += bytes;
  io->space -= bytes;
  io->buf = io->buffer + io->pos;
}

inline void kr_io2_pack (kr_io2_t *io, void *buffer, size_t len) {
  memcpy (io->buf, buffer, len);
  kr_io2_advance (io, len);
}

int kr_io2_write (kr_io2_t *io) {

  int ret;

  ret = write (io->fd, &io->buffer, io->len);

  return ret;
}

int kr_io2_flush (kr_io2_t *io) {

  int ret;
  int len;
  
  len = io->len;
  
  ret = write (io->fd, &io->buffer, io->len);
  if (ret != len) {
    return -1;
  }

  io->buf = io->buffer;
  io->pos = 0;
  io->len = 0;

  return 0;
}

