#include "gen.h"
#include "krad_ebmlx.h"
int info_unpack_fr_ebml(kr_ebml *ebml, uber_St *uber);
typedef int (*info_unpack_fr_ebml_func)(kr_ebml *ebml, void *st);

int info_unpack_fr_ebml(kr_ebml *ebml, uber_St *uber) {
  const info_unpack_fr_ebml_func fr_ebml_functions[18] = {kr_mixer_channels_fr_ebml,
  kr_mixer_control_fr_ebml,kr_mixer_path_type_fr_ebml,
  kr_mixer_adv_ctl_fr_ebml,kr_mixer_info_fr_ebml,
  kr_mixer_path_info_fr_ebml,kr_sfx_control_fr_ebml,
  kr_sfx_effect_type_fr_ebml,kr_sfx_effect_control_fr_ebml,
  kr_eq_band_info_fr_ebml,kr_eq_info_fr_ebml,
  kr_lowpass_info_fr_ebml,kr_highpass_info_fr_ebml,
  kr_analog_info_fr_ebml,kr_v4l2_state_fr_ebml,
  kr_v4l2_mode_fr_ebml,kr_v4l2_info_fr_ebml,
  kr_v4l2_open_info_fr_ebml};
  return fr_ebml_functions[uber->type-1](ebml , uber->actual);
}

