#include "krad_framepool.h"

krad_frame_t *krad_framepool_getframe (krad_framepool_t *krad_framepool) {

  int f;

  for (f = 0; f < krad_framepool->count; f++ ) {
    if (__sync_bool_compare_and_swap( &krad_framepool->frames[f].refs, 0, 1 )) {
      return &krad_framepool->frames[f];
    }
  }

  return NULL;

}

void krad_framepool_ref_frame (krad_frame_t *krad_frame) {
  __sync_fetch_and_add( &krad_frame->refs, 1 );
}


void krad_framepool_unref_frame (krad_frame_t *krad_frame) {
  __sync_fetch_and_sub( &krad_frame->refs, 1 );
}

void krad_framepool_destroy (krad_framepool_t **krad_framepool) {

  int f;
  krad_framepool_t *framepool;

  if ((krad_framepool != NULL) && (*krad_framepool != NULL)) {
    framepool = *krad_framepool;
    for (f = 0; f < framepool->count; f++ ) {
      munlock (framepool->frames[f].pixels, framepool->frame_byte_size);
      free (framepool->frames[f].pixels);
      if (framepool->passthru == 0) {
        cairo_destroy (framepool->frames[f].cr);
        cairo_surface_destroy (framepool->frames[f].cst);
      }
    }

    free (framepool->frames);
    free (framepool);
    *krad_framepool = NULL;
  }
}

krad_framepool_t *krad_framepool_create (int width, int height, int count) {
  return krad_framepool_create_for_upscale (width, height, count, width, height);
}

krad_framepool_t *krad_framepool_create_for_upscale (int width, int height, int count, int upscale_width, int upscale_height) {

  krad_framepool_t *krad_framepool = calloc (1, sizeof(krad_framepool_t));

  int f;

  krad_framepool->width = width;
  krad_framepool->height = height;
  krad_framepool->count = count;

  if (upscale_width > width) {
    krad_framepool->upscale_width = upscale_width;
  } else {
    krad_framepool->upscale_width = width;
  }

  if (upscale_height > height) {
    krad_framepool->upscale_height = upscale_height;
  } else {
    krad_framepool->upscale_height = height;
  }

  krad_framepool->stride = cairo_format_stride_for_width (CAIRO_FORMAT_ARGB32, krad_framepool->upscale_width);

  krad_framepool->frame_byte_size = krad_framepool->stride * krad_framepool->upscale_height;
  
  krad_framepool->frames = calloc (krad_framepool->count, sizeof(krad_frame_t));
  
  for (f = 0; f < krad_framepool->count; f++ ) {
    krad_framepool->frames[f].pixels = malloc (krad_framepool->frame_byte_size);
    if (krad_framepool->frames[f].pixels == NULL) {
      failfast ("Krad Framepool: Out of memory");
    }
    mlock (krad_framepool->frames[f].pixels, krad_framepool->frame_byte_size);

    krad_framepool->frames[f].cst =
      cairo_image_surface_create_for_data ((unsigned char *)krad_framepool->frames[f].pixels,
                         CAIRO_FORMAT_ARGB32,
                         krad_framepool->upscale_width,
                         krad_framepool->upscale_height,
                         krad_framepool->stride);
  
    krad_framepool->frames[f].cr = cairo_create (krad_framepool->frames[f].cst);
  }
  
  return krad_framepool;

}

krad_framepool_t *krad_framepool_create_for_passthru (int size, int count) {

  krad_framepool_t *krad_framepool = calloc (1, sizeof(krad_framepool_t));

  int f;

  krad_framepool->count = count;
  krad_framepool->frame_byte_size = size;
  krad_framepool->passthru = 1;
  
  krad_framepool->frames = calloc (krad_framepool->count, sizeof(krad_frame_t));
  
  for (f = 0; f < krad_framepool->count; f++ ) {
    krad_framepool->frames[f].pixels = malloc (krad_framepool->frame_byte_size);
    if (krad_framepool->frames[f].pixels == NULL) {
      failfast ("Krad Framepool: Out of memory");
    }
    mlock (krad_framepool->frames[f].pixels, krad_framepool->frame_byte_size);
  }
  
  return krad_framepool;

}
