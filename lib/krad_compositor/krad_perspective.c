#include "krad_perspective.h"

typedef struct krad_perspective_priv_St kr_perspective_priv_t;
typedef struct krad_position_St krad_position_t;
typedef struct krad_position_St kr_coord_t;

struct krad_position_St {
  double x;
  double y;
};

struct krad_perspective_priv_St {
  int32_t *map;
  krad_position_t tl;
  krad_position_t tr;
  krad_position_t bl;
  krad_position_t br;
  uint32_t width;
  uint32_t height;
  krad_timer_t *map_timer;
  krad_timer_t *run_timer;
};

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
  kr_coord_t tl;
  kr_coord_t bl;
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

  //krad_timer_start (perspective->priv->map_timer);

  w = perspective->priv->width;
  h = perspective->priv->height;
  tl = perspective->priv->tl;
  bl = perspective->priv->bl;
  map = perspective->priv->map;
  
  sub_vec2 (&top, &perspective->priv->tr, &tl);
  sub_vec2 (&bot, &perspective->priv->br, &bl);

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      in.x = (double)x / (double)w;
      in.y = (double)y / (double)h;
      get_pixel_position (&r, &top, &bot,
                          &tl, &bl, &in );
      rx = lrint (r.x * (float)w);
      ry = lrint (r.y * (float)h);
      if (rx < 0 || rx >= w || ry < 0 || ry >= h) {
        continue;
      }
      map[x + w * y] = rx + w * ry;
    }
  }

  //krad_timer_status (perspective->priv->map_timer);
}

int32_t kr_perspective_set (kr_perspective_t *perspective,
                            kr_perspective_view_t *view) {

  //  perspective->width = 960;
  //perspective->height = 540;
  if ((perspective == NULL) || (view == NULL)) {
    return -1;
  }
  
  printk ("%u-%u\n", view->top_left.x, view->top_left.y);
  printk ("%u-%u\n", view->top_right.x, view->top_right.y);
  printk ("%u-%u\n", view->bottom_left.x, view->bottom_left.y);
  printk ("%u-%u\n", view->bottom_right.x, view->bottom_right.y);

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
    perspective->priv->tl.x = 0;
  } else {
    perspective->priv->tl.x = view->top_left.x/(double)(perspective->width - 1);
  }

  if (view->top_left.y == 0) {
    perspective->priv->tl.y = 0;
  } else {
    perspective->priv->tl.y = view->top_left.y/(double)(perspective->height - 1);
  }

  perspective->priv->tr.x = view->top_right.x/(double)(perspective->width - 1);

  if (view->top_right.y == 0) {
    perspective->priv->tr.y = 0;
  } else {
    perspective->priv->tr.y = view->top_right.y/(double)(perspective->height - 1);
  }

  if (view->bottom_left.x == 0) {
    perspective->priv->bl.x = 0;
  } else {
    perspective->priv->bl.x = view->bottom_left.x/(double)(perspective->width - 1);
  }

  perspective->priv->bl.y = view->bottom_left.y/(double)(perspective->height - 1);

  perspective->priv->br.x = view->bottom_right.x/(double)(perspective->width - 1);
  perspective->priv->br.y = view->bottom_right.y/(double)(perspective->height - 1);


  printk ("%f-%f\n", perspective->priv->tl.x, perspective->priv->tl.y);
  printk ("%f-%f\n", perspective->priv->tr.x, perspective->priv->tr.y);
  printk ("%f-%f\n", perspective->priv->bl.x, perspective->priv->bl.y);
  printk ("%f-%f\n", perspective->priv->br.x, perspective->priv->br.y);

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

void kr_perspective_random (kr_perspective_t *perspective) {

  kr_perspective_view_t view;
  uint32_t hwidth;
  uint32_t hheight;

  hwidth = (perspective->width / 2) - 1;
  hheight = (perspective->height / 2) - 1;

  view.top_left.x = rand() % hwidth;
  view.top_left.y = rand() % hheight;
  view.top_right.x = (rand() % hwidth) + hwidth;
  view.top_right.y = (rand() % hheight);
  view.bottom_left.x = rand() % hwidth;
  view.bottom_left.y = (rand() % hheight) + hheight;
  view.bottom_right.x = (rand() % hwidth) + hwidth;
  view.bottom_right.y = (rand() % hheight) + hheight;

  kr_perspective_set (perspective, &view);
}

kr_perspective_t *kr_perspective_create (uint32_t width, uint32_t height) {

  kr_perspective_t *perspective;

  perspective = calloc (1, sizeof(kr_perspective_t));

  perspective->width = width;
  perspective->height = height;
  perspective->priv = calloc (1, sizeof(kr_perspective_priv_t));
  perspective->priv->map = calloc (1, perspective->width * perspective->height * 4);

  perspective->priv->width = perspective->width;
  perspective->priv->height = perspective->height;

  perspective->priv->map_timer = krad_timer_create_with_name ("Perspective Map");
  perspective->priv->run_timer = krad_timer_create_with_name ("Perspective Run");

  kr_perspective_set_default (perspective);

  return perspective;
}

int32_t kr_perspective_destroy (kr_perspective_t **perspective) {

  if ((perspective == NULL) || (*perspective == NULL)) {
    return -1;
  }

  krad_timer_destroy ((*perspective)->priv->map_timer);
  krad_timer_destroy ((*perspective)->priv->run_timer);

  free ((*perspective)->priv->map);
  free ((*perspective)->priv);
  free (*perspective);
  return 0;
}

int32_t kr_perspective_argb (kr_perspective_t *perspective,
                             uint8_t *out,
                             uint8_t *in) {

  uint32_t w;
  uint32_t h;
  uint32_t x;
  uint32_t y;
  uint32_t *outpx;
  uint32_t *inpx;
  int32_t *map;

  if ((perspective == NULL) || (out == NULL) || (in == NULL)) {
    return -1;
  }

  //krad_timer_start (perspective->priv->run_timer);

  w = perspective->priv->width;
  h = perspective->priv->height; 
  inpx = (uint32_t *)in;
  outpx = (uint32_t *)out;  
  map = perspective->priv->map;

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      outpx[x + w * y] = inpx[map[x + w * y]];
    }
  }

  //krad_timer_status (perspective->priv->run_timer);

  return 0;
}

int32_t kr_perspective (kr_perspective_t *perspective,
                        kr_image_t *out,
                        kr_image_t *in) {

  if ((perspective == NULL) || (out == NULL) || (in == NULL)) {
    return -1;
  }

  if ((out->px == NULL) || (in->px == NULL)) {
    return -2;
  }

  if ((in->w != perspective->width) || (in->h != perspective->height)) {
    return -3;
  }

  if ((out->w != perspective->width) || (out->h != perspective->height)) {
    return -4;
  }

  return kr_perspective_argb (perspective, out->px, in->px);
}
