#include "gen.h"
#include "krad_ebmlx.h"
int info_pack_to_ebml(kr_ebml *ebml, uber_St *uber);
typedef int (*info_pack_to_ebml_func)(kr_ebml *ebml, void *st);

int info_pack_to_ebml(kr_ebml *ebml, uber_St *uber) {
  const info_pack_to_ebml_func to_ebml_functions[62] = {kr_alsa_info_to_ebml,
  kr_alsa_path_info_to_ebml,kr_compositor_path_type_to_ebml,
  kr_compositor_subunit_type_to_ebml,kr_vector_type_to_ebml,
  kr_compositor_control_to_ebml,kr_compositor_controls_to_ebml,
  kr_sprite_info_to_ebml,kr_text_info_to_ebml,
  kr_vector_info_to_ebml,kr_compositor_path_info_to_ebml,
  kr_compositor_info_to_ebml,kr_pos_to_ebml,
  kr_perspective_view_to_ebml,kr_perspective_to_ebml,
  krad_codec_t_to_ebml,krad_container_type_t_to_ebml,
  kr_decklink_info_to_ebml,kr_decklink_path_info_to_ebml,
  krad_io_mode_t_to_ebml,kr_io2_St_to_ebml,
  kr_jack_direction_to_ebml,kr_jack_state_to_ebml,
  kr_jack_setup_info_to_ebml,kr_jack_info_to_ebml,
  kr_jack_path_info_to_ebml,kr_mixer_channels_to_ebml,
  kr_mixer_control_to_ebml,kr_mixer_path_type_to_ebml,
  kr_mixer_adv_ctl_to_ebml,kr_mixer_info_to_ebml,
  kr_mixer_path_info_to_ebml,kr_sfx_control_to_ebml,
  kr_sfx_effect_type_to_ebml,kr_sfx_effect_control_to_ebml,
  kr_eq_band_info_to_ebml,kr_eq_info_to_ebml,
  kr_lowpass_info_to_ebml,kr_highpass_info_to_ebml,
  kr_analog_info_to_ebml,kr_adapter_path_direction_to_ebml,
  kr_adapter_api_to_ebml,kr_adapter_api_info_to_ebml,
  kr_adapter_api_path_info_to_ebml,kr_adapter_info_to_ebml,
  kr_adapter_path_info_to_ebml,krad_link_av_mode_t_to_ebml,
  kr_txpdr_su_type_t_to_ebml,krad_link_transport_mode_t_to_ebml,
  kr_transponder_path_io_type_to_ebml,kr_transponder_info_to_ebml,
  kr_transponder_path_io_path_info_to_ebml,kr_transponder_path_io_info_to_ebml,
  kr_transponder_path_info_to_ebml,kr_v4l2_state_to_ebml,
  kr_v4l2_mode_to_ebml,kr_v4l2_info_to_ebml,
  kr_v4l2_open_info_to_ebml,kr_wayland_info_to_ebml,
  kr_wayland_path_info_to_ebml,kr_x11_info_to_ebml,
  kr_x11_path_info_to_ebml};
  return to_ebml_functions[uber->type-1](ebml , uber->actual);
}

