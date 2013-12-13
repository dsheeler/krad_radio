#include "gen.h"
#include "krad_v4l2_to_json.h"
int info_pack_to_json(char *json, uber_St *uber, int max);
typedef int (*info_pack_to_json_func)(char *json, void *st, int max);

int info_pack_to_json(char *json, uber_St *uber, int max) {
  const info_pack_to_json_func to_json_functions[4] = { kr_v4l2_state_to_json,
   kr_v4l2_mode_to_json, kr_v4l2_info_to_json,
   kr_v4l2_open_info_to_json};
  return to_json_functions[uber->type-1](json , uber->actual, max);
}

