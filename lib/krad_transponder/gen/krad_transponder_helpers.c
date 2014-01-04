#include "krad_transponder_helpers.h"

int kr_adapter_path_direction_to_index(int val) {
  switch (val) {
    case KR_ADP_PATH_INPUT:
      return 0;
    case KR_ADP_PATH_OUTPUT:
      return 1;
  }
  return -1;
}

char *kr_strfr_kr_adapter_path_direction(int val) {
  switch (val) {
    case KR_ADP_PATH_INPUT:
      return "kr_adp_path_input";
    case KR_ADP_PATH_OUTPUT:
      return "kr_adp_path_output";
  }
  return NULL;
}

int kr_strto_kr_adapter_path_direction(char *string) {
  if (!strcmp(string,"kr_adp_path_input")) {
    return KR_ADP_PATH_INPUT;
  }
  if (!strcmp(string,"kr_adp_path_output")) {
    return KR_ADP_PATH_OUTPUT;
  }

  return -1;
}

int kr_adapter_api_to_index(int val) {
  switch (val) {
    case KR_ADP_JACK:
      return 0;
    case KR_ADP_WAYLAND:
      return 1;
    case KR_ADP_V4L2:
      return 2;
    case KR_ADP_DECKLINK:
      return 3;
    case KR_ADP_X11:
      return 4;
    case KR_ADP_ALSA:
      return 5;
    case KR_ADP_KRAD:
      return 6;
    case KR_ADP_ENCODER:
      return 7;
  }
  return -1;
}

char *kr_strfr_kr_adapter_api(int val) {
  switch (val) {
    case KR_ADP_JACK:
      return "kr_adp_jack";
    case KR_ADP_WAYLAND:
      return "kr_adp_wayland";
    case KR_ADP_V4L2:
      return "kr_adp_v4l2";
    case KR_ADP_DECKLINK:
      return "kr_adp_decklink";
    case KR_ADP_X11:
      return "kr_adp_x11";
    case KR_ADP_ALSA:
      return "kr_adp_alsa";
    case KR_ADP_KRAD:
      return "kr_adp_krad";
    case KR_ADP_ENCODER:
      return "kr_adp_encoder";
  }
  return NULL;
}

int kr_strto_kr_adapter_api(char *string) {
  if (!strcmp(string,"kr_adp_jack")) {
    return KR_ADP_JACK;
  }
  if (!strcmp(string,"kr_adp_wayland")) {
    return KR_ADP_WAYLAND;
  }
  if (!strcmp(string,"kr_adp_v4l2")) {
    return KR_ADP_V4L2;
  }
  if (!strcmp(string,"kr_adp_decklink")) {
    return KR_ADP_DECKLINK;
  }
  if (!strcmp(string,"kr_adp_x11")) {
    return KR_ADP_X11;
  }
  if (!strcmp(string,"kr_adp_alsa")) {
    return KR_ADP_ALSA;
  }
  if (!strcmp(string,"kr_adp_krad")) {
    return KR_ADP_KRAD;
  }
  if (!strcmp(string,"kr_adp_encoder")) {
    return KR_ADP_ENCODER;
  }

  return -1;
}

