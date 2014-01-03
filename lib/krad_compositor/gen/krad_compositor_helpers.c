#include "krad_compositor_helpers.h"

int kr_compositor_path_type_to_index(int val) {
  switch (val) {
    case KR_CMP_OUTPUT:
      return 0;
    case KR_CMP_INPUT:
      return 1;
    case KR_CMP_BUS:
      return 2;
  }
  return -1;
}

char *kr_strfr_kr_compositor_path_type(int val) {
  switch (val) {
    case KR_CMP_OUTPUT:
      return "kr_cmp_output";
    case KR_CMP_INPUT:
      return "kr_cmp_input";
    case KR_CMP_BUS:
      return "kr_cmp_bus";
  }
  return NULL;
}

int kr_strto_kr_compositor_path_type(char *string) {
  if (!strcmp(string,"kr_cmp_output")) {
    return KR_CMP_OUTPUT;
  }
  if (!strcmp(string,"kr_cmp_input")) {
    return KR_CMP_INPUT;
  }
  if (!strcmp(string,"kr_cmp_bus")) {
    return KR_CMP_BUS;
  }

  return -1;
}

int kr_vector_type_to_index(int val) {
  switch (val) {
    case NOTHING:
      return 0;
    case HEX:
      return 1;
    case CIRCLE:
      return 2;
    case RECT:
      return 3;
    case TRIANGLE:
      return 4;
    case VIPER:
      return 5;
    case METER:
      return 6;
    case GRID:
      return 7;
    case CURVE:
      return 8;
    case ARROW:
      return 9;
    case CLOCK:
      return 10;
    case SHADOW:
      return 11;
  }
  return -1;
}

char *kr_strfr_kr_vector_type(int val) {
  switch (val) {
    case NOTHING:
      return "nothing";
    case HEX:
      return "hex";
    case CIRCLE:
      return "circle";
    case RECT:
      return "rect";
    case TRIANGLE:
      return "triangle";
    case VIPER:
      return "viper";
    case METER:
      return "meter";
    case GRID:
      return "grid";
    case CURVE:
      return "curve";
    case ARROW:
      return "arrow";
    case CLOCK:
      return "clock";
    case SHADOW:
      return "shadow";
  }
  return NULL;
}

int kr_strto_kr_vector_type(char *string) {
  if (!strcmp(string,"nothing")) {
    return NOTHING;
  }
  if (!strcmp(string,"hex")) {
    return HEX;
  }
  if (!strcmp(string,"circle")) {
    return CIRCLE;
  }
  if (!strcmp(string,"rect")) {
    return RECT;
  }
  if (!strcmp(string,"triangle")) {
    return TRIANGLE;
  }
  if (!strcmp(string,"viper")) {
    return VIPER;
  }
  if (!strcmp(string,"meter")) {
    return METER;
  }
  if (!strcmp(string,"grid")) {
    return GRID;
  }
  if (!strcmp(string,"curve")) {
    return CURVE;
  }
  if (!strcmp(string,"arrow")) {
    return ARROW;
  }
  if (!strcmp(string,"clock")) {
    return CLOCK;
  }
  if (!strcmp(string,"shadow")) {
    return SHADOW;
  }

  return -1;
}

int kr_compositor_controls_init(void *st) {
  struct kr_compositor_controls *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_compositor_controls *)st;
  memset(st, 0, sizeof(struct kr_compositor_controls));

  return 0;
}

int kr_compositor_controls_valid(void *st) {
  struct kr_compositor_controls *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_compositor_controls *)st;

  return 0;
}

int kr_compositor_controls_random(void *st) {
  struct kr_compositor_controls *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_compositor_controls *)st;
  memset(st, 0, sizeof(struct kr_compositor_controls));

  return 0;
}

int kr_sprite_info_init(void *st) {
  struct kr_sprite_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_sprite_info *)st;
  memset(st, 0, sizeof(struct kr_sprite_info));
  for (i = 0; i < 256; i++) {
  }
  kr_compositor_controls_init(&actual->controls);

  return 0;
}

int kr_sprite_info_valid(void *st) {
  struct kr_sprite_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_sprite_info *)st;
  for (i = 0; i < 256; i++) {
    if (!actual->filename[i]) {
      break;
    }
    if (i == 255 && actual->filename[i]) {
      return -2;
    }
  }
  kr_compositor_controls_valid(&actual->controls);

  return 0;
}

