#include "krad_text.h"

static void kr_text_set_font(kr_text *text, char *font);

void kr_text_destroy_arr(kr_text *text, int count) {
  int s;
  for (s = 0; s < count; s++) {
    kr_text_reset(&text[s]);
  }
  free(text);
}

kr_text *kr_text_create_arr(FT_Library *ft_library, int count) {
  int s;
  kr_text *text;
  if ((text = calloc(count, sizeof (kr_text))) == NULL) {
    failfast("Krad Text mem alloc fail");
  }
  for (s = 0; s < count; s++) {
    text[s].ft_library = ft_library;
    text[s].subunit.address.path.unit = KR_COMPOSITOR;
    text[s].subunit.address.path.subunit.compositor_subunit = KR_TEXT;
    text[s].subunit.address.id.number = s;
    kr_text_reset(&text[s]);
  }
  return text;
}

void kr_text_reset(kr_text *text) {
  if (text->cr_face != NULL) {
    cairo_font_face_destroy(text->cr_face);
    text->cr_face = NULL;
  }
  strcpy(text->font, KRAD_TEXT_DEFAULT_FONT);
  krad_compositor_subunit_reset(&text->subunit);
}

void kr_text_set_text(kr_text *text, char *str, char *font) {
  strcpy(text->text_actual, str);
  kr_text_set_font(text, font);
}

static void kr_text_set_font(kr_text *text, char *font) {

  static const cairo_user_data_key_t key;
  int status;

  if ((font == NULL) || (strlen(font) <= 0)) {
    return;
  }

  if (FT_New_Face(*text->ft_library, font, 0, &text->ft_face) == 0) {
    FT_Set_Char_Size(text->ft_face, 0, 50.0, 100, 100 );
    text->cr_face = cairo_ft_font_face_create_for_ft_face(text->ft_face, 0);
    status = cairo_font_face_set_user_data(text->cr_face, &key,
                                            text->ft_face,
                                            (cairo_destroy_func_t)FT_Done_Face);
    if (status) {
       cairo_font_face_destroy(text->cr_face);
       text->cr_face = NULL;
       FT_Done_Face(text->ft_face);
    } else {
      strcpy(text->font, font);
    }
  }
}

void kr_text_prerender_cancel(kr_text *text, cairo_t *cr) {
  if (text->prerendered == 1) {
    cairo_restore(cr);
    text->prerendered = 0;
  }
}

void kr_text_prerender(kr_text *krad_text, cairo_t *cr) {

  cairo_text_extents_t extents;

  if (krad_text->prerendered == 1) {
    kr_text_prerender_cancel(krad_text, cr);
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

void kr_text_render(kr_text *text, cairo_t *cr) {
  kr_text_prerender(text, cr);
  cairo_show_text(cr, text->text_actual);
  cairo_stroke(cr);
  cairo_restore(cr);
  text->prerendered = 0;
  krad_compositor_subunit_tick(&text->subunit);
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
  text_rep->controls.w = text->subunit.width;
  text_rep->controls.h = text->subunit.height;
  text_rep->controls.rotation = text->subunit.rotation;
  text_rep->controls.opacity = text->subunit.opacity;
  return 1;
}
