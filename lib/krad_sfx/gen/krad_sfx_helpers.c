#include "krad_sfx_helpers.h"

int kr_eq_band_info_init(struct kr_eq_band_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_eq_band_info));

  return 0;
}

int kr_eq_band_info_valid(struct kr_eq_band_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_eq_band_info_random(struct kr_eq_band_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_eq_band_info));

  return 0;
}

int kr_eq_info_init(struct kr_eq_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_eq_info));
  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    kr_eq_band_info_init(&st->band[i]);
  }

  return 0;
}

int kr_eq_info_valid(struct kr_eq_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    kr_eq_band_info_valid(&st->band[i]);
  }

  return 0;
}

int kr_eq_info_random(struct kr_eq_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_eq_info));
  for (i = 0; i < KR_EQ_MAX_BANDS; i++) {
    kr_eq_band_info_random(&st->band[i]);
  }

  return 0;
}

int kr_lowpass_info_init(struct kr_lowpass_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_lowpass_info));

  return 0;
}

int kr_lowpass_info_valid(struct kr_lowpass_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_lowpass_info_random(struct kr_lowpass_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_lowpass_info));

  return 0;
}

int kr_highpass_info_init(struct kr_highpass_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_highpass_info));

  return 0;
}

int kr_highpass_info_valid(struct kr_highpass_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_highpass_info_random(struct kr_highpass_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_highpass_info));

  return 0;
}

int kr_analog_info_init(struct kr_analog_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_analog_info));

  return 0;
}

int kr_analog_info_valid(struct kr_analog_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_analog_info_random(struct kr_analog_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_analog_info));

  return 0;
}

