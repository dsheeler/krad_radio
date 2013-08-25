#include "krad_mixer_common.h"

void kr_mixer_path_info_to_ebml(kr_mixer_path_info *unit, kr_ebml *ebml) {

  int i;

  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, unit->name);
  kr_ebml2_pack_int8(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, unit->channels);
//  kr_ebml2_pack_int8(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, unit->direction);
  for (i = 0; i < unit->channels; i++) {
    kr_ebml2_pack_float(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_VOLUME, unit->volume[i]);
  }
  for (i = 0; i < unit->channels; i++) {
    kr_ebml2_pack_float(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_VOLUME, unit->peak[i]);
  }
  for (i = 0; i < unit->channels; i++) {
    kr_ebml2_pack_float(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_VOLUME, unit->rms[i]);
  }
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_MIXBUS, unit->bus);
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE_NAME, unit->crossfade_group);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE, unit->fade);
  kr_ebml2_pack_data(ebml, EBML_ID_KRAD_EFFECT_CONTROL, &unit->eq, sizeof(kr_eq_info));
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_EFFECT_CONTROL, unit->lowpass.hz);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_EFFECT_CONTROL, unit->lowpass.bw);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_EFFECT_CONTROL, unit->highpass.hz);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_EFFECT_CONTROL, unit->highpass.bw);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_EFFECT_CONTROL, unit->analog.drive);
  kr_ebml2_pack_float(ebml, EBML_ID_KRAD_EFFECT_CONTROL, unit->analog.blend);
}

char *kr_mixer_channeltostr(int channel) {
  switch (channel) {
    case 0:
      return "Left";
    case 1:
      return "Right";
    case 2:
      return "RearLeft";
    case 3:
      return "RearRight";
    case 4:
      return "Center";
    case 5:
      return "Sub";
    case 6:
      return "BackLeft";
    case 7:
      return "BackRight";
    default:
      return "Unknown";
  }
}

char *kr_mixer_pathtypetostr(kr_mixer_path_type type) {
  switch (type) {
   case KR_MXR_INPUT:
      return "INPUT";
    case KR_MXR_BUS:
      return "BUS";
    case KR_MXR_OUTPUT:
      return "OUTPUT";
  }
  return "Unknown";
}

char *kr_mixer_ctltostr(kr_mixer_control control) {
  switch (control) {
    case KR_VOLUME:
      return "volume";
    case KR_CROSSFADE:
      return "crossfade";
    case KR_PEAK:
      return "peak";
    case KR_CROSSFADE_GROUP:
      return "crossfade_group";
  }
  return "Unknown";
}
