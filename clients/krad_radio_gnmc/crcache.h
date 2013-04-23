#ifndef KR_GNMC_CRCACHE_H
#define KR_GNMC_CRCACHE_H
typedef struct crates_cache_St cr_cache_t;
#include "gnmc.h"
#include "tds/tommyarray.h"

struct crates_cache_St {
  tommy_array *crates;
  uint8_t alloc_exp; /* Currently allocated memory is 2^alloc_size_exp */
  kr_crate_t* (*read)(cr_cache_t *cache, uint32_t idx);
  void (*write)(cr_cache_t *cache, kr_crate_t *crate, uint32_t idx);
  void (*free)(cr_cache_t *cache);
};

cr_cache_t *crcache_init (void);
#endif