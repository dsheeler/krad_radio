#include "krad_compositor_subunit.h"
#include "krad_radio.h"

#ifndef KRAD_TEXT_H
#define KRAD_TEXT_H

#define KRAD_TEXT_DEFAULT_FONT "sans"

typedef struct krad_text_St krad_text_t;

struct krad_text_St {

  char font[128];
  char text_actual[256];

  float red;
  float blue;
  float green;

  krad_compositor_subunit_t subunit;
};

void krad_text_destroy_arr (krad_text_t *krad_text, int count);
krad_text_t *krad_text_create_arr (int count);

void krad_text_reset (krad_text_t *krad_text);
void krad_text_set_text (krad_text_t *krad_text, char *text);
void krad_text_set_font (krad_text_t *krad_text, char *font);
//void krad_text_expand (krad_text_t *krad_text, cairo_t *cr, int width);

void krad_text_render (krad_text_t *krad_text, cairo_t *cr);

int krad_text_to_rep (krad_text_t *text, krad_text_rep_t *text_rep);

#endif
