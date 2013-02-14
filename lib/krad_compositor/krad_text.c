#include "krad_text.h"

void krad_text_destroy_arr (krad_text_t *krad_text, int count) {
  
  int s;
  
  s = 0;
  
  for (s = 0; s < count; s++) {
    krad_text_reset (&krad_text[s]);
  }

  free (krad_text);
}

krad_text_t *krad_text_create_arr (int count) {

  int s;
  krad_text_t *krad_text;

  s = 0;

  if ((krad_text = calloc (count, sizeof (krad_text_t))) == NULL) {
    failfast ("Krad Text mem alloc fail");
  }
  
  for (s = 0; s < count; s++) {
    krad_text[s].subunit.address.path.unit = KR_COMPOSITOR;
    krad_text[s].subunit.address.path.subunit.compositor_subunit = KR_TEXT;
    krad_text[s].subunit.address.id.number = s;
    krad_text_reset (&krad_text[s]);
  }
  
  return krad_text;
}

void krad_text_reset (krad_text_t *krad_text) {
  strcpy (krad_text->font, KRAD_TEXT_DEFAULT_FONT);
  krad_compositor_subunit_reset (&krad_text->subunit);
}

void krad_text_set_text (krad_text_t *krad_text, char *text) {
  strcpy (krad_text->text_actual, text);
  krad_text->subunit.xscale = 32.0f;
}

void krad_text_set_font (krad_text_t *krad_text, char *font) {
  strcpy (krad_text->font, font);
}

/*
void krad_text_expand (krad_text_t *krad_text, cairo_t *cr, int width) {

  float scale;
  cairo_text_extents_t extents;

  cairo_select_font_face (cr, krad_text->font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (cr, krad_text->subunit.xscale);
  cairo_set_source_rgba (cr,
               krad_text->red / 0.255 * 1.0,
               krad_text->green / 0.255 * 1.0,
               krad_text->blue / 0.255 * 1.0,
               krad_text->subunit.opacity);  

  cairo_text_extents (cr, krad_text->text_actual, &extents);

  scale = krad_text->subunit.xscale;
  
  while (extents.width < width) {
    scale += 1.0;
    cairo_set_font_size (cr, scale);
    cairo_text_extents (cr, krad_text->text_actual, &extents);
  }

  subunit_set_xscale(&krad_text->subunit, scale);
  subunit_set_yscale(&krad_text->subunit, scale);
}
*/

void krad_text_prepare (krad_text_t *krad_text, cairo_t *cr) {

  if (krad_text->subunit.rotation != 0.0f) {
    cairo_translate (cr, krad_text->subunit.x, krad_text->subunit.y);  
    cairo_translate (cr, krad_text->subunit.width / 2, krad_text->subunit.height / 2);
    cairo_rotate (cr, krad_text->subunit.rotation * (M_PI/180.0));
    cairo_translate (cr, krad_text->subunit.width / -2, krad_text->subunit.height / -2);    
    cairo_translate (cr, krad_text->subunit.x * -1, krad_text->subunit.y * -1);
  }  

  cairo_select_font_face (cr, krad_text->font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, krad_text->subunit.xscale);
  cairo_set_source_rgba (cr,
                         krad_text->subunit.red,
                         krad_text->subunit.green,
                         krad_text->subunit.blue,
                         krad_text->subunit.opacity);
  
  cairo_move_to (cr, krad_text->subunit.x, krad_text->subunit.y);
  cairo_show_text (cr, krad_text->text_actual);
}

void krad_text_render (krad_text_t *krad_text, cairo_t *cr) {
  cairo_save (cr);
  krad_text_prepare (krad_text, cr);
  cairo_stroke (cr);
  cairo_restore (cr);
}

int krad_text_to_rep (krad_text_t *text, krad_text_rep_t *text_rep) {
  
  if ((text == NULL) || (text_rep == NULL)) {
    return 0;
  }
  
  strncpy (text_rep->text, text->text_actual, sizeof(text_rep->text));
  strncpy (text_rep->font, text->font, sizeof(text_rep->font));
  
  text_rep->red = text->subunit.red;
  text_rep->green = text->subunit.green;
  text_rep->blue = text->subunit.blue;
  
  text_rep->controls.x = text->subunit.x;
  text_rep->controls.y = text->subunit.y;
  text_rep->controls.z = text->subunit.z;
  
  text_rep->controls.width = text->subunit.width;
  text_rep->controls.height = text->subunit.height;
    
  text_rep->controls.xscale = text->subunit.xscale;
    
  text_rep->controls.rotation = text->subunit.rotation;
  text_rep->controls.opacity = text->subunit.opacity;
   
  return 1;
}
