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

typedef struct krad_text_St krad_text_t;

struct krad_text_St {

  char font[256];
  char text_actual[256];

  cairo_font_face_t *cr_face;
  FT_Face ft_face;

  FT_Library *ft_library;

  krad_compositor_subunit_t subunit;
};

void krad_text_destroy_arr (krad_text_t *krad_text, int count);
krad_text_t *krad_text_create_arr (FT_Library *ft_library, int count);
void krad_text_reset (krad_text_t *krad_text);
void krad_text_set_text (krad_text_t *krad_text, char *text, char *font);
void krad_text_render (krad_text_t *krad_text, cairo_t *cr);
int krad_text_to_rep (krad_text_t *text, krad_text_rep_t *text_rep);

#endif
