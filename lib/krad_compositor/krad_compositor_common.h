#ifndef KRAD_COMPOSITOR_COMMON_H
#define KRAD_COMPOSITOR_COMMON_H

#define KRAD_COMPOSITOR_SUBUNIT_DEFAULT_TICKRATE 4

#include "krad_radio_ipc.h"
#include "krad_system.h"
#include "krad_perspective.h"

typedef enum {
  KR_CMP_OUTPUT = 666,
  KR_CMP_INPUT,
  KR_CMP_BUS
} kr_compositor_path_type;

typedef enum {
  KR_VIDEOPORT = 31,
  KR_SPRITE,
  KR_TEXT,
  KR_VECTOR
} kr_compositor_subunit_type;

typedef enum {
  NOTHING,
  HEX,
  CIRCLE,
  RECT,
  TRIANGLE,
  VIPER,
  METER,
  GRID,
  CURVE,
  ARROW,
  CLOCK,
  SHADOW
} kr_vector_type;

typedef enum {
  KR_NO,
  KR_X,
  KR_Y,
  KR_Z,
  KR_WIDTH,
  KR_HEIGHT,
  KR_ROTATION,
  KR_OPACITY,
  KR_RED,
  KR_GREEN,
  KR_BLUE,
  KR_ALPHA,
  KR_TICKRATE,
  KR_VIEW_TL_X,
  KR_VIEW_TL_Y,
  KR_VIEW_TR_X,
  KR_VIEW_TR_Y,
  KR_VIEW_BL_X,
  KR_VIEW_BL_Y,
  KR_VIEW_BR_X,
  KR_VIEW_BR_Y
} kr_compositor_control;

typedef struct kr_text_info kr_text_info;
typedef struct kr_sprite_info kr_sprite_info;
typedef struct kr_vector_info kr_vector_info;
typedef struct kr_compositor_path_info kr_compositor_path_info;
typedef struct kr_compositor_controls kr_compositor_controls;
typedef struct kr_compositor_info kr_compositor_info;

struct kr_compositor_controls {
  int32_t x;
  int32_t y;
  uint32_t z;
  uint32_t w;
  uint32_t h;
  float rotation;
  float opacity;
};

struct kr_sprite_info {
  char filename[256];
  int32_t tickrate;
  kr_compositor_controls controls;
};

struct kr_text_info {
  char text[1024];
  char font[128];
  float red;
  float green;
  float blue;
  kr_compositor_controls controls;
};

struct kr_vector_info {
  kr_vector_type type;
  float red;
  float green;
  float blue;
  kr_compositor_controls controls;
};

struct kr_compositor_path_info {
  char name[128];
  kr_compositor_path_type type;
  /* Source / Dest res */
  uint32_t width;
  uint32_t height;
  uint32_t crop_x;
  uint32_t crop_y;
  uint32_t crop_width;
  uint32_t crop_height;
  kr_perspective_view view;
  kr_compositor_controls controls;
};

struct kr_compositor_info {
  uint32_t width;
  uint32_t height;
  uint32_t fps_numerator;
  uint32_t fps_denominator;
  uint32_t sprites;
  uint32_t vectors;
  uint32_t texts;
  uint32_t inputs;
  uint32_t outputs;
  uint64_t frames;
  uint64_t timecode;
  char background_filename[256];
};

void kr_aspect_upscale(int srcw, int srch, int dstw, int dsth, int *w, int *h);

char *kr_compositor_control_to_string(kr_compositor_control control);
char *kr_compositor_subunit_type_to_string(kr_compositor_subunit_type type);
char *kr_vector_type_to_string(kr_vector_type type);
kr_vector_type kr_string_to_vector_type(char *string);
kr_compositor_control kr_string_to_compositor_control(char *string);
kr_compositor_subunit_type kr_string_to_subunit_type(char *string);

#define kr_comp_strfsubtype kr_compositor_subunit_type_to_string

#endif
