#include "krad_vector.h"

#define PRO_REEL_SIZE 26.67
#define PRO_REEL_SPEED 38.1
#define NORMAL_REEL_SIZE 17.78
#define NORMAL_REEL_SPEED 19.05
#define SLOW_NORMAL_REEL_SPEED 9.53

#define BLUE 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0
#define BLUE_TRANS 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0, 0.255
#define BLUE_TRANS2 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0, 0.144 / 0.255 * 1.0
#define BLUE_TRANS3 0.0, 0.122 / 0.255 * 1.0, 0.112 / 0.255 * 1.0, 0.144 / 0.255 * 1.0
#define GREEN  0.001 / 0.255 * 1.0, 0.187 / 0.255 * 1.0, 0.0
#define LGREEN  0.001 / 0.255 * 1.0, 0.187 / 0.255 * 1.0, 0.0, 0.044 / 0.255 * 1.0
#define WHITE 0.222 / 0.255 * 1.0, 0.232 / 0.255 * 1.0, 0.233 / 0.255 * 1.0
#define WHITE_TRANS 0.222 / 0.255 * 1.0, 0.232 / 0.255 * 1.0, 0.233 / 0.255 * 1.0, 0.555
#define ORANGE  0.255 / 0.255 * 1.0, 0.080 / 0.255 * 1.0, 0.0
#define GREY  0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255   * 1.0
#define GREY2  0.037 / 0.255 * 1.0, 0.037 / 0.255 * 1.0, 0.038 / 0.255   * 1.0
#define BGCOLOR  0.033 / 0.255 * 1.0, 0.033 / 0.255 * 1.0, 0.033 / 0.255   * 1.0
#define GREY3  0.103 / 0.255 * 1.0, 0.103 / 0.255 * 1.0, 0.124 / 0.255   * 1.0
#define BGCOLOR_TRANS  0.033 / 0.255 * 1.0, 0.033 / 0.255 * 1.0, 0.033 / 0.255 * 1.0, 0.144 / 0.255 * 1.0
#define BGCOLOR_CLR 0.0, 0.0, 0.0, 1.0

struct kr_vector {
  kr_vector_info info;
  kr_compositor_control_easers easers;
  kr_vector_type type;
};

static void render_meter(cairo_t *cr, int x, int y, int size, float pos, float opacity);
static void render_hex(cairo_t *cr, int x, int y, int w, float r, float g, float b, float opacity);
static void render_grid(cairo_t *cr, int x, int y, int w, int h, int lines, float r, float g, float b, float opacity);
static void render_curve(cairo_t *cr, int w, int h);
static void render_circle(cairo_t *cr, int x, int y, float radius, float r, float g, float b, float opacity);
static void render_rectangle(cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity);
static void render_triangle(cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity);
static void render_arrow(cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity);
static void render_viper(cairo_t * cr, int x, int y, int size, float direction);
static void render_clock(cairo_t *cr, int x, int y, int width, int height, float opacity);
static void render_shadow(cairo_t *cr, int x, int y, int w, int h, float r, float g, float b, float a);

size_t kr_vector_size() {
  return sizeof(kr_vector);
}

void kr_vector_clear(kr_vector *vector) {
  if (vector == NULL) return;
  vector->type = NOTHING;
}

int kr_vector_init(kr_vector *vector, char *type) {
  if ((vector == NULL) || (type == NULL)) return -1;
  memset(vector, 0, sizeof(kr_vector));
  vector->info.controls.opacity = 1.0f;
  vector->type = kr_string_to_vector_type(type);
  vector->info.controls.w = 96;
  vector->info.controls.h = 96;
  switch (vector->type) {
    case METER:
    case HEX:
    case CIRCLE:
    case RECT:
    case TRIANGLE:
    case VIPER:
    case GRID:
    case CURVE:
    case ARROW:
    case CLOCK:
    case SHADOW:
      return 0;
    case NOTHING:
      break;
  }
  return -1;
}

int kr_vector_info_get(kr_vector *vector, kr_vector_info *info) {
  if ((vector == NULL) || (info == NULL)) {
    return 0;
  }
  *info = vector->info;
  return 1;
}

