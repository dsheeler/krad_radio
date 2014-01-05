#include "krad_transponder_to_ebml.h"

int kr_adapter_path_direction_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_adapter_path_direction *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_adapter_path_direction *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_adapter_api_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_adapter_api *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_adapter_api *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_adapter_api_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  uber_St *uber_actual;

  kr_adapter_api_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_info *)uber_actual->actual;

  switch (uber_actual->type) {
    case 0: {
      uber.actual = &(actual->jack);
      uber.type = EBML_KR_JACK_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 1: {
      uber.actual = &(actual->wayland);
      uber.type = EBML_KR_WAYLAND_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 2: {
      uber.actual = &(actual->v4l2);
      uber.type = EBML_KR_V4L2_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 3: {
      uber.actual = &(actual->decklink);
      uber.type = EBML_KR_DECKLINK_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 4: {
      uber.actual = &(actual->x11);
      uber.type = EBML_KR_X11_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
  }


  return res;
}

int kr_adapter_api_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  uber_St *uber_actual;

  kr_adapter_api_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_path_info *)uber_actual->actual;

  switch (uber_actual->type) {
    case 0: {
      uber.actual = &(actual->jack);
      uber.type = EBML_KR_JACK_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 1: {
      uber.actual = &(actual->wayland);
      uber.type = EBML_KR_WAYLAND_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 2: {
      uber.actual = &(actual->v4l2);
      uber.type = EBML_KR_V4L2_OPEN_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 3: {
      uber.actual = &(actual->decklink);
      uber.type = EBML_KR_DECKLINK_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 4: {
      uber.actual = &(actual->x11);
      uber.type = EBML_KR_X11_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 5: {
      uber.actual = &(actual->alsa);
      uber.type = EBML_KR_ALSA_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
  }


  return res;
}

int kr_adapter_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_adapter_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_adapter_info *)st;

  uber.actual = &(actual->api);
  uber.type = EBML_KR_ADAPTER_API;
  res += info_pack_to_ebml(&ebml[res],&uber);
  index = kr_adapter_api_to_index(actual->api);
  uber_sub.type = index;
  uber_sub.actual = &(actual->api_info);
  uber.actual = &(uber_sub);
  uber.type = EBML_KR_ADAPTER_API_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_adapter_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_adapter_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_adapter_path_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->name);
  uber.actual = &(actual->dir);
  uber.type = EBML_KR_ADAPTER_PATH_DIRECTION;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->api);
  uber.type = EBML_KR_ADAPTER_API;
  res += info_pack_to_ebml(&ebml[res],&uber);
  index = kr_adapter_api_to_index(actual->api);
  uber_sub.type = index;
  uber_sub.actual = &(actual->info);
  uber.actual = &(uber_sub);
  uber.type = EBML_KR_ADAPTER_API_PATH_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int krad_link_av_mode_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  krad_link_av_mode_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (krad_link_av_mode_t *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_txpdr_su_type_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_txpdr_su_type_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_txpdr_su_type_t *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int krad_link_transport_mode_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  krad_link_transport_mode_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (krad_link_transport_mode_t *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_transponder_path_io_type_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_transponder_path_io_type *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_transponder_path_io_type *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_transponder_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_transponder_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_transponder_info *)st;

  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->active_paths);

  return res;
}

int kr_transponder_path_io_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  uber_St *uber_actual;

  kr_transponder_path_io_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_transponder_path_io_path_info *)uber_actual->actual;

  switch (uber_actual->type) {
    case 0: {
      uber.actual = &(actual->mixer_path_info);
      uber.type = EBML_KR_MIXER_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 1: {
      uber.actual = &(actual->compositor_path_info);
      uber.type = EBML_KR_COMPOSITOR_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 2: {
      uber.actual = &(actual->adapter_path_info);
      uber.type = EBML_KR_ADAPTER_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
  }


  return res;
}

int kr_transponder_path_io_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_transponder_path_io_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_transponder_path_io_info *)st;

  uber.actual = &(actual->type);
  uber.type = EBML_KR_TRANSPONDER_PATH_IO_TYPE;
  res += info_pack_to_ebml(&ebml[res],&uber);
  index = kr_transponder_path_io_type_to_index(actual->type);
  uber_sub.type = index;
  uber_sub.actual = &(actual->info);
  uber.actual = &(uber_sub);
  uber.type = EBML_KR_TRANSPONDER_PATH_IO_PATH_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_transponder_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_transponder_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_transponder_path_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->name);
  uber.actual = &(actual->input);
  uber.type = EBML_KR_TRANSPONDER_PATH_IO_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->output);
  uber.type = EBML_KR_TRANSPONDER_PATH_IO_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

