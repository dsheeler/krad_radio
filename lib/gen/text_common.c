#include "gen.h"
#include "krad_v4l2_to_text.h"
int info_pack_to_text(char *text, uber_St *uber, int max);
typedef int (*info_pack_to_text_func)(char *text, void *st, int max);

int info_pack_to_text(char *text, uber_St *uber, int max) {
  const info_pack_to_text_func to_text_functions[4] = { kr_v4l2_state_to_text,
   kr_v4l2_mode_to_text, kr_v4l2_info_to_text,
   kr_v4l2_open_info_to_text};
  return to_text_functions[uber->type-1](text , uber->actual, max);
}