void kr_vector_render(kr_vector *krad_vector, cairo_t *cr) {

  cairo_save(cr);

  if (krad_vector->type != VIPER) {
    if (krad_vector->info.controls.rotation != 0.0f) {
      cairo_translate(cr, krad_vector->info.controls.x, krad_vector->info.controls.y);
      cairo_translate(cr, krad_vector->info.controls.w / 2, krad_vector->info.controls.h / 2);
      cairo_rotate(cr, krad_vector->info.controls.rotation * (M_PI/180.0));
      cairo_translate(cr, krad_vector->info.controls.w / -2, krad_vector->info.controls.h / -2);
      cairo_translate(cr, krad_vector->info.controls.x * -1, krad_vector->info.controls.y * -1);
    }
  }

  switch (krad_vector->type) {

    case METER:
      render_meter(cr, krad_vector->info.controls.x, krad_vector->info.controls.y,
                                krad_vector->info.controls.w,
                                krad_vector->info.controls.rotation, krad_vector->info.controls.opacity);
      break;

    case HEX:

      render_hex(cr, krad_vector->info.controls.x,
                              krad_vector->info.controls.y,
                              krad_vector->info.controls.w,
                              krad_vector->info.red,
                              krad_vector->info.green,
                              krad_vector->info.blue,
                              krad_vector->info.controls.opacity);
      break;
    case CURVE:
      render_curve(cr, krad_vector->info.controls.x,
                                krad_vector->info.controls.y);
      break;
    case CIRCLE:
      render_circle(cr, krad_vector->info.controls.x,
                                 krad_vector->info.controls.y,
                                 krad_vector->info.controls.w,
                                 krad_vector->info.red,
                                 krad_vector->info.green,
                                 krad_vector->info.blue,
                                 krad_vector->info.controls.opacity);
       break;
    case RECT:
      render_rectangle(cr, krad_vector->info.controls.x,
                                    krad_vector->info.controls.y,
                                    krad_vector->info.controls.w,
                                    krad_vector->info.controls.h,
                                    krad_vector->info.red,
                                    krad_vector->info.green,
                                    krad_vector->info.blue,
                                    krad_vector->info.controls.opacity);
      break;
    case TRIANGLE:
      render_triangle(cr, krad_vector->info.controls.x,
                                   krad_vector->info.controls.y,
                                   krad_vector->info.controls.w,
                                   krad_vector->info.controls.h,
                                   krad_vector->info.red,
                                   krad_vector->info.green,
                                   krad_vector->info.blue,
                                   krad_vector->info.controls.opacity);
      break;
    case ARROW:
      render_arrow(cr, krad_vector->info.controls.x,
                                   krad_vector->info.controls.y,
                                   krad_vector->info.controls.w,
                                   krad_vector->info.controls.h,
                                   krad_vector->info.red,
                                   krad_vector->info.green,
                                   krad_vector->info.blue,
                                   krad_vector->info.controls.opacity);
      break;
    case GRID:
      render_grid(cr, krad_vector->info.controls.x,
                               krad_vector->info.controls.y,
                               krad_vector->info.controls.w,
                               krad_vector->info.controls.h,
                               6,
                               krad_vector->info.red,
                               krad_vector->info.green,
                               krad_vector->info.blue,
                               krad_vector->info.controls.opacity);
      break;
    case VIPER:
      render_viper(cr, krad_vector->info.controls.x,
                                krad_vector->info.controls.y,
                                krad_vector->info.controls.w,
                                krad_vector->info.controls.rotation);
      break;
    case CLOCK:
      render_clock(cr, krad_vector->info.controls.x,
                                krad_vector->info.controls.y,
                                (int) krad_vector->info.controls.w,
                                (int) krad_vector->info.controls.h,
                                 krad_vector->info.controls.opacity);
      break;
    case SHADOW:
      render_shadow(cr, krad_vector->info.controls.x,
                                            krad_vector->info.controls.y,
                                            krad_vector->info.controls.w,
                                            krad_vector->info.controls.h,
                                            krad_vector->info.red,
                                            krad_vector->info.green,
                                            krad_vector->info.blue,
                                            krad_vector->info.controls.opacity);
      break;
    default:
      break;
  }

  cairo_restore(cr);
}

