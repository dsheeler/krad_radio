#include "gen.h"
#include "krad_ebmlx.h"
int info_pack_to_ebml(kr_ebml *ebml, uber_St *uber);
typedef int (*info_pack_to_ebml_func)(kr_ebml *ebml, void *st);

int info_pack_to_ebml(kr_ebml *ebml, uber_St *uber) {
  const info_pack_to_ebml_func to_ebml_functions[18] = {kr_mixer_channels_to_ebml,
  kr_mixer_control_to_ebml,kr_mixer_path_type_to_ebml,
  kr_mixer_adv_ctl_to_ebml,kr_mixer_info_to_ebml,
  kr_mixer_path_info_to_ebml,kr_sfx_control_to_ebml,
  kr_sfx_effect_type_to_ebml,kr_sfx_effect_control_to_ebml,
  kr_eq_band_info_to_ebml,kr_eq_info_to_ebml,
  kr_lowpass_info_to_ebml,kr_highpass_info_to_ebml,
  kr_analog_info_to_ebml,kr_v4l2_state_to_ebml,
  kr_v4l2_mode_to_ebml,kr_v4l2_info_to_ebml,
  kr_v4l2_open_info_to_ebml};
  return to_ebml_functions[uber->type-1](ebml , uber->actual);
}

