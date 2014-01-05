#include "krad_jack_to_json.h"

int kr_jack_direction_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_jack_direction *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_direction *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_jack_state_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_jack_state *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_state *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_jack_setup_info_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_jack_setup_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_setup_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"client_name\" : \"%s\",",actual->client_name);
  res += snprintf(&json[res],max-res,"\"server_name\" : \"%s\"",actual->server_name);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_jack_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  kr_jack_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"client_name\" : \"%s\",",actual->client_name);
  res += snprintf(&json[res],max-res,"\"server_name\" : \"%s\",",actual->server_name);
  res += snprintf(&json[res],max-res,"\"state\": ");
  uber.actual = &(actual->state);
  uber.type = JSON_KR_JACK_STATE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"inputs\" : %u,",actual->inputs);
  res += snprintf(&json[res],max-res,"\"outputs\" : %u,",actual->outputs);
  res += snprintf(&json[res],max-res,"\"sample_rate\" : %u,",actual->sample_rate);
  res += snprintf(&json[res],max-res,"\"period_size\" : %u,",actual->period_size);
  res += snprintf(&json[res],max-res,"\"xruns\" : %u,",actual->xruns);
  res += snprintf(&json[res],max-res,"\"frames\" : %ju",actual->frames);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_jack_path_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  kr_jack_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_path_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"name\" : \"%s\",",actual->name);
  res += snprintf(&json[res],max-res,"\"channels\" : %d,",actual->channels);
  res += snprintf(&json[res],max-res,"\"direction\": ");
  uber.actual = &(actual->direction);
  uber.type = JSON_KR_JACK_DIRECTION;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

