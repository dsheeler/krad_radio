#ifndef KRAD_TEXT_H
#define KRAD_TEXT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftadvanc.h>
#include <freetype/ftsnames.h>
#include <freetype/tttables.h>
#include <cairo-ft.h>

#define KRAD_TEXT_DEFAULT_FONT "sans"

typedef struct kr_text kr_text;

#include "krad_compositor.h"

struct kr_text {
  kr_text_info info;
  char font[256];
  char text_actual[256];
  int prerendered;
  cairo_font_face_t *cr_face;
  FT_Face ft_face;
  FT_Library *ft_library;
  kr_compositor_control_easers easers;
};

void kr_text_destroy_arr(kr_text *text, int count);
kr_text *kr_text_create_arr(FT_Library *ft_library, int count);
void kr_text_reset(kr_text *text);
void kr_text_set_text(kr_text *text, char *str, char *font);
void kr_text_prerender_cancel(kr_text *text, cairo_t *cr);
void kr_text_prerender(kr_text *text, cairo_t *cr);
void kr_text_render(kr_text *text, cairo_t *cr);
int kr_text_to_info(kr_text *text, kr_text_info *text_rep);

#endif
