#include "krad_jack_to_text.h"

int kr_jack_direction_to_text(char *text, void *st, int32_t max) {
  int res;
  kr_jack_direction *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_direction *)st;

  res += snprintf(&text[res],max-res,"kr_jack_direction : %u \n",*actual);

  return res;
}

int kr_jack_state_to_text(char *text, void *st, int32_t max) {
  int res;
  kr_jack_state *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_state *)st;

  res += snprintf(&text[res],max-res,"kr_jack_state : %u \n",*actual);

  return res;
}

int kr_jack_setup_info_to_text(char *text, void *st, int32_t max) {
  int res;
  kr_jack_setup_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_setup_info *)st;

  res += snprintf(&text[res],max-res,"client_name : %s \n",actual->client_name);
  res += snprintf(&text[res],max-res,"server_name : %s \n",actual->server_name);

  return res;
}

int kr_jack_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  kr_jack_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_info *)st;

  res += snprintf(&text[res],max-res,"client_name : %s \n",actual->client_name);
  res += snprintf(&text[res],max-res,"server_name : %s \n",actual->server_name);
  uber.actual = &(actual->state);
  uber.type = TEXT_KR_JACK_STATE;
  res += info_pack_to_text(&text[res],&uber,max-res);
  res += snprintf(&text[res],max-res,"inputs : %u \n",actual->inputs);
  res += snprintf(&text[res],max-res,"outputs : %u \n",actual->outputs);
  res += snprintf(&text[res],max-res,"sample_rate : %u \n",actual->sample_rate);
  res += snprintf(&text[res],max-res,"period_size : %u \n",actual->period_size);
  res += snprintf(&text[res],max-res,"xruns : %u \n",actual->xruns);
  res += snprintf(&text[res],max-res,"frames : %ju \n",actual->frames);

  return res;
}

int kr_jack_path_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  kr_jack_path_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_jack_path_info *)st;

  res += snprintf(&text[res],max-res,"name : %s \n",actual->name);
  res += snprintf(&text[res],max-res,"channels : %d \n",actual->channels);
  uber.actual = &(actual->direction);
  uber.type = TEXT_KR_JACK_DIRECTION;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

