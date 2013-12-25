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
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_eq_info_init(struct kr_eq_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_eq_info));
  kr_eq_band_info_init(&st->band);

  return 0;
}

int kr_eq_info_valid(struct kr_eq_info *st) {
  if (st == NULL) {
    return -1;
  }

  kr_eq_band_info_valid(&st->band);

  return 0;
}

int kr_eq_info_random(struct kr_eq_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_eq_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  kr_eq_band_info_random(&st->band);

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
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


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
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


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
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

