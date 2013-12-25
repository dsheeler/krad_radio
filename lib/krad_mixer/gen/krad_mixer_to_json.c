#include "krad_mixer_to_json.h"

int kr_mixer_channels_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_mixer_channels *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_mixer_channels *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_mixer_control_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_mixer_control *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_mixer_control *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_mixer_path_type_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_mixer_path_type *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_mixer_path_type *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_mixer_adv_ctl_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_mixer_adv_ctl *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_mixer_adv_ctl *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_mixer_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_mixer_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_mixer_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"period_size\" : %u,",actual->period_size);
  res += snprintf(&json[res],max-res,"\"sample_rate\" : %u,",actual->sample_rate);
  res += snprintf(&json[res],max-res,"\"inputs\" : %u,",actual->inputs);
  res += snprintf(&json[res],max-res,"\"buses\" : %u,",actual->buses);
  res += snprintf(&json[res],max-res,"\"outputs\" : %u,",actual->outputs);
  res += snprintf(&json[res],max-res,"\"frames\" : %ju,",actual->frames);
  res += snprintf(&json[res],max-res,"\"timecode\" : %ju,",actual->timecode);
  res += snprintf(&json[res],max-res,"\"clock\" : \"%s\"",actual->clock);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_mixer_path_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int i;
  int res;
  struct kr_mixer_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_mixer_path_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"name\" : \"%s\",",actual->name);
  res += snprintf(&json[res],max-res,"\"bus\" : \"%s\",",actual->bus);
  res += snprintf(&json[res],max-res,"\"crossfade_group\" : \"%s\",",actual->crossfade_group);
  res += snprintf(&json[res],max-res,"\"channels\": ");
  uber.actual = &(actual->channels);
  uber.type = JSON_KR_MIXER_CHANNELS;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"type\": ");
  uber.actual = &(actual->type);
  uber.type = JSON_KR_MIXER_PATH_TYPE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"fade\" : %0.2f,",actual->fade);
  res += snprintf(&json[res],max-res,"\"volume\" : [");
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&json[res],max-res,"%0.2f",actual->volume[i]);
    if (i != (KR_MXR_MAX_CHANNELS - 1)) {
      res += snprintf(&json[res],max-res,",");
    }
  }
  res += snprintf(&json[res],max-res,"]");
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"map\" : [");
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&json[res],max-res,"%d",actual->map[i]);
    if (i != (KR_MXR_MAX_CHANNELS - 1)) {
      res += snprintf(&json[res],max-res,",");
    }
  }
  res += snprintf(&json[res],max-res,"]");
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"mixmap\" : [");
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&json[res],max-res,"%d",actual->mixmap[i]);
    if (i != (KR_MXR_MAX_CHANNELS - 1)) {
      res += snprintf(&json[res],max-res,",");
    }
  }
  res += snprintf(&json[res],max-res,"]");
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"rms\" : [");
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&json[res],max-res,"%0.2f",actual->rms[i]);
    if (i != (KR_MXR_MAX_CHANNELS - 1)) {
      res += snprintf(&json[res],max-res,",");
    }
  }
  res += snprintf(&json[res],max-res,"]");
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"peak\" : [");
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    res += snprintf(&json[res],max-res,"%0.2f",actual->peak[i]);
    if (i != (KR_MXR_MAX_CHANNELS - 1)) {
      res += snprintf(&json[res],max-res,",");
    }
  }
  res += snprintf(&json[res],max-res,"]");
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"delay\" : %d,",actual->delay);
  res += snprintf(&json[res],max-res,"\"lowpass\": ");
  uber.actual = &(actual->lowpass);
  uber.type = JSON_KR_LOWPASS_INFO;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"highpass\": ");
  uber.actual = &(actual->highpass);
  uber.type = JSON_KR_HIGHPASS_INFO;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"analog\": ");
  uber.actual = &(actual->analog);
  uber.type = JSON_KR_ANALOG_INFO;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"eq\": ");
  uber.actual = &(actual->eq);
  uber.type = JSON_KR_EQ_INFO;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

