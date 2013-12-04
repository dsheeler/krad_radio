#ifndef KRAD_TEXT_H
#define KRAD_TEXT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <cairo-ft.h>

#define KRAD_TEXT_DEFAULT_FONT "sans"

typedef struct kr_text kr_text;

#include "krad_compositor.h"

size_t kr_text_size();
void kr_text_clear(kr_text *text);
int kr_text_init(kr_text *text, char *str, char *font, FT_Library *ftlib);
void kr_text_render(kr_text *text, cairo_t *cr);
int kr_text_info_get(kr_text *text, kr_text_info *text_rep);

#endif
