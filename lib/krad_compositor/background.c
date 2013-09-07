void krad_compositor_unset_background(krad_compositor_t *compositor) {
  if (compositor->background->subunit.active != 1) {
    return;
  }
  compositor->background->subunit.active = 2;
  usleep(100000);
  krad_sprite_reset(compositor->background);
  compositor->background->subunit.active = 0;
}

void krad_compositor_set_background(krad_compositor_t *compositor,
 char *filename) {
  krad_compositor_unset_background(compositor);
  if ((filename == NULL) || (strlen(filename) == 0)) {
    return;
  }
  if (krad_sprite_open_file(compositor->background, filename)) {
    compositor->background->subunit.active = 1;
  } else {
    compositor->background->subunit.active = 0;
  }
}

int krad_compositor_get_background_name(krad_compositor_t *compositor,
 char **filename) {

  if ((filename == NULL) || (compositor->background->subunit.active != 1)) {
    return 0;
  }
  *filename = compositor->background->filename;
  return 1;
}

static void krad_compositor_render_background(krad_compositor_t *compositor) {
  if (compositor->background->subunit.active != 1) {
    return;
  }
  cairo_save(compositor->cr);
  if ((compositor->background->subunit.width != compositor->width)
      || (compositor->background->subunit.height != compositor->height)) {
    cairo_set_source(compositor->cr, compositor->background->sprite_pattern);
  } else {
    cairo_set_source_surface(compositor->cr, compositor->background->sprite, 0, 0 );
  }
  cairo_paint(compositor->cr);
  cairo_restore(compositor->cr);
}

int krad_compositor_has_background(krad_compositor_t *compositor) {
  if (compositor->background->subunit.active == 1) {
    return 1;
  }
  return 0;
}

void krad_compositor_render_no_input(krad_compositor_t *compositor) {
  cairo_save(compositor->cr);
  if ((compositor->frames % 24) < 12) {
    cairo_set_source_rgba(compositor->cr, RED, 0.0f + ((compositor->frames % 12) * 0.09f));
  } else {
    cairo_set_source_rgba(compositor->cr, RED, 1.0f + ((compositor->frames % 12) * -0.09f));
  }
  cairo_select_font_face(compositor->cr, "", CAIRO_FONT_SLANT_NORMAL,
   CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(compositor->cr, 42.0);
  cairo_move_to(compositor->cr, 64, 64 + 42);
  cairo_show_text(compositor->cr, "KR: No Input!");
  cairo_stroke(compositor->cr);
  cairo_restore(compositor->cr);
}

void krad_compositor_clear_frame(krad_compositor_t *compositor) {
  cairo_save(compositor->cr);
  cairo_set_source_rgba(compositor->cr, BGCOLOR_CLR);
  cairo_set_operator(compositor->cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(compositor->cr);
  cairo_restore(compositor->cr);
}