static void render_meter(cairo_t *cr, int x, int y, int size, float pos, float opacity) {

  pos = pos * 1.8f - 90.0f;

  cairo_new_path(cr);

  cairo_translate(cr, x, y);
  cairo_set_line_width(cr, 0.05 * size);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
  cairo_set_source_rgba(cr, GREY3, opacity);
  cairo_arc(cr, 0, 0, 0.8 * size, M_PI, 0);
  cairo_stroke(cr);

  cairo_set_source_rgba(cr, ORANGE, opacity);
  cairo_arc(cr, 0, 0, 0.65 * size, 1.8 * M_PI, 0);
  cairo_stroke(cr);

  cairo_arc(cr, size - 0.56 * size, -0.15 * size, 0.07 * size, 0, 2 * M_PI);
  cairo_fill(cr);

  cairo_save(cr);
  cairo_translate(cr, 0.05 * size, 0);
  cairo_rotate(cr, pos * (M_PI/180.0));

  cairo_pattern_t *pat;
  pat = cairo_pattern_create_linear (0, 0, 0.11 * size, 0);
  cairo_pattern_add_color_stop_rgba (pat, 0, 0, 0, 0, 0);
  cairo_pattern_add_color_stop_rgba (pat, 0.3, 0, 0, 0, 0);
  cairo_pattern_add_color_stop_rgba (pat, 0.4, 0, 0, 0, 1);
  cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 0);
  cairo_set_source(cr, pat);
  cairo_rectangle(cr, 0, 0, 0.11 * size, -size);
  cairo_fill(cr);

  cairo_set_source_rgba(cr, WHITE, opacity);
  cairo_move_to(cr, 0, 0);
  cairo_line_to(cr, 0, -size * 0.93);
  cairo_stroke(cr);

  cairo_restore(cr);
  cairo_set_source_rgba(cr, WHITE, opacity);
  cairo_arc(cr, 0.05 * size, 0, 0.1 * size, 0, 2 * M_PI);
  cairo_fill(cr);

}

static void render_hex(cairo_t *cr, int x, int y, int w, float r, float g, float b, float opacity) {

  cairo_pattern_t *pat;

  static float hexrot = 0;

  cairo_set_line_width(cr, 1);
  cairo_set_source_rgba(cr, r, g, b, opacity);

  int r1;
  float scale;

  scale = 2.5;

  r1 = ((w)/2 * sqrt(3));

  cairo_translate(cr, x, y);
  cairo_rotate(cr, hexrot * (M_PI/180.0));
  cairo_translate(cr, -(w/2), -r1);

  cairo_move_to(cr, 0, 0);
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);

  hexrot += 1.5;
  cairo_fill(cr);

  cairo_restore(cr);

  cairo_save(cr);

  cairo_set_line_width(cr, 1.5);
  cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
  cairo_set_source_rgba(cr, GREY, opacity);

  cairo_translate(cr, x, y);
  cairo_rotate(cr, hexrot * (M_PI/180.0));
  cairo_translate(cr, -((w * scale)/2), -r1 * scale);
  cairo_scale(cr, scale, scale);

  cairo_move_to(cr, 0, 0);
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 60 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);

  cairo_rotate(cr, 60 * (M_PI/180.0));

  cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
  pat = cairo_pattern_create_radial (w/2, r1, 3, w/2, r1, r1*scale);
  cairo_pattern_add_color_stop_rgba (pat, 0, r, g, b, opacity);
  cairo_pattern_add_color_stop_rgba (pat, 0.4, 0, 0, 0, 0);
  cairo_set_source(cr, pat);

  cairo_fill(cr);
  cairo_pattern_destroy (pat);

}

static void render_grid(cairo_t *cr, int x, int y, int w, int h, int lines, float r, float g, float b, float opacity) {

  int count;

  cairo_translate(cr, x + w/2, y + h/2);

  cairo_translate(cr, -x + -w/2, -y + -h/2);
  cairo_set_line_width(cr, 10);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
  cairo_set_source_rgba(cr, r, g, b, opacity);

  cairo_save(cr);

  for (count = 1; count != lines; count++) {
    cairo_move_to(cr, x + (w/lines) * count, y);
    cairo_line_to(cr, x + (w/lines) * count, y + h);
    cairo_stroke(cr);
  }

  cairo_restore(cr);

  for (count = 1; count != lines; count++) {
    cairo_move_to(cr, x, y + (h/lines) * count);
    cairo_line_to(cr, x + w, y + (h/lines) * count);
    cairo_stroke(cr);
  }

  cairo_rectangle(cr, x, y, w, h);
  cairo_stroke(cr);
}

