#ifndef KRAD_COMPOSITOR_COMMON_H
#define KRAD_COMPOSITOR_COMMON_H

#define KRAD_COMPOSITOR_SUBUNIT_DEFAULT_TICKRATE 4

#include "krad_radio_ipc.h"
#include "krad_ebml.h"

typedef enum {
  KR_VIDEOPORT = 1,
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
typedef struct kr_compositor_subunit_controls_St kr_compositor_subunit_controls_t;
typedef struct kr_compositor_subunit_controls_St kr_comp_controls_t;
typedef struct krad_compositor_rep_St krad_compositor_rep_t;
typedef struct krad_compositor_rep_St kr_compositor_t;

struct kr_compositor_subunit_controls_St {

  int number;

  int x;
  int y;
  int z;

  int tickrate;
  
  int width;
  int height;

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
  
  krad_vector_type_t krad_vector_type;
  
  float red;
  float green;
  float blue;
  
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
  
  char snapshot_filename[256];
  char background_filename[256];
};

void krad_compositor_subunit_controls_init (kr_comp_controls_t *controls, int number, int x, int y, int z, int tickrate, int width, int height, float scale, float opacity, float rotation);
void krad_compositor_subunit_controls_reset (kr_comp_controls_t *krad_compositor_subunit_controls);
void krad_compositor_subunit_controls_to_ebml (krad_ebml_t *krad_ebml, kr_comp_controls_t *krad_compositor_subunit_controls);
void krad_compositor_subunit_controls_read (krad_ebml_t *krad_ebml, kr_comp_controls_t *subunit_controls);

char *kr_compositor_subunit_type_to_string (kr_compositor_subunit_t type);

krad_text_rep_t *krad_compositor_text_rep_create ();
krad_text_rep_t *krad_compositor_text_rep_create_and_init (int number, char *text, char *font, float red, float green, float blue, int x, int y, int z, int tickrate, float scale, float opacity, float rotation);
krad_text_rep_t *krad_compositor_ebml_to_krad_text_rep (krad_ebml_t *krad_ebml, uint64_t *ebml_data_size, krad_text_rep_t *krad_text_rep);
void krad_compositor_text_rep_destroy (krad_text_rep_t *krad_text_rep);
void krad_compositor_validate_text_rep (krad_text_rep_t *krad_text_rep);
void krad_compositor_text_rep_to_ebml (krad_text_rep_t *krad_text_rep, krad_ebml_t *krad_ebml);
void krad_compositor_text_rep_reset (krad_text_rep_t *krad_text_rep);

kr_sprite_t *kr_compositor_sprite_rep_create ();
void kr_compositor_sprite_rep_destroy (kr_sprite_t *sprite);

krad_sprite_rep_t *krad_compositor_ebml_to_new_krad_sprite_rep (krad_ebml_t *krad_ebml, uint64_t *bytes_read);
krad_sprite_rep_t *krad_compositor_sprite_rep_create_and_init ( int number, char *filename, int x, int y, int z, int tickrate, float scale, float opacity, float rotation);
void krad_compositor_sprite_rep_to_ebml (krad_sprite_rep_t *sprite, krad_ebml_t *ebml);


krad_vector_rep_t *krad_compositor_vector_rep_create ();
krad_vector_rep_t *krad_compositor_vector_rep_create_and_init (int number, char *vector_type, float red, float green, float blue, int x, int y, int z, int tickrate, float scale, float opacity, float rotation);
krad_vector_rep_t *krad_compositor_ebml_to_krad_vector_rep (krad_ebml_t *krad_ebml, uint64_t *ebml_data_size, krad_vector_rep_t *krad_vector_rep);
void krad_compositor_vector_rep_destroy (krad_vector_rep_t *krad_vector_rep);
void krad_compositor_validate_vector_rep (krad_vector_rep_t *krad_vector_rep);
void krad_compositor_vector_rep_to_ebml (krad_vector_rep_t *krad_vector_rep, krad_ebml_t *krad_ebml);
void krad_compositor_vector_rep_reset (krad_vector_rep_t *krad_vector_rep);
krad_vector_type_t krad_string_to_vector_type (char *string);
kr_compositor_control_t krad_string_to_compositor_control (char *string);
char *krad_vector_type_to_string (krad_vector_type_t type);

kr_compositor_t *kr_compositor_rep_create ();
void kr_compositor_rep_destroy (kr_compositor_t *compositor);

#endif // KRAD_COMPOSITOR_COMMON_H
