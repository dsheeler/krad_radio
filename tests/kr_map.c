#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo/cairo.h>

#define M_PI 3.14159265358979323846

/*
*  gcc kr_map.c -o krmap `pkg-config --libs --cflags cairo`
*/

void render_map(cairo_t *cr) {
  /* Line types:
   *
   * Wide Dashed Line - RAW A/V          Wavy Dash / RGB Dash Video
   * Thin Dashed Line - Codec Packets    Short Audio / Long Video
   * Normal Line      - Muxed Stream     Combined Dash colors?
   *
   * Icon types:
   *
   * Hexagon          - KR Station
   * Triangle         - Muxer
   * Hexagle          - Coder
   *
   *
   */
}

void draw_map(char *filename) {

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
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_paint(cr);
  render_map(cr);
  cairo_surface_write_to_png(surface, filename);
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

int main(int argc, char *argv[]) {

  char filename[128];

  snprintf(filename, sizeof(filename), "%s/images/krmap.png", getenv("HOME"));
  draw_map(filename);
  printf("It worked!\n");

  return 0;
}
