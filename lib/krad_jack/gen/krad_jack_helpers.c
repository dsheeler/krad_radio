#include "krad_jack_helpers.h"

int kr_jack_direction_to_index(int val) {
  switch (val) {
    case KR_JACK_INPUT:
      return 0;
    case KR_JACK_OUTPUT:
      return 1;
  }
  return -1;
}

char *kr_strfr_kr_jack_direction(int val) {
  switch (val) {
    case KR_JACK_INPUT:
      return "kr_jack_input";
    case KR_JACK_OUTPUT:
      return "kr_jack_output";
  }
  return NULL;
}

int kr_strto_kr_jack_direction(char *string) {
  if (!strcmp(string,"kr_jack_input")) {
    return KR_JACK_INPUT;
  }
  if (!strcmp(string,"kr_jack_output")) {
    return KR_JACK_OUTPUT;
  }

  return -1;
}

int kr_jack_state_to_index(int val) {
  switch (val) {
    case KR_JACK_OFFLINE:
      return 0;
    case KR_JACK_ONLINE:
      return 1;
  }
  return -1;
}

char *kr_strfr_kr_jack_state(int val) {
  switch (val) {
    case KR_JACK_OFFLINE:
      return "kr_jack_offline";
    case KR_JACK_ONLINE:
      return "kr_jack_online";
  }
  return NULL;
}

int kr_strto_kr_jack_state(char *string) {
  if (!strcmp(string,"kr_jack_offline")) {
    return KR_JACK_OFFLINE;
  }
  if (!strcmp(string,"kr_jack_online")) {
    return KR_JACK_ONLINE;
  }

  return -1;
}

int kr_jack_setup_info_init(kr_jack_setup_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_setup_info));
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_jack_setup_info_valid(kr_jack_setup_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 64; i++) {
    if (!st->client_name[i]) {
      break;
    }
    if (i == 63 && st->client_name[i]) {
      return -2;
    }
  }
  for (i = 0; i < 64; i++) {
    if (!st->server_name[i]) {
      break;
    }
    if (i == 63 && st->server_name[i]) {
      return -3;
    }
  }

  return 0;
}

int kr_jack_setup_info_random(kr_jack_setup_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_setup_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->client_name[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->client_name[63] = '\0';
    }
  }
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->server_name[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->server_name[63] = '\0';
    }
  }

  return 0;
}

int kr_jack_info_init(kr_jack_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_info));
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_jack_info_valid(kr_jack_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 64; i++) {
    if (!st->client_name[i]) {
      break;
    }
    if (i == 63 && st->client_name[i]) {
      return -2;
    }
  }
  for (i = 0; i < 64; i++) {
    if (!st->server_name[i]) {
      break;
    }
    if (i == 63 && st->server_name[i]) {
      return -3;
    }
  }

  return 0;
}

int kr_jack_info_random(kr_jack_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->client_name[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->client_name[63] = '\0';
    }
  }
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->server_name[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->server_name[63] = '\0';
    }
  }

  return 0;
}

int kr_jack_path_info_init(kr_jack_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_path_info));
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_jack_path_info_valid(kr_jack_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 64; i++) {
    if (!st->name[i]) {
      break;
    }
    if (i == 63 && st->name[i]) {
      return -2;
    }
  }

  return 0;
}

int kr_jack_path_info_random(kr_jack_path_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_path_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->name[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->name[63] = '\0';
    }
  }

  return 0;
}

