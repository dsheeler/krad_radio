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

size_t kr_sprite_size();
void kr_sprite_clear(kr_sprite *sprite);
int kr_sprite_open(kr_sprite *sprite, char *filename);
int kr_sprite_rate_set(kr_sprite *sprite, int rate);
int kr_sprite_info_get(kr_sprite *sprite, kr_sprite_info *info);
void kr_sprite_render(kr_sprite *sprite, cairo_t *cr);

#endif
