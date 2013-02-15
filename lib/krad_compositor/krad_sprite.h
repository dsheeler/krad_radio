#ifndef KRAD_SPRITE_H
#define KRAD_SPRITE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <turbojpeg.h>

#ifdef KRAD_GIF
#include <gif_lib.h>
#endif

typedef struct krad_sprite_St krad_sprite_t;

#include "krad_compositor_subunit.h"
#include "krad_radio.h"

#define KRAD_SPRITE_DEFAULT_TICKRATE 4

struct krad_sprite_St {

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

krad_sprite_t *krad_sprite_create ();
krad_sprite_t *krad_sprite_create_arr (int count);
void krad_sprite_destroy (krad_sprite_t *krad_sprite);
void krad_sprite_destroy_arr (krad_sprite_t *krad_sprite, int count);
krad_sprite_t *krad_sprite_create_from_file (char *filename);
void krad_sprite_reset (krad_sprite_t *krad_sprite);
int krad_sprite_open_file (krad_sprite_t *krad_sprite, char *filename);

void krad_sprite_set_tickrate (krad_sprite_t *krad_sprite, int tickrate);
void krad_sprite_render (krad_sprite_t *krad_sprite, cairo_t *cr);
int krad_sprite_to_rep (krad_sprite_t *sprite, krad_sprite_rep_t *sprite_rep);
#endif
