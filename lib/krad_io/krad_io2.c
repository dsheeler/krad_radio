#include "krad_io2.h"

kr_io2_t *kr_io2_create_size (size_t size) {
  kr_io2_t *io;
  io = malloc (sizeof(kr_io2_t));
  io->size = size;
  io->buffer = malloc (io->size);  
  io->fd = -1;
  kr_io2_restart (io);
  return io; 
}

kr_io2_t *kr_io2_create () {
  return kr_io2_create_size (KR_IO2_BUF_SZ); 
}

int kr_io2_destroy (kr_io2_t **io) {
  if ((io != NULL) && (*io != NULL)) {
    free ((*io)->buffer);
    free (*io);
    *io = NULL;
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
  if (io->len > 0) {
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

void kr_io2_advance (kr_io2_t *io, size_t bytes) {
  //io->pos += bytes;
  io->len += bytes;
  io->space -= bytes;
  //io->buf += io->pos;
  io->buf += bytes;
}

void kr_io2_pack (kr_io2_t *io, void *buffer, size_t len) {
  memcpy (io->buf, buffer, len);
  kr_io2_advance (io, len);
}

size_t kr_io2_write (kr_io2_t *io) {
  return write (io->fd, io->buffer, io->len);
}

int kr_io2_restart (kr_io2_t *io) {
  io->buf = io->buffer;
  io->rd_buf = io->buffer;
  //io->pos = 0;
  io->len = 0;
  io->space = io->size;
  return 0;
}

int kr_io2_sync (kr_io2_t *io) {

  int bytes;
  int ret;
  int len;
  uint8_t *buffer;
  
  buffer = io->buffer;
  len = io->len;
  ret = 0;
  bytes = 0;

  if (!kr_io2_want_out (io)) {
    return 0;
  }

  while (bytes != len) {
    ret += write (io->fd, buffer + bytes, len - bytes);
    if (ret <= 0) {
      break;
    } else {
      bytes += ret;
    }
  }
  
  kr_io2_restart (io);
  
  return bytes;
}

int kr_io2_flush (kr_io2_t *io) {

  int ret;
  int len;

  if (!kr_io2_want_out (io)) {
    return 0;
  }

  len = io->len;

  ret = kr_io2_write (io);
  if (ret != len) {
    
    printk ("Could not write all we wanted to: %d of %d",
            ret, len);
    printke ("Failing here a temporary test");
    exit (77);
  }

  kr_io2_restart (io);

  return 0;
}

void kr_io2_pulled (kr_io2_t *io, size_t bytes) {
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

