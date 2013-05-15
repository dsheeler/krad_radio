#ifndef KRAD_COMPOSITOR_COMMON_H
#define KRAD_COMPOSITOR_COMMON_H

#define KRAD_COMPOSITOR_SUBUNIT_DEFAULT_TICKRATE 4

#include "krad_radio_ipc.h"
#include "krad_system.h"

typedef enum {
  KR_VIDEOPORT = 31,
  KR_SPRITE,
  KR_TEXT,
  KR_VECTOR,
} kr_compositor_subunit_t;

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
  SHADOW,
} krad_vector_type_t;

typedef enum {
  KR_NO,
  KR_X,
  KR_Y,
  KR_Z,
  KR_WIDTH,
  KR_HEIGHT,
  KR_ROTATION,
  KR_OPACITY,
  KR_XSCALE,
  KR_YSCALE,
  KR_RED,
  KR_GREEN,
  KR_BLUE,
  KR_ALPHA,
  KR_TICKRATE,
} kr_compositor_control_t;

typedef struct krad_text_rep_St krad_text_rep_t;
typedef struct krad_text_rep_St kr_text_t;
typedef struct krad_sprite_rep_St krad_sprite_rep_t;
typedef struct krad_sprite_rep_St kr_sprite_t;
typedef struct krad_vector_rep_St krad_vector_rep_t;
typedef struct krad_vector_rep_St kr_vector_t;
typedef struct krad_port_rep_St krad_port_rep_t;
typedef struct krad_port_rep_St kr_port_t;
typedef struct kr_compositor_subunit_controls_St kr_compositor_subunit_controls_t;
typedef struct kr_compositor_subunit_controls_St kr_comp_controls_t;
typedef struct krad_compositor_rep_St krad_compositor_rep_t;
typedef struct krad_compositor_rep_St kr_compositor_t;

struct kr_compositor_subunit_controls_St {
  int32_t x;
  int32_t y;
  uint32_t z;
  int32_t tickrate;
  uint32_t width;
  uint32_t height;
  float xscale;
  float yscale;
  float rotation;
  float opacity;
};

struct krad_sprite_rep_St {
  char filename[256];
  kr_comp_controls_t controls;
};

struct krad_text_rep_St {
  char text[1024];
  char font[128];
  float red;
  float green;
  float blue;
  kr_comp_controls_t controls;
};

struct krad_vector_rep_St {
  krad_vector_type_t type;
  float red;
  float green;
  float blue;
  kr_comp_controls_t controls;
};

struct krad_port_rep_St {
  char sysname[128];
  int32_t direction;
  uint32_t source_width;
  uint32_t source_height;
  uint32_t crop_x;
  uint32_t crop_y;
  uint32_t crop_width;
  uint32_t crop_height;
  kr_comp_controls_t controls;
};

struct krad_compositor_rep_St {
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
  char background_filename[256];
};
char *kr_compositor_control_to_string (kr_compositor_control_t control);
char *kr_compositor_subunit_type_to_string (kr_compositor_subunit_t type);
krad_vector_type_t krad_string_to_vector_type (char *string);
kr_compositor_control_t krad_string_to_compositor_control (char *string);
char *krad_vector_type_to_string (krad_vector_type_t type);
kr_compositor_subunit_t kr_string_to_comp_subunit_type (char *string);
#endif // KRAD_COMPOSITOR_COMMON_H
