#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <assert.h>
#include <signal.h>

#include "krad_system.h"

#ifndef KR_MEM_POOL_H
#define KR_MEM_POOL_H

#define KR_CACHELINE 64
#define KR_PAGESIZE 4096

typedef struct kr_pool kr_pool;
typedef struct kr_pool_setup kr_pool_setup;

struct kr_pool_setup {
  uint32_t slices;
  size_t size;
  int shared;
  void *overlay;
  size_t overlay_sz;
};

void *kr_pool_iterate_active(kr_pool *pool, int *count);

int kr_pool_slice_ref(kr_pool *pool, void *slice);
int kr_pool_recycle(kr_pool *pool, void *slice);
void *kr_pool_slice(kr_pool *pool);

void *kr_pool_slice_num(kr_pool *pool, int num);
void kr_pool_debug(kr_pool *pool);
int kr_pool_avail(kr_pool *pool);
int kr_pool_active(kr_pool *pool);
int kr_pool_slices(kr_pool *pool);
int kr_pool_get_overlay(kr_pool *pool, void *overlay);
kr_pool *kr_pool_create(kr_pool_setup *setup);
void kr_pool_destroy(kr_pool *pool);

#endif
