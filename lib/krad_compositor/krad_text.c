#include "krad_text.h"

static void krad_text_set_font (kr_text *text, char *font);

void krad_text_destroy_arr (kr_text *krad_text, int count) {

  int s;

  s = 0;

  for (s = 0; s < count; s++) {
    krad_text_reset (&krad_text[s]);
  }

  free (krad_text);
}

kr_text *krad_text_create_arr (FT_Library *ft_library, int count) {

  int s;
  kr_text *krad_text;

  s = 0;

  if ((krad_text = calloc (count, sizeof (kr_text))) == NULL) {
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

void krad_text_reset (kr_text *krad_text) {

  if (krad_text->cr_face != NULL) {
    cairo_font_face_destroy (krad_text->cr_face);
    krad_text->cr_face = NULL;
  }

  strcpy (krad_text->font, KRAD_TEXT_DEFAULT_FONT);
  krad_compositor_subunit_reset (&krad_text->subunit);
}

void krad_text_set_text (kr_text *krad_text, char *text, char *font) {
  strcpy (krad_text->text_actual, text);
  krad_text_set_font (krad_text, font);
}

static void krad_text_set_font (kr_text *text, char *font) {

  static const cairo_user_data_key_t key;
  int status;

  if ((font == NULL) || (strlen(font) <= 0)) {
    return;
  }

  if (FT_New_Face (*text->ft_library, font, 0, &text->ft_face) == 0) {
    FT_Set_Char_Size (text->ft_face, 0, 50.0, 100, 100 );
    text->cr_face = cairo_ft_font_face_create_for_ft_face (text->ft_face, 0);
    status = cairo_font_face_set_user_data (text->cr_face, &key,
                                            text->ft_face,
                                            (cairo_destroy_func_t)FT_Done_Face);
    if (status) {
       cairo_font_face_destroy (text->cr_face);
       text->cr_face = NULL;
       FT_Done_Face (text->ft_face);
    } else {
      strcpy (text->font, font);
    }
  }
}

void krad_text_prerender_cancel(kr_text *krad_text, cairo_t *cr) {
  if (krad_text->prerendered == 1) {
    cairo_restore(cr);
    krad_text->prerendered = 0;
  }
}

void krad_text_prerender(kr_text *krad_text, cairo_t *cr) {

  cairo_text_extents_t extents;

  if (krad_text->prerendered == 1) {
    krad_text_prerender_cancel(krad_text, cr);
  }

  cairo_save(cr);

  if (krad_text->cr_face != NULL) {
    cairo_set_font_face (cr, krad_text->cr_face);
  } else {
    cairo_select_font_face (cr, krad_text->font,
                            CAIRO_FONT_SLANT_NORMAL,
                            CAIRO_FONT_WEIGHT_NORMAL);
  }
  cairo_set_font_size(cr, krad_text->subunit.height);
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
    cairo_translate (cr, krad_text->subunit.width / 2,
                     krad_text->subunit.height / -2);
    cairo_rotate (cr, krad_text->subunit.rotation * (M_PI/180.0));
    cairo_translate (cr, krad_text->subunit.width / -2,
                     krad_text->subunit.height / 2);
  }
  krad_text->prerendered = 1;
}

void kr_text_render(kr_text *krad_text, cairo_t *cr) {
  krad_text_prerender(krad_text, cr);
  cairo_show_text(cr, krad_text->text_actual);
  cairo_stroke (cr);
  cairo_restore (cr);
  krad_text->prerendered = 0;
  krad_compositor_subunit_tick (&krad_text->subunit);
}

int kr_text_to_info(kr_text *text, kr_text_info *text_rep) {

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
  text_rep->controls.rotation = text->subunit.rotation;
  text_rep->controls.opacity = text->subunit.opacity;
  return 1;
}
