#include "krad_pool.h"

int main(int argc, char *argv[]) {

  int ret;
  kr_pool *pool;
  kr_pool_setup pool_setup;
  void *slice[128];

  memset(&slice, 0, sizeof(slice));

  pool_setup.shared = 1;
  pool_setup.size = 33;
  pool_setup.slices = 44;

  pool = kr_pool_create(&pool_setup);

  if (pool == NULL) {
    fprintf(stderr, "failed to make pool\n");
    return 1;
  }

  kr_pool_debug(pool);

  slice[0] = kr_pool_slice(pool);
  if (slice[0] == NULL) {
    fprintf(stderr, "failed to get slice\n");
    return 1;
  }
  kr_pool_debug(pool);
  slice[1] = kr_pool_slice(pool);
  if (slice[1] == NULL) {
    fprintf(stderr, "failed to get slice\n");
    return 1;
  }
  kr_pool_debug(pool);

  ret = kr_pool_recycle(pool, slice[1]);
  if (ret != 0) {
    fprintf(stderr, "failed to return slice %d\n", ret);
    return 1;
  }
  kr_pool_debug(pool);
  ret = kr_pool_recycle(pool, slice[0]);
  if (ret != 0) {
    fprintf(stderr, "failed to return slice %d\n", ret);
    return 1;
  }

  kr_pool_debug(pool);

  kr_pool_destroy(pool);

  return 0;
}
