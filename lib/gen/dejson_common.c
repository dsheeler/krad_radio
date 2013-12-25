#include "gen.h"
int info_unpack_fr_json(char *json, uber_St *uber);
typedef int (*info_unpack_fr_json_func)(char *json, void *st);

int info_unpack_fr_json(char *json, uber_St *uber) {
  const info_unpack_fr_json_func fr_json_functions[4] = {kr_v4l2_state_fr_json,
  kr_v4l2_mode_fr_json,kr_v4l2_info_fr_json,
  kr_v4l2_open_info_fr_json};
  return fr_json_functions[uber->type-1](json , uber->actual);
}

