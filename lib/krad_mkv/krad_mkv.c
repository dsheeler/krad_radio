#include "krad_mkv.h"

kr_mkv_t *kr_mkv_create () {
  kr_mkv_t *mkv;
  mkv = malloc (sizeof(kr_mkv_t));

  return mkv;
}

int kr_mkv_destroy (kr_mkv_t **mkv) {
  if ((mkv != NULL) && (*mkv != NULL)) {
    free (*mkv);
    return 0;
  }
  return -1;
}
