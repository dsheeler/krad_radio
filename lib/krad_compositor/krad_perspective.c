#include "krad_perspective.h"

static void sub_vec2 (kr_coord_t *r, kr_coord_t *a, kr_coord_t *b) {
  r->x = a->x - b->x;
  r->y = a->y - b->y;
}

static void add_vec2 (kr_coord_t *r, kr_coord_t *a, kr_coord_t *b) {
  r->x = a->x + b->x;
  r->y = a->y + b->y;
}

static void mul_vec2 (kr_coord_t* r, kr_coord_t* a, double scalar) {
  r->x = a->x * scalar;
  r->y = a->y * scalar;
}

static void get_pixel_position (kr_coord_t* r, kr_coord_t* t,
                                kr_coord_t* b, kr_coord_t* tl,
                                kr_coord_t* bl,kr_coord_t* in ) {

  kr_coord_t t_x;
  kr_coord_t b_x;
  kr_coord_t k;

  mul_vec2 (&t_x, t, in->x);
  mul_vec2 (&b_x, b, in->x);
  add_vec2 (&t_x, &t_x, tl);
  add_vec2 (&b_x, &b_x, bl);
  sub_vec2 (&k, &b_x, &t_x);
  mul_vec2 (&k, &k, in->y);
  add_vec2 (r, &k, &t_x);
}

static void perspective_map (kr_perspective_t *perspective) {

  int32_t *map;
  kr_coord_t top;
  kr_coord_t bot;
  kr_coord_t r;
  kr_coord_t in;

  int32_t rx;
  int32_t ry;
  int32_t x;
  int32_t y;
  int32_t w;
  int32_t h;

  w = perspective->width;
  h = perspective->height;  
  map = perspective->map;

  perspective->tl.x = 0.0;
  perspective->tl.y = 0.0;

  perspective->tr.x = 1.0;
  perspective->tr.y = 0.0;

  perspective->bl.x = 0.0;
  perspective->bl.y = 1.0;

  perspective->br.x = 1.0;
  perspective->br.y = 1.0;
  
  sub_vec2 (&top, &perspective->tr, &perspective->tl);
  sub_vec2 (&bot, &perspective->br, &perspective->bl);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      in.x = (double)x / (double)w;
      in.y = (double)y / (double)h;
      get_pixel_position (&r, &top, &bot,
                          &perspective->tl, &perspective->bl, &in );
      rx = lrint(r.x * (float)w);
      ry = lrint(r.y * (float)h);
      if (rx < 0 || rx >= w || ry < 0 || ry >= h) {
        continue;
      }
      map[x + w * y] = rx + w * ry;
    }
  }
}

kr_perspective_t *kr_perspective_create (uint32_t width, uint32_t height) {

  kr_perspective_t *perspective;

  perspective = calloc (1, sizeof(kr_perspective_t));

  perspective->width = width;
  perspective->height = height;

  perspective->map = calloc (1, perspective->width * perspective->height * 4);

  perspective_map (perspective);

  return perspective;
}

int32_t kr_perspective_destroy (kr_perspective_t **perspective) {

  if ((perspective == NULL) || (*perspective == NULL)) {
    return -1;
  }

  free ((*perspective)->map);
  free (*perspective);
  return 0;
}

void kr_perspective (kr_perspective_t *perspective,
                     uint32_t *out,
                     uint32_t *in) {

  uint32_t w;
  uint32_t h;
  uint32_t x;
  uint32_t y;

  w = perspective->width;
  h = perspective->height; 
  
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      out[x + w * y] = in[perspective->map[x + w * y]];
    }
  }
}
