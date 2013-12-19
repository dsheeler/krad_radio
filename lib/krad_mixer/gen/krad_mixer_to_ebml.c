#include "krad_mixer_to_ebml.h"

int kr_mixer_channels_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  kr_mixer_channels *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_mixer_channels*)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_mixer_control_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  kr_mixer_control *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_mixer_control*)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_mixer_path_type_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  kr_mixer_path_type *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_mixer_path_type*)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_mixer_adv_ctl_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  kr_mixer_adv_ctl *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_mixer_adv_ctl*)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_mixer_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  res = 0;
  struct kr_mixer_info *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_mixer_info*)st;

  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->period_size);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->sample_rate);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->inputs);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->buses);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->outputs);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->frames);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->timecode);
  res += kr_ebml_pack_string(ebml, 0xe1, actual->clock);

  return res;
}

int kr_mixer_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int i;
  int res;
  res = 0;
  struct kr_mixer_path_info *actual;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_mixer_path_info*)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->name);
  res += kr_ebml_pack_string(ebml, 0xe1, actual->bus);
  res += kr_ebml_pack_string(ebml, 0xe1, actual->crossfade_group);
  uber.actual = &(actual->channels);
  uber.type = EBML_KR_MIXER_CHANNELS;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->type);
  uber.type = EBML_KR_MIXER_PATH_TYPE;
  res += info_pack_to_ebml(&ebml[res],&uber);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->fade);
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += kr_ebml_pack_float(ebml, 0xe1, actual->volume[i]);
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += kr_ebml_pack_int32(ebml, 0xe1, actual->map[i]);
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += kr_ebml_pack_int32(ebml, 0xe1, actual->mixmap[i]);
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += kr_ebml_pack_float(ebml, 0xe1, actual->rms[i]);
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += kr_ebml_pack_float(ebml, 0xe1, actual->peak[i]);
  }
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->delay);
  uber.actual = &(actual->lowpass);
  uber.type = EBML_KR_LOWPASS_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->highpass);
  uber.type = EBML_KR_HIGHPASS_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->analog);
  uber.type = EBML_KR_ANALOG_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->eq);
  uber.type = EBML_KR_EQ_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

