#include "krad_io2.h"

static ssize_t kr_io2_write (kr_io2_t *io);

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
  io->len += bytes;
  io->space -= bytes;
  io->buf += bytes;
}

void kr_io2_pack (kr_io2_t *io, void *buffer, size_t len) {
  memcpy (io->buf, buffer, len);
  kr_io2_advance (io, len);
}

static ssize_t kr_io2_write (kr_io2_t *io) {
  return write (io->fd, io->wr_buf, io->len);
}

int kr_io2_restart (kr_io2_t *io) {
  io->buf = io->buffer;
  io->rd_buf = io->buffer;
  io->wr_buf = io->buffer;
  io->len = 0;
  io->space = io->size;
  return 0;
}

int kr_io2_sync (kr_io2_t *io) {
 // FILES ONLY blocking or failing!

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
  ret = kr_io2_write (io);
  if (ret != len) {
    printk ("Could not write all we wanted to: %d of %d",
            ret, len);
    printke ("Failing here a, this should not happen as a file only func");
    exit (77);
  }
  kr_io2_restart (io);
  return bytes;
}

int kr_io2_output (kr_io2_t *io) {

  ssize_t ret;

  if (!kr_io2_want_out (io)) {
    return 0;
  }
  ret = kr_io2_write (io);
  if (ret != io->len) {
	if (ret < 1) {
    	printke ("its bad we should certainly fail");
    	exit (77);
	} else {
	  io->len -= ret;
	  io->wr_buf += ret;		
	}
	return 0;
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