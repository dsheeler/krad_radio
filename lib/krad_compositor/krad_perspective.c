#include "krad_perspective.h"

typedef struct kr_coord kr_coord;

struct kr_coord {
  double x;
  double y;
};

struct kr_perspective_priv {
  int32_t *map;
  kr_coord tl;
  kr_coord tr;
  kr_coord bl;
  kr_coord br;
  uint32_t width;
  uint32_t height;
  kr_timer *map_timer;
  kr_timer *run_timer;
};

static void sub_vec2(kr_coord *r, kr_coord *a, kr_coord *b) {
  r->x = a->x - b->x;
  r->y = a->y - b->y;
}

static void add_vec2(kr_coord *r, kr_coord *a, kr_coord *b) {
  r->x = a->x + b->x;
  r->y = a->y + b->y;
}

static void mul_vec2(kr_coord* r, kr_coord* a, double scalar) {
  r->x = a->x * scalar;
  r->y = a->y * scalar;
}

static void get_pixel_position(kr_coord* r, kr_coord* t, kr_coord* b,
 kr_coord* tl, kr_coord* bl,kr_coord* in ) {

  kr_coord t_x;
  kr_coord b_x;
  kr_coord k;

  mul_vec2(&t_x, t, in->x);
  mul_vec2(&b_x, b, in->x);
  add_vec2(&t_x, &t_x, tl);
  add_vec2(&b_x, &b_x, bl);
  sub_vec2(&k, &b_x, &t_x);
  mul_vec2(&k, &k, in->y);
  add_vec2(r, &k, &t_x);
}

static void perspective_map(kr_perspective *perspective) {

  int32_t *map;
  kr_coord tl;
  kr_coord bl;
  kr_coord top;
  kr_coord bot;
  kr_coord r;
  kr_coord in;

  int32_t rx;
  int32_t ry;
  int32_t x;
  int32_t y;
  int32_t w;
  int32_t h;

  //kr_timer_start(perspective->priv->map_timer);

  w = perspective->priv->width;
  h = perspective->priv->height;
  tl = perspective->priv->tl;
  bl = perspective->priv->bl;
  map = perspective->priv->map;

  sub_vec2(&top, &perspective->priv->tr, &tl);
  sub_vec2(&bot, &perspective->priv->br, &bl);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      in.x = (double)x / (double)w;
      in.y = (double)y / (double)h;
      get_pixel_position(&r, &top, &bot, &tl, &bl, &in );
      rx = lrint(r.x * (float)w);
      ry = lrint(r.y * (float)h);
      if (rx < 0 || rx >= w || ry < 0 || ry >= h) {
        continue;
      }
      map[x + w * y] = rx + w * ry;
    }
  }
  //kr_timer_status (perspective->priv->map_timer);
}

int32_t kr_perspective_set(kr_perspective *p, kr_perspective_view *view) {
  if ((p == NULL) || (view == NULL)) {
    return -1;
  }
/*printk("%u-%u\n", view->top_left.x, view->top_left.y);
  printk("%u-%u\n", view->top_right.x, view->top_right.y);
  printk("%u-%u\n", view->bottom_left.x, view->bottom_left.y);
  printk("%u-%u\n", view->bottom_right.x, view->bottom_right.y);*/
  if (view->top_left.x >= p->width) {
    return -2;
  }
  if (view->top_left.y >= p->height) {
    return -2;
  }
  if (view->top_right.x >= p->width) {
    return -2;
  }
  if (view->top_right.y >= p->height) {
    return -2;
  }
  if (view->bottom_left.x >= p->width) {
    return -2;
  }
  if (view->bottom_left.y >= p->height) {
    return -2;
  }
  if (view->bottom_right.x >= p->width) {
    return -2;
  }
  if (view->bottom_right.y >= p->height) {
    return -2;
  }
  if (view->top_left.x >= view->top_right.x) {
    return -3;
  }
  if (view->top_left.y >= view->bottom_left.y) {
    return -3;
  }

  memcpy(&p->view, view, sizeof(kr_perspective_view));
  if (view->top_left.x == 0) {
    p->priv->tl.x = 0;
  } else {
    p->priv->tl.x = view->top_left.x/(double)(p->width - 1);
  }
  if (view->top_left.y == 0) {
    p->priv->tl.y = 0;
  } else {
    p->priv->tl.y = view->top_left.y/(double)(p->height - 1);
  }
  p->priv->tr.x = view->top_right.x/(double)(p->width - 1);
  if (view->top_right.y == 0) {
    p->priv->tr.y = 0;
  } else {
    p->priv->tr.y = view->top_right.y/(double)(p->height - 1);
  }
  if (view->bottom_left.x == 0) {
    p->priv->bl.x = 0;
  } else {
    p->priv->bl.x = view->bottom_left.x/(double)(p->width - 1);
  }
  p->priv->bl.y = view->bottom_left.y/(double)(p->height - 1);
  p->priv->br.x = view->bottom_right.x/(double)(p->width - 1);
  p->priv->br.y = view->bottom_right.y/(double)(p->height - 1);
/*printk("%f-%f\n", p->priv->tl.x, p->priv->tl.y);
  printk("%f-%f\n", p->priv->tr.x, p->priv->tr.y);
  printk("%f-%f\n", p->priv->bl.x, p->priv->bl.y);
  printk("%f-%f\n", p->priv->br.x, p->priv->br.y);*/
  perspective_map(p);
  return 0;
}

