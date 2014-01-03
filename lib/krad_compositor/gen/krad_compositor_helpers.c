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

int kr_compositor_controls_init(struct kr_compositor_controls *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_compositor_controls));

  return 0;
}

int kr_compositor_controls_valid(struct kr_compositor_controls *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_compositor_controls_random(struct kr_compositor_controls *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_compositor_controls));

  return 0;
}

int kr_sprite_info_init(struct kr_sprite_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_sprite_info));
  for (i = 0; i < 256; i++) {
  }
  kr_compositor_controls_init(&st->controls);

  return 0;
}

int kr_sprite_info_valid(struct kr_sprite_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 256; i++) {
  }
  kr_compositor_controls_valid(&st->controls);

  return 0;
}

int kr_sprite_info_random(struct kr_sprite_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_sprite_info));
  for (i = 0; i < 256; i++) {
  }
  kr_compositor_controls_random(&st->controls);

  return 0;
}

int kr_text_info_init(struct kr_text_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_text_info));
  for (i = 0; i < 512; i++) {
  }
  for (i = 0; i < 256; i++) {
  }
  kr_compositor_controls_init(&st->controls);

  return 0;
}

int kr_text_info_valid(struct kr_text_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 512; i++) {
  }
  for (i = 0; i < 256; i++) {
  }
  kr_compositor_controls_valid(&st->controls);

  return 0;
}

int kr_text_info_random(struct kr_text_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_text_info));
  for (i = 0; i < 512; i++) {
  }
  for (i = 0; i < 256; i++) {
  }
  kr_compositor_controls_random(&st->controls);

  return 0;
}

int kr_vector_info_init(struct kr_vector_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vector_info));
  kr_compositor_controls_init(&st->controls);

  return 0;
}

int kr_vector_info_valid(struct kr_vector_info *st) {
  if (st == NULL) {
    return -1;
  }

  kr_compositor_controls_valid(&st->controls);

  return 0;
}

int kr_vector_info_random(struct kr_vector_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vector_info));
  kr_compositor_controls_random(&st->controls);

  return 0;
}

int kr_compositor_path_info_init(struct kr_compositor_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_compositor_path_info));
  for (i = 0; i < 128; i++) {
  }
  kr_perspective_view_init(&st->view);
  kr_compositor_controls_init(&st->controls);

  return 0;
}

int kr_compositor_path_info_valid(struct kr_compositor_path_info *st) {
  int i;

  if (st == NULL) {
    return -1;
  }

  for (i = 0; i < 128; i++) {
  }
  kr_perspective_view_valid(&st->view);
  kr_compositor_controls_valid(&st->controls);

  return 0;
}

int kr_compositor_path_info_random(struct kr_compositor_path_info *st) {
  int i;
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_compositor_path_info));
  for (i = 0; i < 128; i++) {
  }
  kr_perspective_view_random(&st->view);
  kr_compositor_controls_random(&st->controls);

  return 0;
}

int kr_compositor_info_init(struct kr_compositor_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_compositor_info));

  return 0;
}

int kr_compositor_info_valid(struct kr_compositor_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_compositor_info_random(struct kr_compositor_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_compositor_info));

  return 0;
}

