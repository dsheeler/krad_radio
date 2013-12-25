#include "krad_jack_to_ebml.h"

int kr_jack_direction_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_jack_direction *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_direction *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_jack_state_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_jack_state *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_state *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_jack_setup_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_jack_setup_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_setup_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->client_name);
  res += kr_ebml_pack_string(ebml, 0xe1, actual->server_name);

  return res;
}

int kr_jack_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  kr_jack_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->client_name);
  res += kr_ebml_pack_string(ebml, 0xe1, actual->server_name);
  uber.actual = &(actual->state);
  uber.type = EBML_KR_JACK_STATE;
  res += info_pack_to_ebml(&ebml[res],&uber);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->inputs);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->outputs);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->sample_rate);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->period_size);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->xruns);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->frames);

  return res;
}

int kr_jack_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  kr_jack_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_jack_path_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->name);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->channels);
  uber.actual = &(actual->direction);
  uber.type = EBML_KR_JACK_DIRECTION;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

