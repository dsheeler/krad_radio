#include "hex.h"

void render_meter (cairo_t *cr, int x, int y, int size, float pos, float opacity) {

  pos = pos * 1.8f - 90.0f;

  cairo_new_path ( cr );

  cairo_translate (cr, x, y);
  cairo_set_line_width(cr, 0.05 * size);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);
  cairo_set_source_rgba(cr, GREY3, opacity);
  cairo_arc (cr, 0, 0, 0.8 * size, M_PI, 0);
  cairo_stroke (cr);

  cairo_set_source_rgba(cr, ORANGE, opacity);
  cairo_arc (cr, 0, 0, 0.65 * size, 1.8 * M_PI, 0);
  cairo_stroke (cr);

  cairo_arc (cr, size - 0.56 * size, -0.15 * size, 0.07 * size, 0, 2 * M_PI);
  cairo_fill(cr);

  cairo_save(cr);
  cairo_translate (cr, 0.05 * size, 0);
  cairo_rotate (cr, pos * (M_PI/180.0));

  cairo_pattern_t *pat;
  pat = cairo_pattern_create_linear (0, 0, 0.11 * size, 0);
  cairo_pattern_add_color_stop_rgba (pat, 0, 0, 0, 0, 0);
  cairo_pattern_add_color_stop_rgba (pat, 0.3, 0, 0, 0, 0);
  cairo_pattern_add_color_stop_rgba (pat, 0.4, 0, 0, 0, 1);
  cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 0);
  cairo_set_source (cr, pat);
  cairo_rectangle (cr, 0, 0, 0.11 * size, -size);
  cairo_fill (cr);

  cairo_set_source_rgba(cr, WHITE, opacity);
  cairo_move_to (cr, 0, 0);
  cairo_line_to (cr, 0, -size * 0.93);
  cairo_stroke (cr);

  cairo_restore(cr);
  cairo_set_source_rgba(cr, WHITE, opacity);
  cairo_arc (cr, 0.05 * size, 0, 0.1 * size, 0, 2 * M_PI);
  cairo_fill(cr);

}

static void render_hex (cairo_t *cr, int x, int y, int w) {

  static float hexrot = 0;
  int r1;

  r1 = ((w)/2 * sqrt(3));

  cairo_translate (cr, x, y);
  cairo_rotate (cr, hexrot * (M_PI/180.0));
  cairo_translate (cr, -(w/2), -r1);

  cairo_move_to (cr, 0, 0);
  cairo_rel_line_to (cr, w, 0);
  cairo_rotate (cr, 60 * (M_PI/180.0));
  cairo_rel_line_to (cr, w, 0);
  cairo_rotate (cr, 60 * (M_PI/180.0));
  cairo_rel_line_to (cr, w, 0);
  cairo_rotate (cr, 60 * (M_PI/180.0));
  cairo_rel_line_to (cr, w, 0);
  cairo_rotate (cr, 60 * (M_PI/180.0));
  cairo_rel_line_to (cr, w, 0);
  cairo_rotate (cr, 60 * (M_PI/180.0));
  cairo_close_path (cr);

  cairo_set_source_rgba (cr, BLUE, 0.89);
  cairo_fill_preserve (cr);

  cairo_set_line_width (cr, 44);
  cairo_set_source_rgb (cr, ORANGE);
  cairo_stroke (cr);

  hexrot += ((x - y) * 0.01);

}

void hexagon (int width, int height, int x, int y, uint32_t time, void *buffer) {

  cairo_surface_t *cst;
  cairo_t *cr;

  cst = cairo_image_surface_create_for_data ((unsigned char *)buffer,
                         CAIRO_FORMAT_ARGB32,
                         width,
                         height,
                         width * 4);

  cr = cairo_create (cst);
  cairo_save (cr);
  //cairo_set_source_rgba (cr, BGCOLOR_CLR);
  //cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  //cairo_paint (cr);
  //cairo_restore (cr);


  uint32_t *p;
  int i, end, offset;

  p = buffer;
  end = width * height;
  offset = time >> 4;
  offset = offset + x;
  for (i = 0; i < end; i++) {
    p[i] = (i + offset) * 0x0080401;
  }

  if ((x > 0) && (y > 0)) {
    render_hex (cr, x, y, 66);
  }
  cairo_restore (cr);
  cairo_save (cr);
  render_meter (cr, 168, height - 40, 232, x / (float)width * 100.0f, 1);
  cairo_restore (cr);
  cairo_save (cr);
  render_meter (cr, width - 268, height - 40, 232, 100.0f - (x / (float)width * 100.0f), 1);
  cairo_surface_flush (cst);
  cairo_destroy (cr);
  cairo_surface_destroy (cst);
}
