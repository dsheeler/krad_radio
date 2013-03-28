#include "krad_ebml2.h"

#ifndef KRAD_MKV_H
#define KRAD_MKV_H

typedef struct kr_mkv_St kr_mkv_t;

struct kr_mkv_St {
  int yeah;
};


int kr_mkv_destroy (kr_mkv_t **mkv);
kr_mkv_t *kr_mkv_create ();


#endif
