#include "krad_io2.h"

static int kr_io2_restart (kr_io2_t *io);

// kr_io2_mode_t mode ?
kr_io2_t *kr_io2_create () {
  kr_io2_t *io;
  io = malloc (sizeof(kr_io2_t));
  io->fd = -1;
  kr_io2_restart (io);
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

int kr_io2_has_in (kr_io2_t *io) {
  if (io->len > 0) {
    return 1;
  }
  return 0;
}

inline void kr_io2_advance (kr_io2_t *io, size_t bytes) {
  io->pos += bytes;
  io->len += bytes;
  io->space -= bytes;
  io->buf += io->pos;
}

inline void kr_io2_pack (kr_io2_t *io, void *buffer, size_t len) {
  memcpy (io->buf, buffer, len);
  kr_io2_advance (io, len);
}

size_t kr_io2_write (kr_io2_t *io) {
  return write (io->fd, &io->buffer, io->len);
}

static int kr_io2_restart (kr_io2_t *io) {
  io->buf = io->buffer;
  io->rd_buf = io->buffer;
  io->pos = 0;
  io->len = 0;
  io->space = KR_IO2_BUF_SZ;
  return 0;
}

int kr_io2_flush (kr_io2_t *io) {

  int ret;
  int len;
  
  len = io->len;
  
  ret = kr_io2_write (io);
  if (ret != len) {
    //return -1;
    exit (77);
  }

  kr_io2_restart (io);

  return 0;
}

inline void kr_io2_pulled (kr_io2_t *io, size_t bytes) {
  io->len -= bytes;
  io->rd_buf += bytes;
  
  if (io->len == 0) {
    kr_io2_restart (io);
  }
}

size_t kr_io2_read (kr_io2_t *io) {

  size_t ret;

  ret = read (io->fd, io->buf, io->space);
  if (ret > 0) {
    kr_io2_advance (io, ret);
  }
  return ret;
}

