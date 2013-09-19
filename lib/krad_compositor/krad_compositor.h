#ifndef KRAD_COMPOSITOR_H
#define KRAD_COMPOSITOR_H

#include <libswscale/swscale.h>

typedef struct kr_compositor kr_compositor;
typedef struct kr_compositor_setup kr_compositor_setup;
typedef struct kr_compositor_info_cb_arg kr_compositor_info_cb_arg;

#include "krad_pool.h"
#include "krad_compositor_subunit.h"
#include "krad_compositor_path.h"
#include "krad_text.h"
#include "krad_sprite.h"
#include "krad_vector.h"
#include "krad_framepool.h"
#include "krad_compositor_interface.h"

#define KR_COMPOSITOR_WIDTH_DEF 960
#define KR_COMPOSITOR_HEIGHT_DEF 540
#define KR_COMPOSITOR_FPS_DEF 30
#define KR_COMPOSITOR_FPS_NUM_DEF KR_COMPOSITOR_FPS_DEF * 1000
#define KR_COMPOSITOR_FPS_DEN_DEF 1 * 1000

#define DEFAULT_COMPOSITOR_BUFFER_FRAMES 120
#define KC_MAX_PORTS 32
#define KC_MAX_SPRITES 32
#define KC_MAX_TEXTS 32
#define KC_MAX_VECTORS 32
#define KC_MAX_ANY MAX(KC_MAX_PORTS, MAX(KC_MAX_SPRITES, MAX(KC_MAX_TEXTS, KC_MAX_VECTORS)))
#define KC_MAX_SUBUNITS KC_MAX_PORTS * KC_MAX_SPRITES * KC_MAX_TEXTS * KC_MAX_VECTORS

#define RED 0.244 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0
#define GREY 0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255 * 1.0
#define BGCOLOR_CLR 0.0, 0.0, 0.0, 1.0

struct kr_compositor_info_cb_arg {
  void *user;
};

struct kr_compositor_setup {
  uint32_t width;
  uint32_t height;
  uint32_t fps_num;
  uint32_t fps_den;
  void *user;
  kr_compositor_info_cb *cb;
};

struct kr_compositor {
  cairo_t *cr;

  krad_frame_t *frame;
  krad_framepool_t *framepool;
  int frame_byte_size;

  int width;
  int height;
  int fps_numerator;
  int fps_denominator;

  uint64_t frames;
  uint64_t timecode;

  FT_Library ftlib;
  kr_sprite *background;

  kr_sprite *sprite;
  int active_sprites;

  kr_text *text;
  int active_texts;

  kr_vector *vector;
  int active_vectors;

  kr_pool *path_pool;
  int active_paths;
  int active_output_paths;
  int active_input_paths;
  krad_compositor_subunit_t *subunit[KC_MAX_SUBUNITS];
};

int krad_compositor_subunit_create(kr_compositor *compositor, kr_compositor_subunit_t type, char *option, char *option2);
int krad_compositor_subunit_destroy(kr_compositor *compositor, kr_address_t *address);
void krad_compositor_subunit_update(kr_compositor *compositor, kr_unit_control_t *uc);

int kr_compositor_background(kr_compositor *compositor, char **filename);
void kr_compositor_background_set(kr_compositor *compositor, char *filename);
void kr_compositor_resolution_set(kr_compositor *comp, uint32_t w, uint32_t h);

/* Below looks good, above replace */

void kr_compositor_setup_init(kr_compositor_setup *setup);
kr_compositor *kr_compositor_create(kr_compositor_setup *setup);
int kr_compositor_destroy(kr_compositor *compositor);
int kr_compositor_process(kr_compositor *compositor);

/*
 ***Compositor as a whole
 int kr_compositor_ctl(kr_compositor *compr, XXX);
 int kr_compositor_get_info(kr_compositor *compr, kr_compositor_info *info);
*/

#endif
