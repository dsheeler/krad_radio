#include "gen.h"
#include "krad_ebmlx.h"
int info_unpack_fr_ebml(kr_ebml *ebml, uber_St *uber);
typedef int (*info_unpack_fr_ebml_func)(kr_ebml *ebml, void *st);

int info_unpack_fr_ebml(kr_ebml *ebml, uber_St *uber) {
  const info_unpack_fr_ebml_func fr_ebml_functions[4] = { kr_v4l2_state_fr_ebml,
   kr_v4l2_mode_fr_ebml, kr_v4l2_info_fr_ebml,
   kr_v4l2_open_info_fr_ebml};
  return fr_ebml_functions[uber->type-1](ebml , uber->actual);
}

