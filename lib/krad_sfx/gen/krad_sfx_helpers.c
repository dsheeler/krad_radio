#include "krad_sfx_helpers.h"

int kr_eq_band_info_init(void *st) {
  struct kr_eq_band_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_eq_band_info *)st;
  memset(actual, 0, sizeof(struct kr_eq_band_info));

  return 0;
}

int kr_eq_band_info_valid(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_eq_band_info_random(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_eq_info_init(void *st) {
  struct kr_eq_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_eq_info *)st;
  memset(actual, 0, sizeof(struct kr_eq_info));
  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    kr_eq_band_info_init(&actual->band[i]);
  }

  return 0;
}

int kr_eq_info_valid(void *st) {
  struct kr_eq_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_eq_info *)st;
  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    kr_eq_band_info_valid(&actual->band[i]);
  }

  return 0;
}

int kr_eq_info_random(void *st) {
  struct kr_eq_info *actual;

  int i;
  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_eq_info *)st;
  memset(st, 0, sizeof(struct kr_eq_info));
  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    kr_eq_band_info_random(&actual->band[i]);
  }

  return 0;
}

int kr_lowpass_info_init(void *st) {
  struct kr_lowpass_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_lowpass_info *)st;
  memset(actual, 0, sizeof(struct kr_lowpass_info));

  return 0;
}

int kr_lowpass_info_valid(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_lowpass_info_random(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_highpass_info_init(void *st) {
  struct kr_highpass_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_highpass_info *)st;
  memset(actual, 0, sizeof(struct kr_highpass_info));

  return 0;
}

int kr_highpass_info_valid(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_highpass_info_random(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_analog_info_init(void *st) {
  struct kr_analog_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_analog_info *)st;
  memset(actual, 0, sizeof(struct kr_analog_info));

  return 0;
}

int kr_analog_info_valid(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_analog_info_random(void *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