static void render_curve(cairo_t *cr, int w, int h) {

  cairo_set_line_width(cr, 3.5);
  cairo_set_source_rgb(cr, BLUE);

  static float pointy_positioner = 0.0f;
  int pointy;
  int pointy_start = 15;
  int pointy_range = 83;
  float pointy_final;
  float pointy_final_adj;
  float speed = 0.021;

  pointy = pointy_start + (pointy_range / 2) + round(pointy_range * sin(pointy_positioner) / 2);

  pointy_positioner += speed;

  if (pointy_positioner >= 2 * M_PI) {
    pointy_positioner -= 2 * M_PI;
  }

  pointy_final = pointy * 0.01f;

  pointy_final_adj = pointy_final / 3.0f;

  int point1_x, point1_y, point2_x, point2_y;

  point1_x =  (0.250 + pointy_final_adj) * w;
  point1_y =  0.25 * h;
  point2_x =  (0.75 - pointy_final_adj) * w;
  point2_y =  0.25 * h;

  point1_y =  pointy_final * h;
  point2_y =  pointy_final * h;

  cairo_move_to(cr, 20, 70);
  cairo_curve_to(cr, point1_x, point1_y, point2_x, point2_y, 0.85 * w, 0.85 * h);
  cairo_stroke(cr);
}

static void render_circle(cairo_t *cr, int x, int y, float radius, float r, float g, float b, float opacity) {
  cairo_set_source_rgba(cr, r, g, b, opacity);
  cairo_arc(cr, x, y, radius, 0, 2*M_PI);
  cairo_fill(cr);
}

static void render_rectangle(cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity) {

  cairo_set_source_rgba(cr, r, g, b, opacity);
  cairo_set_line_width(cr, 0);

  cairo_move_to(cr, x-w/2.0, y-h/2.0);
  cairo_rel_line_to(cr, w, 0);
  cairo_rel_line_to(cr, 0, h);
  cairo_rel_line_to(cr, -w, 0);
  cairo_close_path(cr);

  cairo_stroke_preserve(cr);

  cairo_fill(cr);
}

static void render_triangle(cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity) {

  cairo_set_source_rgba(cr, r, g, b, opacity);
  cairo_set_line_width(cr, 0);

  cairo_move_to(cr, x, y);
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 120 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_rotate(cr, 120 * (M_PI/180.0));
  cairo_rel_line_to(cr, w, 0);
  cairo_close_path(cr);

  cairo_stroke_preserve(cr);
  cairo_fill(cr);
}

static void render_arrow(cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity) {

  cairo_set_source_rgba(cr, r, g, b, opacity);
  cairo_set_line_width(cr, 0);

  cairo_move_to(cr, x, y);
  cairo_rel_line_to(cr, w, 0);
  cairo_rel_line_to(cr, 0, h);
  cairo_rel_line_to(cr, w/4, 0);
  cairo_rel_line_to(cr, -3*w/4, w);
  cairo_rel_line_to(cr, -3*w/4, -w);
  cairo_rel_line_to(cr, w/4, 0);
  cairo_rel_line_to(cr, 0, -h);

  cairo_close_path(cr);
  cairo_stroke_preserve(cr);
  cairo_fill(cr);
}

