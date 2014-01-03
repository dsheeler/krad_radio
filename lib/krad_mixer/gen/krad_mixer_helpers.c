#include "krad_mixer_helpers.h"

int kr_mixer_channels_to_index(int val) {
  switch (val) {
    case NIL:
      return 0;
    case MONO:
      return 1;
    case STEREO:
      return 2;
    case THREE:
      return 3;
    case QUAD:
      return 4;
    case FIVE:
      return 5;
    case SIX:
      return 6;
    case SEVEN:
      return 7;
    case EIGHT:
      return 8;
  }
  return -1;
}

char *kr_strfr_kr_mixer_channels(int val) {
  switch (val) {
    case NIL:
      return "nil";
    case MONO:
      return "mono";
    case STEREO:
      return "stereo";
    case THREE:
      return "three";
    case QUAD:
      return "quad";
    case FIVE:
      return "five";
    case SIX:
      return "six";
    case SEVEN:
      return "seven";
    case EIGHT:
      return "eight";
  }
  return NULL;
}

int kr_strto_kr_mixer_channels(char *string) {
  if (!strcmp(string,"nil")) {
    return NIL;
  }
  if (!strcmp(string,"mono")) {
    return MONO;
  }
  if (!strcmp(string,"stereo")) {
    return STEREO;
  }
  if (!strcmp(string,"three")) {
    return THREE;
  }
  if (!strcmp(string,"quad")) {
    return QUAD;
  }
  if (!strcmp(string,"five")) {
    return FIVE;
  }
  if (!strcmp(string,"six")) {
    return SIX;
  }
  if (!strcmp(string,"seven")) {
    return SEVEN;
  }
  if (!strcmp(string,"eight")) {
    return EIGHT;
  }

  return -1;
}

int kr_mixer_path_type_to_index(int val) {
  switch (val) {
    case KR_MXR_INPUT:
      return 0;
    case KR_MXR_BUS:
      return 1;
    case KR_MXR_OUTPUT:
      return 2;
  }
  return -1;
}

char *kr_strfr_kr_mixer_path_type(int val) {
  switch (val) {
    case KR_MXR_INPUT:
      return "kr_mxr_input";
    case KR_MXR_BUS:
      return "kr_mxr_bus";
    case KR_MXR_OUTPUT:
      return "kr_mxr_output";
  }
  return NULL;
}

int kr_strto_kr_mixer_path_type(char *string) {
  if (!strcmp(string,"kr_mxr_input")) {
    return KR_MXR_INPUT;
  }
  if (!strcmp(string,"kr_mxr_bus")) {
    return KR_MXR_BUS;
  }
  if (!strcmp(string,"kr_mxr_output")) {
    return KR_MXR_OUTPUT;
  }

  return -1;
}

int kr_mixer_info_init(struct kr_mixer_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_mixer_info));
  for (i = 0; i < 32; i++) {
  }

  return 0;
}

int kr_mixer_info_valid(struct kr_mixer_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 32; i++) {
    if (!st->clock[i]) {
      break;
    }
    if (i == 31 && st->clock[i]) {
      return -9;
    }
  }

  return 0;
}

int kr_mixer_info_random(struct kr_mixer_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_mixer_info));
  for (i = 0; i < 32; i++) {
    scale = (double)25 / RAND_MAX;
    st->clock[i] = 97 + floor(rand() * scale);
    if (i == 31) {
      st->clock[31] = '\0';
    }
  }

  return 0;
}

int kr_mixer_path_info_init(struct kr_mixer_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_mixer_path_info));
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  kr_lowpass_info_init(&st->lowpass);
  kr_highpass_info_init(&st->highpass);
  kr_analog_info_init(&st->analog);
  kr_eq_info_init(&st->eq);

  return 0;
}

int kr_mixer_path_info_valid(struct kr_mixer_path_info *st) {
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
  for (i = 0; i < 64; i++) {
    if (!st->bus[i]) {
      break;
    }
    if (i == 63 && st->bus[i]) {
      return -3;
    }
  }
  for (i = 0; i < 64; i++) {
    if (!st->crossfade_group[i]) {
      break;
    }
    if (i == 63 && st->crossfade_group[i]) {
      return -4;
    }
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  kr_lowpass_info_valid(&st->lowpass);
  kr_highpass_info_valid(&st->highpass);
  kr_analog_info_valid(&st->analog);
  kr_eq_info_valid(&st->eq);

  return 0;
}

int kr_mixer_path_info_random(struct kr_mixer_path_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_mixer_path_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->name[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->name[63] = '\0';
    }
  }
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->bus[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->bus[63] = '\0';
    }
  }
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->crossfade_group[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->crossfade_group[63] = '\0';
    }
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  for (i = 0; i < KR_MXR_MAX_CHANNELS; i++) {
  }
  kr_lowpass_info_random(&st->lowpass);
  kr_highpass_info_random(&st->highpass);
  kr_analog_info_random(&st->analog);
  kr_eq_info_random(&st->eq);

  return 0;
}

