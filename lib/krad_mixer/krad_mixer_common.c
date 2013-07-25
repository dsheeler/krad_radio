#include "krad_mixer_common.h"

void kr_mixer_path_info_to_ebml(kr_mixer_path_info *unit, kr_ebml *ebml) {

  int i;

  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, unit->name);
  kr_ebml2_pack_int8(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, unit->channels);
//  kr_ebml2_pack_int8(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, unit->direction);
//  kr_ebml2_pack_int8(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, unit->output_type);
/*
  if (unit->io_type == 0) {
    kr_ebml2_pack_string(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_TYPE, "Jack");
  } else {
    kr_ebml2_pack_string(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_TYPE, "Internal");
  }
*/
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

//  kr_ebml2_pack_int8(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_XMMS2, unit->has_xmms2);
//  if (unit->has_xmms2 == 1) {
//    kr_ebml2_pack_string(ebml, EBML_ID_KRAD_MIXER_PORTGROUP_XMMS2, unit->xmms2_ipc_path);
//  }

 /*
  for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
    kr_ebml2_pack_float(ebml, EBML_ID_KRAD_EFFECT_CONTROL, unit->eq.band[i].db);
    kr_ebml2_pack_float(ebml, EBML_ID_KRAD_EFFECT_CONTROL, unit->eq.band[i].bandwidth);
    kr_ebml2_pack_float(ebml, EBML_ID_KRAD_EFFECT_CONTROL, unit->eq.band[i].hz);
    // ("NOW hz is %f %f\n", krad_mixer_unit->eq.band[i].hz);
  }
  */

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
    case KR_MXR_AUX:
      return "AUX";
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
