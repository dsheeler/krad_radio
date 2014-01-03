#include "krad_mixer_from_json.h"

int kr_mixer_channels_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  jsmn_init(&parser);
  err = jsmn_parse(&parser,json,tokens,512);
  ntokens = parser.toknext;

  k = 0;

  if (err != JSMN_SUCCESS || ntokens < 3) {
    return -1;
  }

  if (tokens[k].type != JSMN_OBJECT) {
    return -1;
  }

  k++;


  return res;
}

int kr_mixer_control_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  jsmn_init(&parser);
  err = jsmn_parse(&parser,json,tokens,512);
  ntokens = parser.toknext;

  k = 0;

  if (err != JSMN_SUCCESS || ntokens < 3) {
    return -1;
  }

  if (tokens[k].type != JSMN_OBJECT) {
    return -1;
  }

  k++;


  return res;
}

int kr_mixer_path_type_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  jsmn_init(&parser);
  err = jsmn_parse(&parser,json,tokens,512);
  ntokens = parser.toknext;

  k = 0;

  if (err != JSMN_SUCCESS || ntokens < 3) {
    return -1;
  }

  if (tokens[k].type != JSMN_OBJECT) {
    return -1;
  }

  k++;


  return res;
}

int kr_mixer_adv_ctl_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  jsmn_init(&parser);
  err = jsmn_parse(&parser,json,tokens,512);
  ntokens = parser.toknext;

  k = 0;

  if (err != JSMN_SUCCESS || ntokens < 3) {
    return -1;
  }

  if (tokens[k].type != JSMN_OBJECT) {
    return -1;
  }

  k++;


  return res;
}

int kr_mixer_info_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  struct kr_mixer_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_mixer_info *)st;

  jsmn_init(&parser);
  err = jsmn_parse(&parser,json,tokens,512);
  ntokens = parser.toknext;

  k = 0;

  if (err != JSMN_SUCCESS || ntokens < 3) {
    return -1;
  }

  if (tokens[k].type != JSMN_OBJECT) {
    return -1;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -1;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"period_size",11)) {
    return -1;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -1;
  }

  json[tokens[k].end] = '\0';
  actual->period_size = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -2;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"sample_rate",11)) {
    return -2;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -2;
  }

  json[tokens[k].end] = '\0';
  actual->sample_rate = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -3;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"inputs",6)) {
    return -3;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -3;
  }

  json[tokens[k].end] = '\0';
  actual->inputs = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -4;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"buses",5)) {
    return -4;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -4;
  }

  json[tokens[k].end] = '\0';
  actual->buses = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -5;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"outputs",7)) {
    return -5;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -5;
  }

  json[tokens[k].end] = '\0';
  actual->outputs = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -6;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"frames",6)) {
    return -6;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -6;
  }

  json[tokens[k].end] = '\0';
  actual->frames = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -7;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"timecode",8)) {
    return -7;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -7;
  }

  json[tokens[k].end] = '\0';
  actual->timecode = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -8;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"clock",5)) {
    return -8;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -8;
  }

  json[tokens[k].end] = '\0';
  snprintf(actual->clock, sizeof(actual->clock), "%s", &json[tokens[k].start]);
  k++;

  res = k;

  return res;
}

