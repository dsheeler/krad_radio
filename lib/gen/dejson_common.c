#include "gen.h"
int info_unpack_fr_json(char *json, uber_St *uber);
typedef int (*info_unpack_fr_json_func)(char *json, void *st);

int info_unpack_fr_json(char *json, uber_St *uber) {
  const info_unpack_fr_json_func fr_json_functions[18] = {kr_mixer_channels_fr_json,
  kr_mixer_control_fr_json,kr_mixer_path_type_fr_json,
  kr_mixer_adv_ctl_fr_json,kr_mixer_info_fr_json,
  kr_mixer_path_info_fr_json,kr_sfx_control_fr_json,
  kr_sfx_effect_type_fr_json,kr_sfx_effect_control_fr_json,
  kr_eq_band_info_fr_json,kr_eq_info_fr_json,
  kr_lowpass_info_fr_json,kr_highpass_info_fr_json,
  kr_analog_info_fr_json,kr_v4l2_state_fr_json,
  kr_v4l2_mode_fr_json,kr_v4l2_info_fr_json,
  kr_v4l2_open_info_fr_json};
  return fr_json_functions[uber->type-1](json , uber->actual);
}