int kr_adapter_api_info_init(void *st, int idx) {
  kr_adapter_api_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_info *)st;
  memset(actual, 0, sizeof(kr_adapter_api_info));
  switch (idx) {
    case 0: {
      kr_jack_info_init(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_info_init(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_info_init(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_info_init(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_info_init(&actual->x11);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_info_valid(void *st, int idx) {
  kr_adapter_api_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_info *)st;
  switch (idx) {
    case 0: {
      kr_jack_info_valid(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_info_valid(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_info_valid(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_info_valid(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_info_valid(&actual->x11);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_info_random(void *st, int idx) {
  kr_adapter_api_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_info *)st;
  memset(st, 0, sizeof(kr_adapter_api_info));
  switch (idx) {
    case 0: {
      kr_jack_info_random(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_info_random(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_info_random(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_info_random(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_info_random(&actual->x11);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_path_info_init(void *st, int idx) {
  kr_adapter_api_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_path_info *)st;
  memset(actual, 0, sizeof(kr_adapter_api_path_info));
  switch (idx) {
    case 0: {
      kr_jack_path_info_init(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_path_info_init(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_open_info_init(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_path_info_init(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_path_info_init(&actual->x11);
      break;
    }
    case 5: {
      kr_alsa_path_info_init(&actual->alsa);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_path_info_valid(void *st, int idx) {
  kr_adapter_api_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_path_info *)st;
  switch (idx) {
    case 0: {
      kr_jack_path_info_valid(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_path_info_valid(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_open_info_valid(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_path_info_valid(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_path_info_valid(&actual->x11);
      break;
    }
    case 5: {
      kr_alsa_path_info_valid(&actual->alsa);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_path_info_random(void *st, int idx) {
  kr_adapter_api_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_path_info *)st;
  memset(st, 0, sizeof(kr_adapter_api_path_info));
  switch (idx) {
    case 0: {
      kr_jack_path_info_random(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_path_info_random(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_open_info_random(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_path_info_random(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_path_info_random(&actual->x11);
      break;
    }
    case 5: {
      kr_alsa_path_info_random(&actual->alsa);
      break;
    }
  }


  return -1;
}

int kr_adapter_info_init(void *st) {
  struct kr_adapter_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_info *)st;
  memset(actual, 0, sizeof(struct kr_adapter_info));
  kr_adapter_api_info_init(&actual->api_info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_info_valid(void *st) {
  struct kr_adapter_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_info *)st;
  kr_adapter_api_info_valid(&actual->api_info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_info_random(void *st) {
  struct kr_adapter_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_info *)st;
  memset(st, 0, sizeof(struct kr_adapter_info));
  kr_adapter_api_info_random(&actual->api_info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_path_info_init(void *st) {
  struct kr_adapter_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_path_info *)st;
  memset(actual, 0, sizeof(struct kr_adapter_path_info));
  for (i = 0; i < 64; i++) {
  }
  kr_adapter_api_path_info_init(&actual->info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_path_info_valid(void *st) {
  struct kr_adapter_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_path_info *)st;
  for (i = 0; i < 64; i++) {
    if (!actual->name[i]) {
      break;
    }
    if (i == 63 && actual->name[i]) {
      return -2;
    }
  }
  kr_adapter_api_path_info_valid(&actual->info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_path_info_random(void *st) {
  struct kr_adapter_path_info *actual;

  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_path_info *)st;
  memset(st, 0, sizeof(struct kr_adapter_path_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    actual->name[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      actual->name[63] = '\0';
    }
  }
  kr_adapter_api_path_info_random(&actual->info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_transponder_path_io_type_to_index(int val) {
  switch (val) {
    case KR_XPDR_MIXER:
      return 0;
    case KR_XPDR_COMPOSITOR:
      return 1;
    case KR_XPDR_ADAPTER:
      return 2;
  }
  return -1;
}

char *kr_strfr_kr_transponder_path_io_type(int val) {
  switch (val) {
    case KR_XPDR_MIXER:
      return "kr_xpdr_mixer";
    case KR_XPDR_COMPOSITOR:
      return "kr_xpdr_compositor";
    case KR_XPDR_ADAPTER:
      return "kr_xpdr_adapter";
  }
  return NULL;
}

int kr_strto_kr_transponder_path_io_type(char *string) {
  if (!strcmp(string,"kr_xpdr_mixer")) {
    return KR_XPDR_MIXER;
  }
  if (!strcmp(string,"kr_xpdr_compositor")) {
    return KR_XPDR_COMPOSITOR;
  }
  if (!strcmp(string,"kr_xpdr_adapter")) {
    return KR_XPDR_ADAPTER;
  }

  return -1;
}

int kr_transponder_info_init(void *st) {
  struct kr_transponder_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_info *)st;
  memset(actual, 0, sizeof(struct kr_transponder_info));

  return 0;
}

int kr_transponder_info_valid(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_transponder_info_random(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_transponder_path_io_path_info_init(void *st, int idx) {
  kr_transponder_path_io_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_transponder_path_io_path_info *)st;
  memset(actual, 0, sizeof(kr_transponder_path_io_path_info));
  switch (idx) {
    case 0: {
      kr_mixer_path_info_init(&actual->mixer_path_info);
      break;
    }
    case 1: {
      kr_compositor_path_info_init(&actual->compositor_path_info);
      break;
    }
    case 2: {
      kr_adapter_path_info_init(&actual->adapter_path_info);
      break;
    }
  }


  return -1;
}

int kr_transponder_path_io_path_info_valid(void *st, int idx) {
  kr_transponder_path_io_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_transponder_path_io_path_info *)st;
  switch (idx) {
    case 0: {
      kr_mixer_path_info_valid(&actual->mixer_path_info);
      break;
    }
    case 1: {
      kr_compositor_path_info_valid(&actual->compositor_path_info);
      break;
    }
    case 2: {
      kr_adapter_path_info_valid(&actual->adapter_path_info);
      break;
    }
  }


  return -1;
}

int kr_transponder_path_io_path_info_random(void *st, int idx) {
  kr_transponder_path_io_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_transponder_path_io_path_info *)st;
  memset(st, 0, sizeof(kr_transponder_path_io_path_info));
  switch (idx) {
    case 0: {
      kr_mixer_path_info_random(&actual->mixer_path_info);
      break;
    }
    case 1: {
      kr_compositor_path_info_random(&actual->compositor_path_info);
      break;
    }
    case 2: {
      kr_adapter_path_info_random(&actual->adapter_path_info);
      break;
    }
  }


  return -1;
}

int kr_transponder_path_io_info_init(void *st) {
  struct kr_transponder_path_io_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_io_info *)st;
  memset(actual, 0, sizeof(struct kr_transponder_path_io_info));
  kr_transponder_path_io_path_info_init(&actual->info,kr_transponder_path_io_type_to_index(actual->type));

  return 0;
}

int kr_transponder_path_io_info_valid(void *st) {
  struct kr_transponder_path_io_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_io_info *)st;
  kr_transponder_path_io_path_info_valid(&actual->info,kr_transponder_path_io_type_to_index(actual->type));

  return 0;
}

int kr_transponder_path_io_info_random(void *st) {
  struct kr_transponder_path_io_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_io_info *)st;
  memset(st, 0, sizeof(struct kr_transponder_path_io_info));
  kr_transponder_path_io_path_info_random(&actual->info,kr_transponder_path_io_type_to_index(actual->type));

  return 0;
}

int kr_transponder_path_info_init(void *st) {
  struct kr_transponder_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_info *)st;
  memset(actual, 0, sizeof(struct kr_transponder_path_info));
  for (i = 0; i < 128; i++) {
  }
  kr_transponder_path_io_info_init(&actual->input);
  kr_transponder_path_io_info_init(&actual->output);

  return 0;
}

int kr_transponder_path_info_valid(void *st) {
  struct kr_transponder_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_info *)st;
  for (i = 0; i < 128; i++) {
    if (!actual->name[i]) {
      break;
    }
    if (i == 127 && actual->name[i]) {
      return -2;
    }
  }
  kr_transponder_path_io_info_valid(&actual->input);
  kr_transponder_path_io_info_valid(&actual->output);

  return 0;
}

int kr_transponder_path_info_random(void *st) {
  struct kr_transponder_path_info *actual;

  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_info *)st;
  memset(st, 0, sizeof(struct kr_transponder_path_info));
  for (i = 0; i < 128; i++) {
    scale = (double)25 / RAND_MAX;
    actual->name[i] = 97 + floor(rand() * scale);
    if (i == 127) {
      actual->name[127] = '\0';
    }
  }
  kr_transponder_path_io_info_random(&actual->input);
  kr_transponder_path_io_info_random(&actual->output);

  return 0;
}

