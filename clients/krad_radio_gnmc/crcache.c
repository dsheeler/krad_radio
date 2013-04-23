#include "crcache.h"

static kr_crate_t *crcache_read (cr_cache_t *cache, uint32_t idx) {
  return tommy_array_get (cache->crates,idx);
}

static void crcache_grow (cr_cache_t *cache) {
  cache->alloc_exp++;
  tommy_array_grow (cache->crates,pow (2,cache->alloc_exp));
  return;
}

static void crcache_write (cr_cache_t *cache, kr_crate_t *crate, uint32_t idx) {
  kr_crate_t *cached_crate; 
  if (idx >= pow (2,cache->alloc_exp)) {
    crcache_grow (cache);
  }
  cached_crate = malloc (sizeof (kr_crate_t));
  memcpy (cached_crate,crate,sizeof (kr_crate_t));
  tommy_array_set (cache->crates,idx,cached_crate);
  return;
}

static void crcache_free (cr_cache_t *cache) {
  uint32_t size;
  uint32_t i;

  if (cache != NULL) {
    size = tommy_array_size (cache->crates);
    for (i=0;i<size;i++) {
      free (crcache_read (cache,i));
    }
    tommy_array_done (cache->crates);
    free (cache->crates);
    free (cache);
  }
  return;
}

cr_cache_t *crcache_init (void) {
  cr_cache_t *cache;
  tommy_array *array;

  cache = calloc (1,sizeof (cr_cache_t));
  array = calloc (1,sizeof (tommy_array));

  tommy_array_init (array);

  cache->crates = array;
  cache->alloc_exp = TOMMY_ARRAY_BIT;
  cache->read = crcache_read;
  cache->write = crcache_write;
  cache->free = crcache_free;

  tommy_array_grow (cache->crates,pow (2,cache->alloc_exp));

  return cache;
}
