#include "krad_mixer_common.h"

void krad_mixer_portgroup_rep_to_ebml2 (kr_portgroup_t *portgroup_rep, kr_ebml2_t *ebml) {

  unsigned char *portgroup;
  int i;

  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroup_rep->sysname);
  kr_ebml2_pack_int8 (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, portgroup_rep->channels);
  kr_ebml2_pack_int8 (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, portgroup_rep->direction);
  kr_ebml2_pack_int8 (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, portgroup_rep->output_type);  
  if (portgroup_rep->io_type == 0) {
    kr_ebml2_pack_string (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_TYPE, "Jack");
  } else {
    kr_ebml2_pack_string (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_TYPE, "Internal");
  }
  for (i = 0; i < portgroup_rep->channels; i++) {
    kr_ebml2_pack_float (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_VOLUME, portgroup_rep->volume[i]);      
  }
  for (i = 0; i < portgroup_rep->channels; i++) {
    kr_ebml2_pack_float (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_VOLUME, portgroup_rep->peak[i]);
  }
  for (i = 0; i < portgroup_rep->channels; i++) {
    kr_ebml2_pack_float (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_VOLUME, portgroup_rep->rms[i]);      
  }
 
  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_MIXBUS, portgroup_rep->mixbus);      

  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE_NAME, portgroup_rep->crossfade_group);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE, portgroup_rep->fade);  
  
  kr_ebml2_pack_int8 (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_XMMS2, portgroup_rep->has_xmms2);
  if (portgroup_rep->has_xmms2 == 1) {
    kr_ebml2_pack_string (ebml, EBML_ID_KRAD_MIXER_PORTGROUP_XMMS2, portgroup_rep->xmms2_ipc_path);
  }

 /* 
  for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
    kr_ebml2_pack_float (ebml, EBML_ID_KRAD_EFFECT_CONTROL, krad_mixer_portgroup_rep->eq.band[i].db);
    kr_ebml2_pack_float (ebml, EBML_ID_KRAD_EFFECT_CONTROL, krad_mixer_portgroup_rep->eq.band[i].bandwidth);
    kr_ebml2_pack_float (ebml, EBML_ID_KRAD_EFFECT_CONTROL, krad_mixer_portgroup_rep->eq.band[i].hz);
    // ("NOW hz is %f %f\n", krad_mixer_portgroup_rep->eq.band[i].hz); 
  }
  */

  kr_ebml2_pack_data (ebml, EBML_ID_KRAD_EFFECT_CONTROL, &portgroup_rep->eq, sizeof(kr_eq_rep_t));
  
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->lowpass.hz);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->lowpass.bandwidth);

  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->highpass.hz);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->highpass.bandwidth);

  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->analog.drive);
  kr_ebml2_pack_float (ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->analog.blend);
}

char *krad_mixer_channel_number_to_string (int channel) {

  switch ( channel ) {
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


char *portgroup_output_type_to_string (krad_mixer_output_t output_type) {
  switch (output_type) {
   case NOTOUTPUT:
      return "Notoutput";
    case DIRECT:
      return "Direct";
    case AUX:
      return "Aux";
  }
  return "Unknown";
}


char *portgroup_direction_to_string (krad_mixer_portgroup_direction_t direction) {
  switch (direction) {
   case OUTPUT:
      return "Output";
    case INPUT:
      return "Input";
    case MIX:
      return "Bus";
  }
  return "Unknown";
}

char *portgroup_control_to_string (kr_mixer_portgroup_control_t portgroup_control) {
  switch (portgroup_control) {
    case KR_VOLUME:
      return "volume";
    case KR_CROSSFADE:
      return "crossfade";
    case KR_PEAK:
      return "peak";
    case KR_CROSSFADE_GROUP:
      return "crossfade_group";
    case KR_XMMS2_IPC_PATH:
      return "xmms2_ipc_path";
  }
  return "Unknown";
}

char *effect_control_to_string (kr_mixer_effect_control_t effect_control) {

  switch (effect_control) {
    case DB:
      return "db";
    case HZ:
      return "hz";
    case BANDWIDTH:
      return "bw";
    case TYPE:
      return "type";
    case DRIVE:
      return "drive";
    case BLEND:
      return "blend";
  }
  return "Unknown";
}

char *effect_type_to_string (kr_effect_type_t effect_type) {

  switch (effect_type) {
    case KRAD_NOFX:
      return "Unknown";
    case KRAD_EQ:
      return "eq";
    case KRAD_LOWPASS:
      return "lowpass";
    case KRAD_HIGHPASS:
      return "highpass";
    case KRAD_ANALOG:
      return "analog";
  }
  return "Unknown";
}
