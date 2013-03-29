#include "krad_text.h"

static void krad_text_set_font (krad_text_t *krad_text, char *font);

void krad_text_destroy_arr (krad_text_t *krad_text, int count) {
  
  int s;
  
  s = 0;
  
  for (s = 0; s < count; s++) {
    krad_text_reset (&krad_text[s]);
  }

  free (krad_text);
}

krad_text_t *krad_text_create_arr (FT_Library *ft_library, int count) {

  int s;
  krad_text_t *krad_text;

  s = 0;

  if ((krad_text = calloc (count, sizeof (krad_text_t))) == NULL) {
    failfast ("Krad Text mem alloc fail");
  }
  
  for (s = 0; s < count; s++) {
    krad_text[s].ft_library = ft_library;
    krad_text[s].subunit.address.path.unit = KR_COMPOSITOR;
    krad_text[s].subunit.address.path.subunit.compositor_subunit = KR_TEXT;
    krad_text[s].subunit.address.id.number = s;
    krad_text_reset (&krad_text[s]);
  }
  
  return krad_text;
}

void krad_text_reset (krad_text_t *krad_text) {

  if (krad_text->cairo_ft_face != NULL) {
    cairo_font_face_destroy (krad_text->cairo_ft_face);
    krad_text->cairo_ft_face = NULL;
    if (FT_Done_Face (krad_text->ft_face) != 0) {
      printke ("Hrm I guess we didn't need to free that font face.");
    }
  }

  strcpy (krad_text->font, KRAD_TEXT_DEFAULT_FONT);
  krad_compositor_subunit_reset (&krad_text->subunit);
}

void krad_text_set_text (krad_text_t *krad_text, char *text, char *font) {
  strcpy (krad_text->text_actual, text);
  krad_text->subunit.xscale = 42.0f;
  krad_text_set_font (krad_text, font);
}

static void krad_text_set_font (krad_text_t *krad_text, char *font) {

  static const cairo_user_data_key_t key;
  int len;
  int status;
  len = strlen (KRAD_TEXT_DEFAULT_FONT);

  if ((strlen(krad_text->font) == len) &&
      (strncmp(krad_text->font, KRAD_TEXT_DEFAULT_FONT, len) == 0)) {
    strcpy (krad_text->font, font);
    if (FT_New_Face (*krad_text->ft_library, krad_text->font, 0, &krad_text->ft_face) == 0) {
      FT_Set_Char_Size (krad_text->ft_face, 0, 50.0, 100, 100 );
      krad_text->cairo_ft_face = cairo_ft_font_face_create_for_ft_face (krad_text->ft_face, 0);
      // something is wacky about this destroy func and calling FT_DONE_FACE above
      // but it doesn't crash :/
      status = cairo_font_face_set_user_data (krad_text->cairo_ft_face, &key,
                                              krad_text->ft_face, (cairo_destroy_func_t) FT_Done_Face);
      if (status) {
         cairo_font_face_destroy (krad_text->cairo_ft_face);
         krad_text->cairo_ft_face = NULL;
         FT_Done_Face (krad_text->ft_face);
      }
    }
  }
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

  cairo_text_extents_t extents;

  if (krad_text->cairo_ft_face != NULL) {
    cairo_set_font_face (cr, krad_text->cairo_ft_face);
  } else {
    cairo_select_font_face (cr, krad_text->font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);  
  }
  cairo_set_font_size (cr, krad_text->subunit.xscale);
  cairo_set_source_rgba (cr,
                         krad_text->subunit.red,
                         krad_text->subunit.green,
                         krad_text->subunit.blue,
                         krad_text->subunit.opacity);
  cairo_text_extents (cr, krad_text->text_actual, &extents);
  krad_text->subunit.width = extents.width;
  krad_text->subunit.height = extents.height;  
  cairo_translate (cr, krad_text->subunit.x, krad_text->subunit.y);
  if (krad_text->subunit.rotation != 0.0f) {
    cairo_translate (cr, krad_text->subunit.width / 2, krad_text->subunit.xscale / 2);
    cairo_rotate (cr, krad_text->subunit.rotation * (M_PI/180.0));
    cairo_translate (cr, krad_text->subunit.width / -2, krad_text->subunit.xscale / 4);    
  }
  cairo_show_text (cr, krad_text->text_actual);
}

void krad_text_render (krad_text_t *krad_text, cairo_t *cr) {
  cairo_save (cr);
  krad_text_prepare (krad_text, cr);
  cairo_stroke (cr);
  cairo_restore (cr);
  krad_compositor_subunit_tick (&krad_text->subunit);
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
