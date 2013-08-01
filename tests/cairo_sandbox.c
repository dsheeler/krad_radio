#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo/cairo.h>

#define M_PI 3.14159265358979323846

/*
*  gcc cairo_sandbox.c -o cairo_sandbox `pkg-config --libs --cflags cairo`
*/

static void krad_vector_render_rrect(cairo_t *cr, int x, int y, int w,
 int h, float r, float g, float b, float op) {

  double aspect;
  double corner_radius;
  double radius;
  double degrees;

  if (w > h) {
    aspect = w/h;
  } else {
    aspect = h/w;
  }
  corner_radius = h / 15.0;
  radius = corner_radius / aspect;
  degrees = M_PI / 180.0;

  cairo_new_sub_path(cr);
  cairo_arc(cr, x + w - radius, y + radius, radius, -90 * degrees, 0);
  cairo_arc(cr, x + w - radius, y + h - radius, radius, 0, 90 * degrees);
  cairo_arc(cr, x + radius, y + h - radius, radius, 90 * degrees,
   180 * degrees);
  cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
  cairo_close_path(cr);
  cairo_set_source_rgba(cr, r, g, b, op);
  cairo_fill_preserve(cr);
  cairo_set_source_rgba(cr, r, g, b, op);
  cairo_set_line_width(cr, 5.0);
  cairo_stroke(cr);

  return;
}

void render_rectangle(char *filename) {

  cairo_surface_t *surface;
  cairo_t *cr;
  int width;
  int height;
  int rec_x;
  int rec_y;
  int rec_width;
  int rec_height;

  width = 1280;
  height = 720;
  rec_x = 320;
  rec_y = 240;
  rec_width = 30;
  rec_height = 300;

  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  cr = cairo_create(surface);
  // Fill with white (default is transparent)
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);
  krad_vector_render_rrect(cr, rec_x, rec_y, rec_width, rec_height,
   0.8, 0.2, 0.2, 0.5);
  cairo_surface_write_to_png(surface, filename);
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

int main(int argc, char *argv[]) {

  char filename[128];

  snprintf(filename, sizeof(filename),
           "%s/images/rec_test.png",
           getenv("HOME"));
  render_rectangle(filename);
  printf("It worked!\n");

  return 0;
}
