#include "gen.h"
#include "krad_ebmlx.h"
int info_pack_to_ebml(kr_ebml *ebml, uber_St *uber);
typedef int (*info_pack_to_ebml_func)(kr_ebml *ebml, void *st);

int info_pack_to_ebml(kr_ebml *ebml, uber_St *uber) {
  const info_pack_to_ebml_func to_ebml_functions[4] = { kr_v4l2_state_to_ebml,
   kr_v4l2_mode_to_ebml, kr_v4l2_info_to_ebml,
   kr_v4l2_open_info_to_ebml};
  return to_ebml_functions[uber->type-1](ebml , uber->actual);
}

