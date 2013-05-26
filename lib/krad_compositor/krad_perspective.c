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

int32_t kr_perspective_set (kr_perspective_t *perspective,
                            kr_perspective_view_t *view) {

  if ((perspective == NULL) || (view == NULL)) {
    return -1;
  }

  printf ("%u-%u\n", view->top_left.x, view->top_left.y);
  printf ("%u-%u\n", view->top_right.x, view->top_right.y);
  printf ("%u-%u\n", view->bottom_left.x, view->bottom_left.y);
  printf ("%u-%u\n", view->bottom_right.x, view->bottom_right.y);

  if (view->top_left.x >= perspective->width) {
    return -2;
  }
  if (view->top_left.y >= perspective->height) {
    return -2;
  }
  if (view->top_right.x >= perspective->width) {
    return -2;
  }
  if (view->top_right.y >= perspective->height) {
    return -2;
  }
  if (view->bottom_left.x >= perspective->width) {
    return -2;
  }
  if (view->bottom_left.y >= perspective->height) {
    return -2;
  }
  if (view->bottom_right.x >= perspective->width) {
    return -2;
  }
  if (view->bottom_right.y >= perspective->height) {
    return -2;
  }

  if (view->top_left.x >= view->top_right.x) {
    return -3;
  }
  if (view->top_left.y >= view->bottom_left.y) {
    return -3;
  }

  memcpy (&perspective->view, view, sizeof(kr_perspective_view_t));

  if (view->top_left.x == 0) {
    perspective->tl.x = 0;
  } else {
    perspective->tl.x = view->top_left.x/(double)(perspective->width - 1);
  }

  if (view->top_left.y == 0) {
    perspective->tl.y = 0;
  } else {
    perspective->tl.y = view->top_left.y/(double)(perspective->height - 1);
  }

  perspective->tr.x = view->top_right.x/(double)(perspective->width - 1);

  if (view->top_right.y == 0) {
    perspective->tr.y = 0;
  } else {
    perspective->tr.y = view->top_right.y/(double)(perspective->height - 1);
  }

  if (view->bottom_left.x == 0) {
    perspective->bl.x = 0;
  } else {
    perspective->bl.x = view->bottom_left.x/(double)(perspective->width - 1);
  }

  perspective->bl.y = view->bottom_left.y/(double)(perspective->height - 1);

  perspective->br.x = view->bottom_right.x/(double)(perspective->width - 1);
  perspective->br.y = view->bottom_right.y/(double)(perspective->height - 1);


  printf ("%f-%f\n", perspective->tl.x, perspective->tl.y);
  printf ("%f-%f\n", perspective->tr.x, perspective->tr.y);
  printf ("%f-%f\n", perspective->bl.x, perspective->bl.y);
  printf ("%f-%f\n", perspective->br.x, perspective->br.y);

  perspective_map (perspective);

  return 0;
}

static void kr_perspective_set_default (kr_perspective_t *perspective) {

  kr_perspective_view_t view;

  view.top_left.x = 0;
  view.top_left.y = 0;
  view.top_right.x = perspective->width - 1;
  view.top_right.y = 0;
  view.bottom_left.x = 0;
  view.bottom_left.y = perspective->height - 1;
  view.bottom_right.x = perspective->width - 1;
  view.bottom_right.y = perspective->height - 1;

  kr_perspective_set (perspective, &view);
}

kr_perspective_t *kr_perspective_create (uint32_t width, uint32_t height) {

  kr_perspective_t *perspective;

  perspective = calloc (1, sizeof(kr_perspective_t));

  perspective->width = width;
  perspective->height = height;

  perspective->map = calloc (1, perspective->width * perspective->height * 4);

  kr_perspective_set_default (perspective);

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
