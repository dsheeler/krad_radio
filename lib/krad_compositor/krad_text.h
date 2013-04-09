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

#define KRAD_TEXT_DEFAULT_FONT "helvetica sans"

typedef struct krad_text_St krad_text_t;

struct krad_text_St {

  char font[128];
  char text_actual[256];

  cairo_font_face_t *cairo_ft_face;
  FT_Face ft_face;

  //unsigned int glyph_count;
  //cairo_glyph_t *cairo_glyphs

  FT_Library *ft_library;

  krad_compositor_subunit_t subunit;
};

void krad_text_destroy_arr (krad_text_t *krad_text, int count);
krad_text_t *krad_text_create_arr (FT_Library *ft_library, int count);

void krad_text_reset (krad_text_t *krad_text);
void krad_text_set_text (krad_text_t *krad_text, char *text, char *font);
//void krad_text_expand (krad_text_t *krad_text, cairo_t *cr, int width);

void krad_text_render (krad_text_t *krad_text, cairo_t *cr);

int krad_text_to_rep (krad_text_t *text, krad_text_rep_t *text_rep);

#endif
