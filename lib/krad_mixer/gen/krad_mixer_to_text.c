#include "krad_mixer_to_text.h"

int kr_mixer_channels_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  kr_mixer_channels *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_mixer_channels*)st;

  res += snprintf(&text[res],max-res,"kr_mixer_channels : %u \n",*actual);

  return res;
}

int kr_mixer_control_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  kr_mixer_control *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_mixer_control*)st;

  res += snprintf(&text[res],max-res,"kr_mixer_control : %u \n",*actual);

  return res;
}

int kr_mixer_path_type_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  kr_mixer_path_type *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_mixer_path_type*)st;

  res += snprintf(&text[res],max-res,"kr_mixer_path_type : %u \n",*actual);

  return res;
}

int kr_mixer_adv_ctl_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  kr_mixer_adv_ctl *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_mixer_adv_ctl*)st;

  res += snprintf(&text[res],max-res,"kr_mixer_adv_ctl : %u \n",*actual);

  return res;
}

int kr_mixer_info_to_text(char *text, void *st, int32_t max) {
  int res;
  res = 0;
  struct kr_mixer_info *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_mixer_info*)st;

  res += snprintf(&text[res],max-res,"period_size : %u \n",actual->period_size);
  res += snprintf(&text[res],max-res,"sample_rate : %u \n",actual->sample_rate);
  res += snprintf(&text[res],max-res,"inputs : %u \n",actual->inputs);
  res += snprintf(&text[res],max-res,"buses : %u \n",actual->buses);
  res += snprintf(&text[res],max-res,"outputs : %u \n",actual->outputs);
  res += snprintf(&text[res],max-res,"frames : %ju \n",actual->frames);
  res += snprintf(&text[res],max-res,"timecode : %ju \n",actual->timecode);
  res += snprintf(&text[res],max-res,"clock : %s \n",actual->clock);

  return res;
}

int kr_mixer_path_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int i;
  int res;
  res = 0;
  struct kr_mixer_path_info *actual;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_mixer_path_info*)st;

  res += snprintf(&text[res],max-res,"name : %s \n",actual->name);
  res += snprintf(&text[res],max-res,"bus : %s \n",actual->bus);
  res += snprintf(&text[res],max-res,"crossfade_group : %s \n",actual->crossfade_group);
  uber.actual = &(actual->channels);
  uber.type = TEXT_KR_MIXER_CHANNELS;
  res += info_pack_to_text(&text[res],&uber,max-res);
  uber.actual = &(actual->type);
  uber.type = TEXT_KR_MIXER_PATH_TYPE;
  res += info_pack_to_text(&text[res],&uber,max-res);
  res += snprintf(&text[res],max-res,"fade : %0.2f \n",actual->fade);
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&text[res],max-res,"volume[%d] : %0.2f \n",actual->volume[i],i);
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&text[res],max-res,"map[%d] : %d \n",actual->map[i],i);
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&text[res],max-res,"mixmap[%d] : %d \n",actual->mixmap[i],i);
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&text[res],max-res,"rms[%d] : %0.2f \n",actual->rms[i],i);
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&text[res],max-res,"peak[%d] : %0.2f \n",actual->peak[i],i);
  }
  res += snprintf(&text[res],max-res,"delay : %d \n",actual->delay);
  uber.actual = &(actual->lowpass);
  uber.type = TEXT_KR_LOWPASS_INFO;
  res += info_pack_to_text(&text[res],&uber,max-res);
  uber.actual = &(actual->highpass);
  uber.type = TEXT_KR_HIGHPASS_INFO;
  res += info_pack_to_text(&text[res],&uber,max-res);
  uber.actual = &(actual->analog);
  uber.type = TEXT_KR_ANALOG_INFO;
  res += info_pack_to_text(&text[res],&uber,max-res);
  uber.actual = &(actual->eq);
  uber.type = TEXT_KR_EQ_INFO;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

