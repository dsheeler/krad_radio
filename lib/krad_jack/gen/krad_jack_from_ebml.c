#include "krad_jack_from_ebml.h"

int kr_jack_direction_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_jack_direction *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_direction *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, actual);

  return res;
}

int kr_jack_state_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_jack_state *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_state *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, actual);

  return res;
}

int kr_jack_setup_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_jack_setup_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_setup_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->client_name , sizeof(actual->client_name));
  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->server_name , sizeof(actual->server_name));

  return res;
}

int kr_jack_info_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  kr_jack_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->client_name , sizeof(actual->client_name));
  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->server_name , sizeof(actual->server_name));
  uber.actual = &(actual->state);
  uber.type = DEBML_KR_JACK_STATE;
  res += info_unpack_fr_ebml(&ebml[res],&uber);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->inputs);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->outputs);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->sample_rate);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->period_size);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->xruns);
  res += kr_ebml2_unpack_element_uint64(ebml, NULL, &actual->frames);

  return res;
}

int kr_jack_path_info_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  kr_jack_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_path_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->name , sizeof(actual->name));
  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->channels);
  uber.actual = &(actual->direction);
  uber.type = DEBML_KR_JACK_DIRECTION;
  res += info_unpack_fr_ebml(&ebml[res],&uber);

  return res;
}

