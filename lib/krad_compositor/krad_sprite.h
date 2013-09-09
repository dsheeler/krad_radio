#ifndef KRAD_SPRITE_H
#define KRAD_SPRITE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cairo.h>
#include <turbojpeg.h>
#ifdef KRAD_GIF
#include <gif_lib.h>
#endif

typedef struct kr_sprite kr_sprite;

#include "krad_compositor_subunit.h"
#include "krad_file.h"

#define KRAD_SPRITE_DEFAULT_TICKRATE 4

struct kr_sprite {
  char filename[256];
  int frames;
  int tick;
  int tickrate;
  int frame;
  cairo_surface_t *sprite;
  cairo_surface_t **sprite_frames;
  int multisurface;
  cairo_pattern_t *sprite_pattern;
  int sheet_width;
  int sheet_height;
  krad_compositor_subunit_t subunit;
};

kr_sprite *krad_sprite_create();
kr_sprite *krad_sprite_create_arr(int count);
void krad_sprite_destroy(kr_sprite *krad_sprite);
void krad_sprite_destroy_arr(kr_sprite *krad_sprite, int count);
kr_sprite *krad_sprite_create_from_file(char *filename);
void krad_sprite_reset(kr_sprite *krad_sprite);
int krad_sprite_open_file(kr_sprite *krad_sprite, char *filename);

void krad_sprite_set_tickrate(kr_sprite *krad_sprite, int tickrate);
void kr_sprite_render(kr_sprite *krad_sprite, cairo_t *cr);
int kr_sprite_to_rep(kr_sprite *sprite, krad_sprite_rep_t *sprite_rep);

#endif
