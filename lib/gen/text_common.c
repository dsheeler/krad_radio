#include "gen.h"
int info_pack_to_text(char *text, uber_St *uber, int max);
typedef int (*info_pack_to_text_func)(char *text, void *st, int max);

int info_pack_to_text(char *text, uber_St *uber, int max) {
  const info_pack_to_text_func to_text_functions[18] = { kr_mixer_channels_to_text,
   kr_mixer_control_to_text, kr_mixer_path_type_to_text,
   kr_mixer_adv_ctl_to_text, kr_mixer_info_to_text,
   kr_mixer_path_info_to_text, kr_sfx_control_to_text,
   kr_sfx_effect_type_to_text, kr_sfx_effect_control_to_text,
   kr_eq_band_info_to_text, kr_eq_info_to_text,
   kr_lowpass_info_to_text, kr_highpass_info_to_text,
   kr_analog_info_to_text, kr_v4l2_state_to_text,
   kr_v4l2_mode_to_text, kr_v4l2_info_to_text,
   kr_v4l2_open_info_to_text};
  return to_text_functions[uber->type-1](text , uber->actual, max);
}

