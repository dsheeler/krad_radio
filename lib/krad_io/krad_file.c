#include "krad_file.h"

kr_io2_t *kr_file_create (char *filename) {
  
  kr_io2_t *io;
  int flags;
  int fd;
  
  flags = O_WRONLY | O_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  
  if (file_exists(filename)) {
    return NULL;
  }
  
  fd = open ( filename, flags );
  
  if (fd < 0) {
    return NULL;
  }
  
  io = kr_io2_create_size (65535);
  kr_io2_set_fd (io, fd);

  return io;
}

kr_io2_t *kr_file_open (char *filename) {
  
  kr_io2_t *io;
  int flags;
  int fd;
  
  flags = O_RDONLY;
  
  if (!file_exists(filename)) {
    return NULL;
  }
  
  fd = open ( filename, flags );
  
  if (fd < 0) {
    return NULL;
  }
  
  io = kr_io2_create_size (65535);
  kr_io2_set_fd (io, fd);

  kr_io2_read (io);

  printk ("read %zu bytes", io->len);

  return io;
}
