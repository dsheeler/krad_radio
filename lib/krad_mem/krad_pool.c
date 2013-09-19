#include "krad_pool.h"

#define KR_POOL_MAX 64

struct kr_pool {
  uint32_t slices;
  uint32_t active;
  int shared;
  size_t info_size;
  size_t slice_size;
  size_t total_size;
  uint64_t use;
  /*uint8_t type[KR_POOL_MAX];
  uint8_t state[KR_POOL_MAX];*/
  int type[KR_POOL_MAX];
  int state[KR_POOL_MAX];
  void *data;
  void *map;
};

int kr_pool_avail(kr_pool *pool) {
  if (pool == NULL) return -1;
  return pool->slices - pool->active;
}

int kr_pool_active(kr_pool *pool) {
  if (pool == NULL) return -1;
  return pool->active;
}

int kr_pool_slices(kr_pool *pool) {
  if (pool == NULL) return -1;
  return pool->slices;
}

void *kr_pool_iterate_active(kr_pool *pool, int *count) {

  uint64_t mask;

  if ((pool == NULL) || (count == NULL)) return NULL;
  if ((*count < 0) || (*count >= pool->slices)) return NULL;

  mask = 1;
  mask = mask << ((*count));
  while (*count < pool->slices) {
    if ((pool->use & mask) != 0) {
      return pool->data + (pool->slice_size * (*count)++);
    } else {
      (*count)++;
      mask = mask << 1;
    }
  }
  (*count) = 0;
  return NULL;
}
/*
void *kr_pool_iterate_state(kr_pool *pool, int *count) {

  return NULL;
}

void *kr_pool_iterate_type(kr_pool *pool, int *count) {

  return NULL;
}

void *kr_pool_iterate_type_state(kr_pool *pool, int *count) {
  return NULL;
}
*/
int kr_pool_recycle(kr_pool *pool, void *slice) {

  int i;
  uint64_t mask;

  if ((pool == NULL) || (slice == NULL)) return -2;

  mask = 1;
  for (i = 0; i < pool->slices; i++) {
    if (((pool->use & mask) != 0)
        && (slice == (pool->data + (pool->slice_size * i)))) {
      pool->use = pool->use ^ mask;
      pool->active--;
      return 0;
    }
    mask = mask << 1;
  }
  return -1;
}

void *kr_pool_slice(kr_pool *pool) {

  int i;
  uint64_t mask;

  if (pool == NULL) return NULL;

  mask = 1;
  for (i = 0; i < pool->slices; i++) {
    if ((pool->use & mask) == 0) {
      pool->use = pool->use | mask;
      pool->active++;
      return pool->data + (pool->slice_size * i);
    }
    mask = mask << 1;
  }
  return NULL;
}

void kr_pool_debug(kr_pool *pool) {
  if (pool == NULL) return;
  printf("pool slices: %d\n", kr_pool_slices(pool));
  printf("pool active: %d\n", kr_pool_active(pool));
  printf("pool avail: %d\n", kr_pool_avail(pool));
  printf("pool use: %"PRIu64"\n", pool->use);
  printf("pool info size: %zu\n", pool->info_size);
  printf("pool slice size: %zu\n", pool->slice_size);
  printf("pool total size: %zu\n\n", pool->total_size);
}

void kr_pool_destroy(kr_pool *pool) {
  if (pool == NULL) return;
  munmap(pool->map, pool->total_size);
}

kr_pool *kr_pool_create(kr_pool_setup *setup) {

  char filename[] = "/tmp/test-shm-XXXXXX";
  int fd;
  int flags;
  kr_pool pool;

  if (setup == NULL) return NULL;
  if (setup->slices == 0) return NULL;
  // temp fixed size bitmap
  if (setup->slices > KR_POOL_MAX) return NULL;
  if (setup->size == 0) return NULL;
  memset(&pool, 0, sizeof(kr_pool));
  pool.info_size = sizeof(kr_pool);
  pool.info_size = pool.info_size + (KR_CACHELINE % pool.info_size);
  pool.slices = setup->slices;
  pool.slice_size = setup->size + (KR_CACHELINE % setup->size);
  pool.total_size = (pool.slices * pool.slice_size) + pool.info_size;
  pool.total_size = pool.total_size + (KR_PAGESIZE % pool.total_size);
  if (setup->shared != 0) {
    pool.shared = 1;
    flags = MAP_SHARED;
  } else {
    flags = MAP_PRIVATE;
    pool.shared = 0;
  }
  fd = mkstemp(filename);
  if (fd < 0) {
    printke("open %s failed: %m\n", filename);
    return NULL;
  }
  if (ftruncate(fd, pool.total_size) < 0) {
    printke("ftruncate failed: %m\n");
    close(fd);
    return NULL;
  }
  pool.map = mmap(NULL, pool.total_size, PROT_READ | PROT_WRITE, flags, fd, 0);
  unlink(filename);
  if (pool.map == MAP_FAILED) {
    printke("mmap failed\n");
    close(fd);
    return NULL;
  }
  close(fd);
  pool.data = pool.map + pool.info_size;
  memcpy(pool.map, &pool, sizeof(kr_pool));
  return (kr_pool *)pool.map;
}