static void render_viper(cairo_t * cr, int x, int y, int size, float direction) {

  cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);

  cairo_set_line_width(cr, size/7);
  cairo_set_source_rgb(cr, WHITE);
  cairo_translate(cr, x, y);

  cairo_arc(cr, 0.0, 0.0, size/2, 0, 2*M_PI);
  cairo_stroke(cr);

  cairo_set_source_rgb(cr, GREEN);

  cairo_arc(cr, 0.0, -size/5, size/7, 0, 2*M_PI);
  cairo_fill(cr);

  cairo_rotate(cr, 40 * (M_PI/180.0));
  cairo_arc(cr, size/3.8, 0.0, size/7, 0, 2*M_PI);
  cairo_fill(cr);

  cairo_move_to(cr, size/1.8,  0.0);
  cairo_rel_line_to(cr, size/2.3, 0.0);
  cairo_stroke(cr);

  cairo_rotate(cr, 100 * (M_PI/180.0));
  cairo_arc(cr, size/3.8, 0.0, size/7, 0, 2*M_PI);
  cairo_fill(cr);

  cairo_move_to(cr, size/1.8,  0.0);
  cairo_rel_line_to(cr, size/2.3, 0.0);
  cairo_stroke(cr);

  cairo_rotate(cr, 130 * (M_PI/180.0));
  cairo_move_to(cr, size/1.8,  0.0);
  cairo_rel_line_to(cr, size/2.3, 0.0);
  cairo_stroke(cr);

  cairo_set_source_rgb(cr, WHITE);
  cairo_rotate(cr, (180 + direction - 90) * (M_PI/180.0));
  cairo_move_to(cr, size/1.6,  size/6);
  cairo_rel_line_to(cr, size/6, -size/6);
  cairo_rel_line_to(cr, -size/6, -size/6);
  //cairo_rel_line_to(cr, size/1.6,  size/6);
  cairo_fill(cr);

  cairo_set_line_width(cr, size/7);
  cairo_rotate(cr, 180 * (M_PI/180.0));
  cairo_move_to(cr, size/1.6, 0.0);
  cairo_rel_line_to(cr, size/8, 0.0);
  cairo_stroke(cr);
}

static void render_clock(cairo_t *cr, int x, int y, int width, int height, float opacity) {

  double m_line_width = 0.051;
  double m_radius = 0.42;
  int i;

  cairo_translate(cr, x, y);
  cairo_scale(cr, width, height);
  cairo_translate(cr, 0.5, 0.5);
  cairo_set_line_width(cr, m_line_width);
  cairo_set_source_rgba(cr,  0.117, 0.337, 0.612, opacity);
  cairo_arc(cr, 0, 0, m_radius, 0, 2 * M_PI);
  cairo_save(cr) ;
  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8*opacity);
  cairo_fill_preserve(cr) ;
  cairo_restore(cr) ;
  cairo_stroke_preserve(cr) ;
  cairo_clip(cr) ;

  //clock ticks
  for (i = 0; i < 12; i++) {
    double inset = 0.05;

    cairo_save(cr) ;
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    if(i % 3 != 0) {
      inset *= 0.8;
      cairo_set_line_width(cr, 0.03);
    }

    cairo_move_to(cr,
      (m_radius - inset) * cos (i * M_PI / 6),
      (m_radius - inset) * sin (i * M_PI / 6));
    cairo_line_to(cr,
      m_radius * cos (i * M_PI / 6),
      m_radius * sin (i * M_PI / 6));
    cairo_stroke(cr);
    cairo_restore(cr);
  }

  // store the current time
  time_t rawtime;
  time(&rawtime);
  struct tm * timeinfo = localtime (&rawtime);

  // compute the angles of the indicators of our clock
  double minutes = timeinfo->tm_min * M_PI / 30;
  double hours = timeinfo->tm_hour * M_PI / 6;
  double seconds= timeinfo->tm_sec * M_PI / 30;

  cairo_save(cr) ;
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

  // draw the seconds hand
  cairo_save(cr) ;
  cairo_set_line_width(cr, m_line_width / 3);
  cairo_set_source_rgba(cr, 0.612, 0.117,0.117, 0.8*opacity); // gray
  cairo_move_to(cr, 0, 0);
  cairo_line_to(cr, sin(seconds) * (m_radius * 0.9),
    -cos(seconds) * (m_radius * 0.9));
  cairo_stroke(cr) ;
  cairo_restore(cr) ;

  // draw the minutes hand
  cairo_set_source_rgba(cr, 0.117, 0.337, 0.612, 0.9*opacity);   // blue
  cairo_move_to(cr, 0, 0);
  cairo_line_to(cr, sin(minutes + seconds / 60) * (m_radius * 0.8),
    -cos(minutes + seconds / 60) * (m_radius * 0.8));
  cairo_stroke(cr) ;

  // draw the hours hand
  cairo_set_source_rgba(cr, 0.117, 0.337, 0.612, 0.9*opacity);   // green
  cairo_move_to(cr, 0, 0);
  cairo_line_to(cr, sin(hours + minutes / 12.0) * (m_radius * 0.5),
    -cos(hours + minutes / 12.0) * (m_radius * 0.5));
  cairo_stroke(cr) ;
  cairo_restore(cr) ;

  // draw a little dot in the middle
  cairo_arc(cr, 0, 0, m_line_width / 3.0, 0, 2 * M_PI);
  cairo_fill(cr) ;
}