static void kr_perspective_set_default (kr_perspective *perspective) {

  kr_perspective_view view;

  view.top_left.x = 0;
  view.top_left.y = 0;
  view.top_right.x = perspective->width - 1;
  view.top_right.y = 0;
  view.bottom_left.x = 0;
  view.bottom_left.y = perspective->height - 1;
  view.bottom_right.x = perspective->width - 1;
  view.bottom_right.y = perspective->height - 1;

  kr_perspective_set(perspective, &view);
}

void kr_perspective_rand (kr_perspective *perspective) {

  kr_perspective_view view;
  uint32_t halfwidth;
  uint32_t halfheight;

  halfwidth = (perspective->width / 2) - 1;
  halfheight = (perspective->height / 2) - 1;

  view.top_left.x = rand() % halfwidth;
  view.top_left.y = rand() % halfheight;
  view.top_right.x = (rand() % halfwidth) + halfwidth;
  view.top_right.y = (rand() % halfheight);
  view.bottom_left.x = rand() % halfwidth;
  view.bottom_left.y = (rand() % halfheight) + halfheight;
  view.bottom_right.x = (rand() % halfwidth) + halfwidth;
  view.bottom_right.y = (rand() % halfheight) + halfheight;

  kr_perspective_set(perspective, &view);
}

kr_perspective *kr_perspective_create(uint32_t width, uint32_t height) {

  kr_perspective *p;

  p = calloc(1, sizeof(kr_perspective));

  p->width = width;
  p->height = height;
  p->priv = calloc (1, sizeof(kr_perspective_priv));
  p->priv->map = calloc(1, p->width * p->height * 4);

  p->priv->width = p->width;
  p->priv->height = p->height;

  p->priv->map_timer = kr_timer_create_with_name("Perspective Map");
  p->priv->run_timer = kr_timer_create_with_name("Perspective Run");

  kr_perspective_set_default(p);

  return p;
}

int32_t kr_perspective_destroy(kr_perspective **perspective) {
  if ((perspective == NULL) || (*perspective == NULL)) {
    return -1;
  }
  kr_timer_destroy((*perspective)->priv->map_timer);
  kr_timer_destroy((*perspective)->priv->run_timer);
  free((*perspective)->priv->map);
  free((*perspective)->priv);
  free(*perspective);
  return 0;
}

int32_t kr_perspective_argb(kr_perspective *p, uint8_t *out, uint8_t *in) {

  uint32_t w;
  uint32_t h;
  uint32_t x;
  uint32_t y;
  uint32_t *outpx;
  uint32_t *inpx;
  int32_t *map;

  if ((p == NULL) || (out == NULL) || (in == NULL)) {
    return -1;
  }
  //kr_timer_start (perspective->priv->run_timer);
  w = p->priv->width;
  h = p->priv->height;
  inpx = (uint32_t *)in;
  outpx = (uint32_t *)out;
  map = p->priv->map;
  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      outpx[x + w * y] = inpx[map[x + w * y]];
    }
  }
  //kr_timer_status(perspective->priv->run_timer);
  return 0;
}

int32_t kr_perspective_apply(kr_perspective *p, kr_image *out, kr_image *in) {
  if ((p == NULL) || (out == NULL) || (in == NULL)) {
    return -1;
  }
  if ((out->px == NULL) || (in->px == NULL)) {
    return -2;
  }
  if ((in->w != p->width) || (in->h != p->height)) {
    return -3;
  }
  if ((out->w != p->width) || (out->h != p->height)) {
    return -4;
  }
  return kr_perspective_argb(p, out->px, in->px);
}
