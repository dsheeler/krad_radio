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
    if (!st->device[i]) {
      break;
    }
    if (i == 63 && st->device[i]) {
      return -2;
    }
  }

  return 0;
}

int kr_decklink_info_random(struct kr_decklink_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_decklink_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->device[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->device[63] = '\0';
    }
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
    if (!st->device[i]) {
      break;
    }
    if (i == 63 && st->device[i]) {
      return -2;
    }
  }
  for (i = 0; i < 64; i++) {
    if (!st->video_connector[i]) {
      break;
    }
    if (i == 63 && st->video_connector[i]) {
      return -7;
    }
  }
  for (i = 0; i < 64; i++) {
    if (!st->audio_connector[i]) {
      break;
    }
    if (i == 63 && st->audio_connector[i]) {
      return -8;
    }
  }

  return 0;
}

int kr_decklink_path_info_random(struct kr_decklink_path_info *st) {
  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_decklink_path_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->device[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->device[63] = '\0';
    }
  }
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->video_connector[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->video_connector[63] = '\0';
    }
  }
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    st->audio_connector[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      st->audio_connector[63] = '\0';
    }
  }

  return 0;
}

