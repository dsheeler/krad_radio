#include "krad_mixer_common.h"

void krad_mixer_portgroup_rep_to_ebml (kr_portgroup_t *portgroup_rep, krad_ebml_t *krad_ebml) {

  uint64_t portgroup;
  int i;

  krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP, &portgroup);

  krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_NAME, portgroup_rep->sysname);
  krad_ebml_write_int8 (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, portgroup_rep->channels);
  krad_ebml_write_int8 (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, portgroup_rep->direction);
  krad_ebml_write_int8 (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CHANNELS, portgroup_rep->output_type);  
  if (portgroup_rep->io_type == 0) {
    krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_TYPE, "Jack");
  } else {
    krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_TYPE, "Internal");
  }
  for (i = 0; i < portgroup_rep->channels; i++) {
    krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_VOLUME, portgroup_rep->volume[i]);      
  }
  for (i = 0; i < portgroup_rep->channels; i++) {
    krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_VOLUME, portgroup_rep->peak[i]);
  }
  for (i = 0; i < portgroup_rep->channels; i++) {
    krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_VOLUME, portgroup_rep->rms[i]);      
  }
 
  krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_MIXBUS, portgroup_rep->mixbus);      

  krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE_NAME, portgroup_rep->crossfade_group);
  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_CROSSFADE, portgroup_rep->fade);  
  
  krad_ebml_write_int8 (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_XMMS2, portgroup_rep->has_xmms2);
  if (portgroup_rep->has_xmms2 == 1) {
    krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_MIXER_PORTGROUP_XMMS2, portgroup_rep->xmms2_ipc_path);
  }

 /* 
  for (i = 0; i < KRAD_EQ_MAX_BANDS; i++) {
    krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, krad_mixer_portgroup_rep->eq.band[i].db);
    krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, krad_mixer_portgroup_rep->eq.band[i].bandwidth);
    krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, krad_mixer_portgroup_rep->eq.band[i].hz);
    // ("NOW hz is %f %f\n", krad_mixer_portgroup_rep->eq.band[i].hz); 
  }
  */

  krad_ebml_write_data (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, &portgroup_rep->eq, sizeof(kr_eq_rep_t));
  
  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->lowpass.hz);
  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->lowpass.bandwidth);

  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->highpass.hz);
  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->highpass.bandwidth);

  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->analog.drive);
  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_EFFECT_CONTROL, portgroup_rep->analog.blend);
  
  krad_ebml_finish_element (krad_ebml, portgroup);
  
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