int kr_sprite_info_random(void *st) {
  struct kr_sprite_info *actual;

  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_sprite_info *)st;
  memset(st, 0, sizeof(struct kr_sprite_info));
  for (i = 0; i < 256; i++) {
    scale = (double)25 / RAND_MAX;
    actual->filename[i] = 97 + floor(rand() * scale);
    if (i == 255) {
      actual->filename[255] = '\0';
    }
  }
  kr_compositor_controls_random(&actual->controls);

  return 0;
}

int kr_text_info_init(void *st) {
  struct kr_text_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_text_info *)st;
  memset(st, 0, sizeof(struct kr_text_info));
  for (i = 0; i < 512; i++) {
  }
  for (i = 0; i < 256; i++) {
  }
  kr_compositor_controls_init(&actual->controls);

  return 0;
}

int kr_text_info_valid(void *st) {
  struct kr_text_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_text_info *)st;
  for (i = 0; i < 512; i++) {
    if (!actual->string[i]) {
      break;
    }
    if (i == 511 && actual->string[i]) {
      return -2;
    }
  }
  for (i = 0; i < 256; i++) {
    if (!actual->font[i]) {
      break;
    }
    if (i == 255 && actual->font[i]) {
      return -3;
    }
  }
  kr_compositor_controls_valid(&actual->controls);

  return 0;
}

int kr_text_info_random(void *st) {
  struct kr_text_info *actual;

  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_text_info *)st;
  memset(st, 0, sizeof(struct kr_text_info));
  for (i = 0; i < 512; i++) {
    scale = (double)25 / RAND_MAX;
    actual->string[i] = 97 + floor(rand() * scale);
    if (i == 511) {
      actual->string[511] = '\0';
    }
  }
  for (i = 0; i < 256; i++) {
    scale = (double)25 / RAND_MAX;
    actual->font[i] = 97 + floor(rand() * scale);
    if (i == 255) {
      actual->font[255] = '\0';
    }
  }
  kr_compositor_controls_random(&actual->controls);

  return 0;
}

int kr_vector_info_init(void *st) {
  struct kr_vector_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vector_info *)st;
  memset(st, 0, sizeof(struct kr_vector_info));
  kr_compositor_controls_init(&actual->controls);

  return 0;
}

int kr_vector_info_valid(void *st) {
  struct kr_vector_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vector_info *)st;
  kr_compositor_controls_valid(&actual->controls);

  return 0;
}

int kr_vector_info_random(void *st) {
  struct kr_vector_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vector_info *)st;
  memset(st, 0, sizeof(struct kr_vector_info));
  kr_compositor_controls_random(&actual->controls);

  return 0;
}

int kr_compositor_path_info_init(void *st) {
  struct kr_compositor_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_compositor_path_info *)st;
  memset(st, 0, sizeof(struct kr_compositor_path_info));
  for (i = 0; i < 128; i++) {
  }
  kr_perspective_view_init(&actual->view);
  kr_compositor_controls_init(&actual->controls);

  return 0;
}

int kr_compositor_path_info_valid(void *st) {
  struct kr_compositor_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_compositor_path_info *)st;
  for (i = 0; i < 128; i++) {
    if (!actual->name[i]) {
      break;
    }
    if (i == 127 && actual->name[i]) {
      return -2;
    }
  }
  kr_perspective_view_valid(&actual->view);
  kr_compositor_controls_valid(&actual->controls);

  return 0;
}

int kr_compositor_path_info_random(void *st) {
  struct kr_compositor_path_info *actual;

  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_compositor_path_info *)st;
  memset(st, 0, sizeof(struct kr_compositor_path_info));
  for (i = 0; i < 128; i++) {
    scale = (double)25 / RAND_MAX;
    actual->name[i] = 97 + floor(rand() * scale);
    if (i == 127) {
      actual->name[127] = '\0';
    }
  }
  kr_perspective_view_random(&actual->view);
  kr_compositor_controls_random(&actual->controls);

  return 0;
}

int kr_compositor_info_init(void *st) {
  struct kr_compositor_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_compositor_info *)st;
  memset(st, 0, sizeof(struct kr_compositor_info));

  return 0;
}

int kr_compositor_info_valid(void *st) {
  struct kr_compositor_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_compositor_info *)st;

  return 0;
}

int kr_compositor_info_random(void *st) {
  struct kr_compositor_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_compositor_info *)st;
  memset(st, 0, sizeof(struct kr_compositor_info));

  return 0;
}

