#ifndef KRAD_IMAGE_POOL_H
#define KRAD_IMAGE_POOL_H

#include "krad_system.h"
#include "krad_pool.h"
#include "krad_av.h"

typedef struct kr_pool kr_image_pool;

kr_image_pool *kr_image_pool_create(kr_image *image, size_t len);
int kr_image_pool_getimage(kr_image_pool *image_pool, kr_image *image);
void kr_image_pool_destroy(kr_image_pool *image_pool);

#endif