int kr_mixer_path_info_fr_json(char *json, void *st) {
  uber_St uber;
  int i;
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  struct kr_mixer_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_mixer_path_info *)st;

  jsmn_init(&parser);
  err = jsmn_parse(&parser,json,tokens,512);
  ntokens = parser.toknext;

  k = 0;

  if (err != JSMN_SUCCESS || ntokens < 3) {
    return -1;
  }

  if (tokens[k].type != JSMN_OBJECT) {
    return -1;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -1;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"name",4)) {
    return -1;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -1;
  }

  json[tokens[k].end] = '\0';
  snprintf(actual->name, sizeof(actual->name), "%s", &json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -2;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"bus",3)) {
    return -2;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -2;
  }

  json[tokens[k].end] = '\0';
  snprintf(actual->bus, sizeof(actual->bus), "%s", &json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -3;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"crossfade_group",15)) {
    return -3;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -3;
  }

  json[tokens[k].end] = '\0';
  snprintf(actual->crossfade_group, sizeof(actual->crossfade_group), "%s", &json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -4;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"channels",8)) {
    return -4;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -4;
  }
  json[tokens[k].end] = '\0';
  actual->channels = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -5;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"type",4)) {
    return -5;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -5;
  }
  json[tokens[k].end] = '\0';
  actual->type = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -6;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"fade",4)) {
    return -6;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -6;
  }

  json[tokens[k].end] = '\0';
  actual->fade = atof(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -7;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"volume",6)) {
    return -7;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_ARRAY) {
    return -7;
  }

  k++;
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
      return -7;
    }

    json[tokens[k].end] = '\0';
    actual->volume[i] = atof(&json[tokens[k].start]);
    k++;
  }

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -8;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"map",3)) {
    return -8;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_ARRAY) {
    return -8;
  }

  k++;
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
      return -8;
    }

    json[tokens[k].end] = '\0';
    actual->map[i] = atoi(&json[tokens[k].start]);
    k++;
  }

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -9;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"mixmap",6)) {
    return -9;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_ARRAY) {
    return -9;
  }

  k++;
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
      return -9;
    }

    json[tokens[k].end] = '\0';
    actual->mixmap[i] = atoi(&json[tokens[k].start]);
    k++;
  }

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -10;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"rms",3)) {
    return -10;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_ARRAY) {
    return -10;
  }

  k++;
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
      return -10;
    }

    json[tokens[k].end] = '\0';
    actual->rms[i] = atof(&json[tokens[k].start]);
    k++;
  }

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -11;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"peak",4)) {
    return -11;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_ARRAY) {
    return -11;
  }

  k++;
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
    if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
      return -11;
    }

    json[tokens[k].end] = '\0';
    actual->peak[i] = atof(&json[tokens[k].start]);
    k++;
  }

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -12;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"delay",5)) {
    return -12;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -12;
  }

  json[tokens[k].end] = '\0';
  actual->delay = atoi(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -13;
  }

  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"lowpass",7)) {
    return -13;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_OBJECT) {
    return -13;
  }

  uber.actual = &(actual->lowpass);
  uber.type = DEJSON_KR_LOWPASS_INFO;
  json[tokens[k].end] = '\0';
  res = info_unpack_fr_json(&json[tokens[k].start],&uber);
  if (res < 0) {
    return -13;
  }

  k += res;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -14;
  }

  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"highpass",8)) {
    return -14;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_OBJECT) {
    return -14;
  }

  uber.actual = &(actual->highpass);
  uber.type = DEJSON_KR_HIGHPASS_INFO;
  json[tokens[k].end] = '\0';
  res = info_unpack_fr_json(&json[tokens[k].start],&uber);
  if (res < 0) {
    return -14;
  }

  k += res;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -15;
  }

  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"analog",6)) {
    return -15;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_OBJECT) {
    return -15;
  }

  uber.actual = &(actual->analog);
  uber.type = DEJSON_KR_ANALOG_INFO;
  json[tokens[k].end] = '\0';
  res = info_unpack_fr_json(&json[tokens[k].start],&uber);
  if (res < 0) {
    return -15;
  }

  k += res;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -16;
  }

  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"eq",2)) {
    return -16;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_OBJECT) {
    return -16;
  }

  uber.actual = &(actual->eq);
  uber.type = DEJSON_KR_EQ_INFO;
  json[tokens[k].end] = '\0';
  res = info_unpack_fr_json(&json[tokens[k].start],&uber);
  if (res < 0) {
    return -16;
  }

  k += res;

  res = k;

  return res;
}

