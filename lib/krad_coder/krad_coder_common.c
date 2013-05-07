#include "krad_coder_common.h"

kr_codeme_t *kr_codeme_kludge_create () {
  kr_codeme_t *codeme;
  codeme = calloc (1, sizeof(kr_codeme_t));
  codeme->data = malloc (1000000);
  return codeme;
}

int32_t kr_codeme_kludge_destroy (kr_codeme_t **codeme) {
  if ((codeme != NULL) && (*codeme != NULL)) {
    free ((*codeme)->data);
    free (*codeme);
    *codeme = NULL;
    return 0;
  }
  return -1;
}

kr_medium_t *kr_medium_kludge_create () {
  kr_medium_t *medium;
  int32_t c;
  medium = calloc (1, sizeof(kr_medium_t));
  medium->sz = 1920 * 1200 * 4;
  medium->data = calloc (1, medium->sz);
  medium->v.px = medium->data;
  for (c = 0; c < 6; c++) {
    medium->a.samples[c] = (float *)medium->data + (c * 4096 * 4);
  }
  return medium;
}

int32_t kr_medium_kludge_destroy (kr_medium_t **medium) {
  if ((medium != NULL) && (*medium != NULL)) {
    free ((*medium)->data);
    free (*medium);
    *medium = NULL;
    return 0;
  }
  return -1;
}
