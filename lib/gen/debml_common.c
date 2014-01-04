#include "gen.h"
#include "krad_ebmlx.h"
int info_unpack_fr_ebml(kr_ebml *ebml, uber_St *uber);
typedef int (*info_unpack_fr_ebml_func)(kr_ebml *ebml, void *st);

int info_unpack_fr_ebml(kr_ebml *ebml, uber_St *uber) {
  const info_unpack_fr_ebml_func fr_ebml_functions[60] = {kr_alsa_info_fr_ebml,
  kr_alsa_path_info_fr_ebml,kr_compositor_path_type_fr_ebml,
  kr_compositor_subunit_type_fr_ebml,kr_vector_type_fr_ebml,
  kr_compositor_control_fr_ebml,kr_compositor_controls_fr_ebml,
  kr_sprite_info_fr_ebml,kr_text_info_fr_ebml,
  kr_vector_info_fr_ebml,kr_compositor_path_info_fr_ebml,
  kr_compositor_info_fr_ebml,kr_pos_fr_ebml,
  kr_perspective_view_fr_ebml,kr_perspective_fr_ebml,
  krad_codec_t_fr_ebml,krad_container_type_t_fr_ebml,
  kr_decklink_info_fr_ebml,kr_decklink_path_info_fr_ebml,
  kr_jack_direction_fr_ebml,kr_jack_state_fr_ebml,
  kr_jack_setup_info_fr_ebml,kr_jack_info_fr_ebml,
  kr_jack_path_info_fr_ebml,kr_mixer_channels_fr_ebml,
  kr_mixer_control_fr_ebml,kr_mixer_path_type_fr_ebml,
  kr_mixer_adv_ctl_fr_ebml,kr_mixer_info_fr_ebml,
  kr_mixer_path_info_fr_ebml,kr_sfx_control_fr_ebml,
  kr_sfx_effect_type_fr_ebml,kr_sfx_effect_control_fr_ebml,
  kr_eq_band_info_fr_ebml,kr_eq_info_fr_ebml,
  kr_lowpass_info_fr_ebml,kr_highpass_info_fr_ebml,
  kr_analog_info_fr_ebml,kr_adapter_path_direction_fr_ebml,
  kr_adapter_api_fr_ebml,kr_adapter_api_info_fr_ebml,
  kr_adapter_api_path_info_fr_ebml,kr_adapter_info_fr_ebml,
  kr_adapter_path_info_fr_ebml,krad_link_av_mode_t_fr_ebml,
  kr_txpdr_su_type_t_fr_ebml,krad_link_transport_mode_t_fr_ebml,
  kr_transponder_path_io_type_fr_ebml,kr_transponder_info_fr_ebml,
  kr_transponder_path_io_path_info_fr_ebml,kr_transponder_path_io_info_fr_ebml,
  kr_transponder_path_info_fr_ebml,kr_v4l2_state_fr_ebml,
  kr_v4l2_mode_fr_ebml,kr_v4l2_info_fr_ebml,
  kr_v4l2_open_info_fr_ebml,kr_wayland_info_fr_ebml,
  kr_wayland_path_info_fr_ebml,kr_x11_info_fr_ebml,
  kr_x11_path_info_fr_ebml};
  return fr_ebml_functions[uber->type-1](ebml , uber->actual);
}

