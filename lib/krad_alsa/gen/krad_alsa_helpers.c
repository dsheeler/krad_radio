#include "krad_alsa_helpers.h"

int kr_alsa_info_init(struct kr_alsa_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_alsa_info));
  for (i = 0; i < 40; i++) {
  }

  return 0;
}

int kr_alsa_info_valid(struct kr_alsa_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 40; i++) {
  }

  return 0;
}

int kr_alsa_info_random(struct kr_alsa_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_alsa_info));
  for (i = 0; i < 40; i++) {
  }

  return 0;
}

int kr_alsa_path_info_init(struct kr_alsa_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_alsa_path_info));

  return 0;
}

int kr_alsa_path_info_valid(struct kr_alsa_path_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_alsa_path_info_random(struct kr_alsa_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_alsa_path_info));

  return 0;
}

