#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef KR_LINUX
#include <sys/statfs.h>
#else
#ifdef FRAK_MACOSX
#include <sys/param.h>
#include <sys/mount.h>
#endif
#endif

//#include "krad_system.h"

#ifndef KRAD_FILE_H
#define KRAD_FILE_H

typedef struct kr_file_St kr_file_t;

struct kr_file_St {
  int32_t fd;
  size_t position;
  int32_t readable;
  int32_t writable;
  int32_t seekable;
  int32_t std_stream;
  int32_t local;
  int32_t remote;
  int32_t mapmode;
  char *path;
  struct stat st;
  struct statfs stfs;
};

int file_exists (char *path);
int64_t file_size (char *path);

kr_file_t *kr_file_open (char *path);
kr_file_t *kr_file_create (char *path);

int kr_file_close (kr_file_t **file);

#endif
