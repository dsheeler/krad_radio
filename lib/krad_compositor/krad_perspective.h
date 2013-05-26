#include <math.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct krad_perspective_St krad_perspective_t;
typedef struct krad_perspective_St kr_perspective_t;
typedef struct krad_position_St krad_position_t;
typedef struct krad_position_St kr_coord_t;

struct krad_position_St {
  double x;
  double y;
};

struct krad_perspective_St {
  uint32_t width;
  uint32_t height;
  krad_position_t tl;
  krad_position_t tr;
  krad_position_t bl;
  krad_position_t br;
  int32_t *map;  
};

void kr_perspective (kr_perspective_t *perspective,
                     uint32_t *out,
                     uint32_t *in);

int32_t kr_perspective_destroy (kr_perspective_t **krad_perspective);
kr_perspective_t *kr_perspective_create (uint32_t width, uint32_t height);
