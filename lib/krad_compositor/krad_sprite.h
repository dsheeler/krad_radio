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
#include "krad_compositor.h"

#include "krad_file.h"

#define KRAD_SPRITE_DEFAULT_TICKRATE 4

struct kr_sprite {
  kr_sprite_info info;
  kr_compositor_control_easers easers;
  int frames;
  int tick;
  int frame;
  cairo_surface_t *sprite;
  cairo_surface_t **sprite_frames;
  int multisurface;
  cairo_pattern_t *sprite_pattern;
  int sheet_width;
  int sheet_height;
};

kr_sprite *kr_sprite_create();
kr_sprite *kr_sprite_create_arr(int count);
void kr_sprite_destroy(kr_sprite *sprite);
void kr_sprite_destroy_arr(kr_sprite *sprite, int count);
kr_sprite *kr_sprite_create_from_file(char *filename);
void kr_sprite_reset(kr_sprite *sprite);
int kr_sprite_open_file(kr_sprite *sprite, char *filename);
void kr_sprite_set_tickrate(kr_sprite *sprite, int tickrate);
void kr_sprite_render(kr_sprite *sprite, cairo_t *cr);
int kr_sprite_to_info(kr_sprite *sprite, kr_sprite_info *info);

#endif
