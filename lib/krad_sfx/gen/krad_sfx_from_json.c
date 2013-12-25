#include "krad_sfx_from_json.h"

int kr_sfx_control_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  kr_sfx_control *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_sfx_control *)st;

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

int kr_sfx_effect_type_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  kr_sfx_effect_type *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_sfx_effect_type *)st;

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

int kr_sfx_effect_control_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  kr_sfx_effect_control *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_sfx_effect_control *)st;

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

int kr_eq_band_info_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  struct kr_eq_band_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_eq_band_info *)st;

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
  if (strncmp(&json[tokens[k].start],"db",2)) {
    return -1;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -1;
  }

  json[tokens[k].end] = '\0';
  actual->db = atof(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -2;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"bw",2)) {
    return -2;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -2;
  }

  json[tokens[k].end] = '\0';
  actual->bw = atof(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -3;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"hz",2)) {
    return -3;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -3;
  }

  json[tokens[k].end] = '\0';
  actual->hz = atof(&json[tokens[k].start]);
  k++;

  res = k;

  return res;
}

int kr_eq_info_fr_json(char *json, void *st) {
  uber_St uber;
  int i;
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  struct kr_eq_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_eq_info *)st;

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
  if (strncmp(&json[tokens[k].start],"band",4)) {
    return -1;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_ARRAY) {
    return -1;
  }

  k++;

  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    if (ntokens > k && tokens[k].type != JSMN_OBJECT) {
      return -1;
    }

    uber.actual = &(actual->band[i]);
    uber.type = DEJSON_KR_EQ_BAND_INFO;
    json[tokens[k].end] = '\0';
    res = info_unpack_fr_json(&json[tokens[k].start],&uber);
    if (res < 0) {
      return -1;
    }

    k += res;

  }

  res = k;

  return res;
}

int kr_lowpass_info_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  struct kr_lowpass_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_lowpass_info *)st;

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
  if (strncmp(&json[tokens[k].start],"bw",2)) {
    return -1;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -1;
  }

  json[tokens[k].end] = '\0';
  actual->bw = atof(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -2;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"hz",2)) {
    return -2;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -2;
  }

  json[tokens[k].end] = '\0';
  actual->hz = atof(&json[tokens[k].start]);
  k++;

  res = k;

  return res;
}

int kr_highpass_info_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  struct kr_highpass_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_highpass_info *)st;

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
  if (strncmp(&json[tokens[k].start],"bw",2)) {
    return -1;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -1;
  }

  json[tokens[k].end] = '\0';
  actual->bw = atof(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -2;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"hz",2)) {
    return -2;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -2;
  }

  json[tokens[k].end] = '\0';
  actual->hz = atof(&json[tokens[k].start]);
  k++;

  res = k;

  return res;
}

int kr_analog_info_fr_json(char *json, void *st) {
  int res;
  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmnerr_t err;
  int ntokens;
  int k;
  struct kr_analog_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_analog_info *)st;

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
  if (strncmp(&json[tokens[k].start],"drive",5)) {
    return -1;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -1;
  }

  json[tokens[k].end] = '\0';
  actual->drive = atof(&json[tokens[k].start]);
  k++;

  if (ntokens > k && tokens[k].type != JSMN_STRING) {
    return -2;
  }
  json[tokens[k].end] = '\0';
  if (strncmp(&json[tokens[k].start],"blend",5)) {
    return -2;
  }

  k++;

  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {
    return -2;
  }

  json[tokens[k].end] = '\0';
  actual->blend = atof(&json[tokens[k].start]);
  k++;

  res = k;

  return res;
}

