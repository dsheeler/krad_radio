#include "krad_image_pool.h"

kr_image_pool *kr_image_pool_create(kr_image *image, size_t len) {
  kr_pool_setup setup;
  setup.slices = len;
  setup.overlay = image;
  setup.overlay_sz = sizeof(kr_image);
  setup.shared = 1;
  setup.size = image->w * image->h * 4;
  return kr_pool_create(&setup);
}

int kr_image_pool_getimage(kr_image_pool *image_pool, kr_image *image) {
  kr_pool_get_overlay(image_pool, image);
  image->px = kr_pool_slice(image_pool);
  if (image->px == NULL) return 0;
  image->ppx[0] = image->px;
  /* FIXME */
  image->ppx[1] = image->px + (image->pps[0] * image->h);
  image->ppx[2] = image->ppx[1] + (image->pps[1] * (image->h/2));
  image->ppx[3] = 0;
  return 1;
}

void kr_image_pool_destroy(kr_image_pool *image_pool) {
  return kr_pool_destroy(image_pool);
}