/*
static void
patch_arc (cairo_pattern_t *pattern,
     double x, double y,
     double start, double end,
     double radius,
     double r, double g, double b, double a)
{
    double r_sin_A, r_cos_A;
    double r_sin_B, r_cos_B;
    double h;

    r_sin_A = radius * sin (start);
    r_cos_A = radius * cos (start);
    r_sin_B = radius * sin (end);
    r_cos_B = radius * cos (end);

    h = 4.0/3.0 * tan ((end - start) / 4.0);

    cairo_mesh_pattern_begin_patch (pattern);

    cairo_mesh_pattern_move_to (pattern, x, y);
    cairo_mesh_pattern_line_to (pattern,
        x + r_cos_A,
        y + r_sin_A);

    cairo_mesh_pattern_curve_to (pattern,
         x + r_cos_A - h * r_sin_A,
         y + r_sin_A + h * r_cos_A,
         x + r_cos_B + h * r_sin_B,
         y + r_sin_B - h * r_cos_B,
         x + r_cos_B,
         y + r_sin_B);

    cairo_mesh_pattern_line_to (pattern, x, y);

    cairo_mesh_pattern_set_corner_color_rgba (pattern, 0, r, g, b, a);
    cairo_mesh_pattern_set_corner_color_rgba (pattern, 1, r, g, b, 0);
    cairo_mesh_pattern_set_corner_color_rgba (pattern, 2, r, g, b, 0);
    cairo_mesh_pattern_set_corner_color_rgba (pattern, 3, r, g, b, a);

    cairo_mesh_pattern_end_patch (pattern);
}

static void
patch_line (cairo_pattern_t *pattern,
      double x0, double y0,
      double x1, double y1,
      double radius,
      double r, double g, double b, double a)
{
    double dx = y1 - y0;
    double dy = x0 - x1;
    double len = radius / hypot (dx, dy);

    dx *= len;
    dy *= len;

    cairo_mesh_pattern_begin_patch (pattern);

    cairo_mesh_pattern_move_to (pattern, x0, y0);
    cairo_mesh_pattern_line_to (pattern, x0 + dx, y0 + dy);
    cairo_mesh_pattern_line_to (pattern, x1 + dx, y1 + dy);
    cairo_mesh_pattern_line_to (pattern, x1, y1);

    cairo_mesh_pattern_set_corner_color_rgba (pattern, 0, r, g, b, a);
    cairo_mesh_pattern_set_corner_color_rgba (pattern, 1, r, g, b, 0);
    cairo_mesh_pattern_set_corner_color_rgba (pattern, 2, r, g, b, 0);
    cairo_mesh_pattern_set_corner_color_rgba (pattern, 3, r, g, b, a);

    cairo_mesh_pattern_end_patch (pattern);
}

static void
patch_rect (cairo_pattern_t *pattern,
      double x0, double y0,
      double x1, double y1,
      double radius,
      double r, double g, double b, double a)
{
    patch_arc  (pattern, x0, y0,   -M_PI, -M_PI/2, radius, r, g, b, a);
    patch_arc  (pattern, x1, y0, -M_PI/2,       0, radius, r, g, b, a);
    patch_arc  (pattern, x1, y1,       0,  M_PI/2, radius, r, g, b, a);
    patch_arc  (pattern, x0, y1,  M_PI/2,    M_PI, radius, r, g, b, a);

    patch_line (pattern, x0, y0, x1, y0, radius, r, g, b, a);
    patch_line (pattern, x1, y0, x1, y1, radius, r, g, b, a);
    patch_line (pattern, x1, y1, x0, y1, radius, r, g, b, a);
    patch_line (pattern, x0, y1, x0, y0, radius, r, g, b, a);
}
*/
static void render_shadow (cairo_t *cr, int x, int y, int w, int h, float r, float g, float b, float a) {

  //cairo_pattern_t *pattern;
  //float radius = 60;

  //pattern = cairo_pattern_create_mesh ();

  //patch_rect (pattern, x - w /2.0, y - h / 2.0, x + w / 2.0, y + h / 2.0, radius, r, g, b, a);

  //cairo_set_source(cr, pattern);
  //cairo_paint(cr);
}
