#include "krad_slice.h"

int kr_slice_set_data (kr_slice_t *kr_slice, uint8_t *data, uint32_t size) {
  if ((kr_slice->size == 0) && (kr_slice->data == NULL)) {
    kr_slice->size = size;
    kr_slice->data = malloc (kr_slice->size);
    memcpy (kr_slice->data, data, kr_slice->size);
    return kr_slice->size;
  }
  return 0;
}

void kr_slice_destroy (kr_slice_t *kr_slice) {
  if ((kr_slice->size > 0) && (kr_slice->data != NULL)) {
    free (kr_slice->data);
    kr_slice->size = 0;
  }
  free (kr_slice);
}

kr_slice_t *kr_slice_create () {
  kr_slice_t *kr_slice;
  kr_slice = calloc (1, sizeof(kr_slice_t));
  kr_slice_ref (kr_slice);  
  return kr_slice;
}

kr_slice_t *kr_slice_create_with_data (uint8_t *data, uint32_t size) {
  kr_slice_t *kr_slice;
  kr_slice = kr_slice_create ();
  kr_slice_set_data (kr_slice, data, size);
  return kr_slice;
}

void kr_slice_ref (kr_slice_t *kr_slice) {
  __sync_fetch_and_add( &kr_slice->refs, 1 );
}

void kr_slice_unref (kr_slice_t *kr_slice) {
  __sync_fetch_and_sub( &kr_slice->refs, 1 );
  if (__sync_bool_compare_and_swap( &kr_slice->refs, 0, -1 )) {
    kr_slice_destroy (kr_slice);
  }
}
