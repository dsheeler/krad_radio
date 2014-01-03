#include "krad_decklink_helpers.h"

int kr_decklink_info_init(struct kr_decklink_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_decklink_info));
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_decklink_info_valid(struct kr_decklink_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_decklink_info_random(struct kr_decklink_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_decklink_info));
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_decklink_path_info_init(struct kr_decklink_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_decklink_path_info));
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_decklink_path_info_valid(struct kr_decklink_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_decklink_path_info_random(struct kr_decklink_path_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_decklink_path_info));
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

