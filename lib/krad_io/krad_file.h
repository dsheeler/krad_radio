#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "krad_io2.h"

#ifndef KRAD_FILE_H
#define KRAD_FILE_H

kr_io2_t *kr_file_open (char *filename);
kr_io2_t *kr_file_create (char *filename);

#endif
