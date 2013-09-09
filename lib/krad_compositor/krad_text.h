#include "krad_compositor_subunit.h"
#include "krad_radio.h"

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

struct kr_text {
  char font[256];
  char text_actual[256];
  int prerendered;
  cairo_font_face_t *cr_face;
  FT_Face ft_face;
  FT_Library *ft_library;
  krad_compositor_subunit_t subunit;
};

void krad_text_destroy_arr (kr_text *krad_text, int count);
kr_text *krad_text_create_arr (FT_Library *ft_library, int count);
void krad_text_reset (kr_text *krad_text);
void krad_text_set_text (kr_text *krad_text, char *text, char *font);
void krad_text_prerender_cancel(kr_text *krad_text, cairo_t *cr);
void krad_text_prerender(kr_text *krad_text, cairo_t *cr);
void kr_text_render(kr_text *text, cairo_t *cr);
int kr_text_to_rep(kr_text *text, krad_text_rep_t *text_rep);

#endif
