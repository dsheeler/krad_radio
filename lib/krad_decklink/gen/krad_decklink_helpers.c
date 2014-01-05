#include "krad_decklink_helpers.h"

int kr_decklink_info_init(void *st) {
  struct kr_decklink_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_decklink_info *)st;
  memset(actual, 0, sizeof(struct kr_decklink_info));
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_decklink_info_valid(void *st) {
  struct kr_decklink_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_decklink_info *)st;
  for (i = 0; i < 64; i++) {
    if (!actual->device[i]) {
      break;
    }
    if (i == 63 && actual->device[i]) {
      return -2;
    }
  }

  return 0;
}

int kr_decklink_info_random(void *st) {
  struct kr_decklink_info *actual;

  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_decklink_info *)st;
  memset(st, 0, sizeof(struct kr_decklink_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    actual->device[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      actual->device[63] = '\0';
    }
  }

  return 0;
}

int kr_decklink_path_info_init(void *st) {
  struct kr_decklink_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_decklink_path_info *)st;
  memset(actual, 0, sizeof(struct kr_decklink_path_info));
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }
  for (i = 0; i < 64; i++) {
  }

  return 0;
}

int kr_decklink_path_info_valid(void *st) {
  struct kr_decklink_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_decklink_path_info *)st;
  for (i = 0; i < 64; i++) {
    if (!actual->device[i]) {
      break;
    }
    if (i == 63 && actual->device[i]) {
      return -2;
    }
  }
  for (i = 0; i < 64; i++) {
    if (!actual->video_connector[i]) {
      break;
    }
    if (i == 63 && actual->video_connector[i]) {
      return -7;
    }
  }
  for (i = 0; i < 64; i++) {
    if (!actual->audio_connector[i]) {
      break;
    }
    if (i == 63 && actual->audio_connector[i]) {
      return -8;
    }
  }

  return 0;
}

int kr_decklink_path_info_random(void *st) {
  struct kr_decklink_path_info *actual;

  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_decklink_path_info *)st;
  memset(st, 0, sizeof(struct kr_decklink_path_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    actual->device[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      actual->device[63] = '\0';
    }
  }
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    actual->video_connector[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      actual->video_connector[63] = '\0';
    }
  }
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    actual->audio_connector[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      actual->audio_connector[63] = '\0';
    }
  }

  return 0;
}

