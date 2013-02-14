#include "krad_vector.h"

static void krad_vector_render_meter (cairo_t *cr, int x, int y, int size, float pos, float opacity);
static void krad_vector_render_hex (cairo_t *cr, int x, int y, int w, float r, float g, float b, float opacity);
static void krad_vector_render_grid (cairo_t *cr, int x, int y, int w, int h, int lines, float r, float g, float b, float opacity);
static void krad_vector_render_curve (cairo_t *cr, int w, int h);
static void krad_vector_render_circle (cairo_t *cr, int x, int y, float radius, float r, float g, float b, float opacity);
static void krad_vector_render_rectangle (cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity);
static void krad_vector_render_triangle (cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity);
static void krad_vector_render_arrow (cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity);
static void krad_vector_render_viper (cairo_t * cr, int x, int y, int size, int direction);
static void krad_vector_render_clock (cairo_t *cr, int x, int y, int width, int height, float opacity);
static void krad_vector_render_blurred_rectangle (cairo_t *cr, int x, int y, int w, int h, float r, float g, float b, float a);

void krad_vector_destroy_arr (krad_vector_t *vector, int count) {
  
  int s;
  
  s = 0;
  
  for (s = 0; s < count; s++) {
    krad_vector_reset (&vector[s]);
  }

  free (vector);
}

krad_vector_t *krad_vector_create_arr (int count) {

  int s;
  krad_vector_t *vector;

  s = 0;

  if ((vector = calloc (count, sizeof (krad_vector_t))) == NULL) {
    failfast ("Krad Vector mem alloc fail");
  }
  
  for (s = 0; s < count; s++) {
    vector[s].subunit.address.path.unit = KR_COMPOSITOR;
    vector[s].subunit.address.path.subunit.compositor_subunit = KR_VECTOR;
    vector[s].subunit.address.id.number = s;
    krad_vector_reset (&vector[s]);
  }
  
  return vector;
}

void krad_vector_reset (krad_vector_t *vector) {
  krad_compositor_subunit_reset (&vector->subunit);
}

void krad_vector_set_type (krad_vector_t *krad_vector, char *vector_type) {
  krad_vector->type = krad_string_to_vector_type (vector_type);
}

int krad_vector_to_rep (krad_vector_t *vector, krad_vector_rep_t *vector_rep) {
  
  if ((vector == NULL) || (vector_rep == NULL)) {
    return 0;
  }
  
  //vector_rep->type = vector->type;

  //vector_rep->controls.red = vector->subunit->red;
  //vector_rep->controls.green = vector->subunit->green;
  //vector_rep->controls.blue = vector->subunit->blue;
  
  vector_rep->controls.x = vector->subunit.x;
  vector_rep->controls.y = vector->subunit.y;
  vector_rep->controls.z = vector->subunit.z;
  
  //vector_rep->controls.duration = vector->subunit->duration;

  vector_rep->controls.width = vector->subunit.width;
  vector_rep->controls.height = vector->subunit.height;
    
  vector_rep->controls.xscale = vector->subunit.xscale;
  vector_rep->controls.yscale = vector->subunit.yscale;
    
  vector_rep->controls.rotation = vector->subunit.rotation;
  vector_rep->controls.opacity = vector->subunit.opacity;
   
  return 1;
}

void krad_vector_render (krad_vector_t *krad_vector, cairo_t *cr) {

  float peak;
  cairo_save (cr);

  if ((krad_vector->subunit.xscale != 1.0f) || (krad_vector->subunit.yscale != 1.0f)) {
    cairo_translate (cr, krad_vector->subunit.x, krad_vector->subunit.y);
    //cairo_translate (cr, ((krad_vector->subunit.width / 2) * krad_vector->subunit.xscale),
    //       ((krad_vector->subunit.height / 2) * krad_vector->subunit.yscale));
    cairo_translate (cr, krad_vector->subunit.width / 2, krad_vector->subunit.height / 2);
    cairo_scale (cr, krad_vector->subunit.xscale, krad_vector->subunit.yscale);
    cairo_translate (cr, ((krad_vector->subunit.width / -2) * krad_vector->subunit.xscale),
            ((krad_vector->subunit.height / -2) * krad_vector->subunit.yscale));
    //cairo_translate (cr, krad_vector->subunit.width / -2, krad_vector->subunit.height / -2);
    cairo_translate (cr, krad_vector->subunit.x * -1, krad_vector->subunit.y * -1);    
  }
  
  if (krad_vector->subunit.rotation != 0.0f) {
    cairo_translate (cr, krad_vector->subunit.x, krad_vector->subunit.y);  
    cairo_translate (cr, krad_vector->subunit.width / 2, krad_vector->subunit.height / 2);
    cairo_rotate (cr, krad_vector->subunit.rotation * (M_PI/180.0));
    cairo_translate (cr, krad_vector->subunit.width / -2, krad_vector->subunit.height / -2);    
    cairo_translate (cr, krad_vector->subunit.x * -1, krad_vector->subunit.y * -1);
  }


  
  switch ( krad_vector->type ) {

  case METER:
    //peak = krad_mixer_portgroup_read_peak (krad_vector->krad_mixer_portgroup);
    //peak = krad_mixer_peak_scale (peak);
    peak = 50.0f;
    krad_vector_render_meter (cr, krad_vector->subunit.x, krad_vector->subunit.y, krad_vector->subunit.yscale, peak, krad_vector->subunit.opacity);
    break;
    
  case HEX:

    krad_vector_render_hex (cr, krad_vector->subunit.x, 
                            krad_vector->subunit.y, 
                            krad_vector->subunit.width,
//                            0.3 * peak,
                            krad_vector->subunit.red,
                            krad_vector->subunit.green,
                            krad_vector->subunit.blue,
                            krad_vector->subunit.opacity);
    break;
    
  case CURVE:
    krad_vector_render_curve (cr, krad_vector->subunit.x,
                              krad_vector->subunit.y);
    break;
  
  case CIRCLE:
    krad_vector_render_circle (cr, krad_vector->subunit.x,
                               krad_vector->subunit.y,
                               krad_vector->subunit.width,
                               krad_vector->subunit.red,
                               krad_vector->subunit.green,
                               krad_vector->subunit.blue,
                               krad_vector->subunit.opacity);
     break;
  case RECT:

    //krad_vector->subunit.width = krad_vector->subunit.xscale;
    //krad_vector->subunit.height = krad_vector->subunit.yscale;
    krad_vector_render_rectangle (cr, krad_vector->subunit.x,
                                  krad_vector->subunit.y,
                                  krad_vector->subunit.width,
                                  krad_vector->subunit.height,
                                  krad_vector->subunit.red,
                                  krad_vector->subunit.green,
                                  krad_vector->subunit.blue,
                                  krad_vector->subunit.opacity);
    break;

  case TRIANGLE:


    krad_vector_render_triangle (cr, krad_vector->subunit.x - krad_vector->subunit.xscale * sin (krad_vector->subunit.rotation * M_PI/180.0),
                                 krad_vector->subunit.y - krad_vector->subunit.xscale * cos( 30 * M_PI/180.0) * cos (krad_vector->subunit.rotation * M_PI/180.0),
                                 krad_vector->subunit.width,
                                 krad_vector->subunit.height,
                                 krad_vector->subunit.red,
                                 krad_vector->subunit.green,
                                 krad_vector->subunit.blue,
                                 krad_vector->subunit.opacity);
    break;

  case ARROW:

    krad_vector_render_arrow (cr, krad_vector->subunit.x,
                                 krad_vector->subunit.y,
                                 krad_vector->subunit.width,
                                 krad_vector->subunit.height,
                                 krad_vector->subunit.red,
                                 krad_vector->subunit.green,
                                 krad_vector->subunit.blue,
                                 krad_vector->subunit.opacity);
    break;

  case GRID:


    krad_vector_render_grid (cr, krad_vector->subunit.x,
                             krad_vector->subunit.y,
                             krad_vector->subunit.width,
                             krad_vector->subunit.height,
                             6,
                             krad_vector->subunit.red,
                             krad_vector->subunit.green,
                             krad_vector->subunit.blue,
                             krad_vector->subunit.opacity);
    break;

  case VIPER:
    krad_vector_render_viper (cr, krad_vector->subunit.x,
                              krad_vector->subunit.y,
                              (int) krad_vector->subunit.xscale, (int) krad_vector->subunit.yscale);
    break;

  case CLOCK:


    krad_vector_render_clock (cr, krad_vector->subunit.x,
                              krad_vector->subunit.y,
                              (int) krad_vector->subunit.width,
                              (int) krad_vector->subunit.height,
                               krad_vector->subunit.opacity);
    break;

  case SHADOW:

    krad_vector_render_blurred_rectangle (cr, krad_vector->subunit.x,
                                          krad_vector->subunit.y,
                                          krad_vector->subunit.width,
                                          krad_vector->subunit.height,
                                          krad_vector->subunit.red,
                                          krad_vector->subunit.green,
                                          krad_vector->subunit.blue,
                                          krad_vector->subunit.opacity);
    break;
  default:
    break;
  }
  
  cairo_restore (cr);
}

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

static void krad_vector_render_blurred_rectangle (cairo_t *cr, int x, int y, int w, int h, float r, float g, float b, float a) {

    cairo_pattern_t *pattern;
    float radius = 60;

    pattern = cairo_pattern_create_mesh ();

    cairo_set_source_rgba (cr, r, g, b, a);
    cairo_paint (cr);

    patch_rect (pattern, x - w /2.0, y - h / 2.0, x + w / 2.0, y + h / 2.0, radius, 0, 0, 0, a);

    cairo_set_source (cr, pattern);
    cairo_paint (cr);

}

static void krad_vector_render_meter (cairo_t *cr, int x, int y, int size, float pos, float opacity) {


  pos = pos * 1.8f - 90.0f;

  cairo_save(cr);

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
  //
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

  cairo_restore(cr);

}

static void krad_vector_render_hex (cairo_t *cr, int x, int y, int w, float r, float g, float b, float opacity) {

  cairo_pattern_t *pat;

  static float hexrot = 0;

  cairo_save(cr);

  cairo_set_line_width(cr, 1);
  cairo_set_source_rgba(cr, r, g, b, opacity);

  int r1;
  float scale;

  scale = 2.5;

  r1 = ((w)/2 * sqrt(3));

  cairo_translate (cr, x, y);
  cairo_rotate (cr, hexrot * (M_PI/180.0));
  cairo_translate (cr, -(w/2), -r1);

  // draw radius 
  //cairo_move_to (cr, w/2, 0);
  //cairo_line_to (cr, w/2, r1);
  //cairo_stroke (cr);

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
  cairo_rel_line_to (cr, w, 0);
  //cairo_stroke_preserve (cr);


  hexrot += 1.5;
  cairo_fill (cr);

  cairo_restore(cr);


//-----------------------

  cairo_save(cr);

  cairo_set_line_width(cr, 1.5);
  cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
  cairo_set_source_rgba(cr, GREY, opacity);


  cairo_translate (cr, x, y);
  cairo_rotate (cr, hexrot * (M_PI/180.0));
  cairo_translate (cr, -((w * scale)/2), -r1 * scale);
  cairo_scale(cr, scale, scale);
  //hexrot += 0.11;

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
  cairo_rel_line_to (cr, w, 0);

  //cairo_stroke_preserve (cr);

  cairo_rotate (cr, 60 * (M_PI/180.0));


  // draw radius 
  //cairo_move_to (cr, w/2, 0);
  //cairo_line_to (cr, w/2, r1);
  //cairo_stroke_preserve (cr);

  cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
  pat = cairo_pattern_create_radial (w/2, r1, 3, w/2, r1, r1*scale);
  cairo_pattern_add_color_stop_rgba (pat, 0, r, g, b, opacity);
  //cairo_pattern_add_color_stop_rgba (pat, 0.6, 0, 0, 1, 0.3);
  //cairo_pattern_add_color_stop_rgba (pat, 0.8, 0, 0, 1, 0.05);
  cairo_pattern_add_color_stop_rgba (pat, 0.4, 0, 0, 0, 0);
  cairo_set_source (cr, pat);

  cairo_fill (cr);
  cairo_pattern_destroy (pat);



  cairo_restore(cr);

}

static void krad_vector_render_grid (cairo_t *cr, int x, int y, int w, int h, int lines, float r, float g, float b, float opacity) {

  int count;

  //srand(time(NULL));
  cairo_save(cr);
  cairo_translate (cr, x + w/2, y + h/2);

  cairo_translate (cr, -x + -w/2, -y + -h/2);
  cairo_set_line_width(cr, 10);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);
  cairo_set_source_rgba(cr, r, g, b, opacity);

  cairo_save(cr);

  for (count = 1; count != lines; count++) {

    cairo_move_to (cr, x + (w/lines) * count, y);
    cairo_line_to (cr, x + (w/lines) * count, y + h);
    cairo_stroke (cr);

  }

  cairo_restore(cr);

  for (count = 1; count != lines; count++) {

    cairo_move_to (cr, x, y + (h/lines) * count);
    cairo_line_to (cr, x + w, y + (h/lines) * count);
    cairo_stroke (cr);

  }


  cairo_rectangle (cr, x, y, w, h);
  cairo_stroke (cr);

  cairo_restore(cr);
}

static void krad_vector_render_curve (cairo_t *cr, int w, int h) {

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
    //new_speed = 1;
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
    
  ///printk ("f1 %f f2 %f\n", pointy_final, pointy_final_adj);
  cairo_move_to (cr, 20, 70);
  cairo_curve_to (cr, point1_x, point1_y, point2_x, point2_y, 0.85 * w, 0.85 * h);
  cairo_stroke (cr);
  
}

static void krad_vector_render_circle (cairo_t *cr, int x, int y, float radius, float r, float g, float b, float opacity) {
  
  cairo_set_source_rgba(cr, r, g, b, opacity);
  cairo_arc (cr, x, y, radius, 0, 2*M_PI);
  cairo_fill (cr);
  
}

static void krad_vector_render_rectangle (cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity) {

  cairo_save(cr);

  cairo_set_source_rgba(cr, r, g, b, opacity);
//  printk ("scales: %f %f", w, h);
  cairo_set_line_width(cr, 0);
  
//    cairo_rectangle(cr, (double)x, (double)y, (double)w, (double)h);
    

  cairo_move_to (cr, x-w/2.0, y-h/2.0);
  cairo_rel_line_to (cr, w, 0);
  cairo_rel_line_to (cr, 0, h);
  cairo_rel_line_to (cr, -w, 0);
  cairo_close_path (cr);

  cairo_stroke_preserve(cr);

  cairo_fill(cr);

  cairo_restore(cr);

}

static void krad_vector_render_triangle (cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity) {

  cairo_save(cr);

  cairo_set_source_rgba(cr, r, g, b, opacity);
  cairo_set_line_width(cr, 0);

  cairo_move_to (cr, x, y);
  cairo_rel_line_to (cr, w, 0);
  cairo_rotate (cr, 120 * (M_PI/180.0));
  cairo_rel_line_to (cr, w, 0);
  cairo_rotate (cr, 120 * (M_PI/180.0));
  cairo_rel_line_to (cr, w, 0);
  cairo_close_path (cr);

  cairo_stroke_preserve(cr);

  cairo_fill(cr);

  cairo_restore(cr);

}


static void krad_vector_render_arrow (cairo_t *cr, int x, int y, float w, float h, float r, float g, float b, float opacity) {

  cairo_save(cr);

  cairo_set_source_rgba(cr, r, g, b, opacity);
  cairo_set_line_width(cr, 0);

  cairo_move_to (cr, x, y);
  cairo_rel_line_to (cr, w, 0);
  cairo_rel_line_to (cr, 0, h);
  cairo_rel_line_to (cr, w/4, 0);
  cairo_rel_line_to (cr, -3*w/4, w);
  cairo_rel_line_to (cr, -3*w/4, -w);
  cairo_rel_line_to (cr, w/4, 0);
  cairo_rel_line_to (cr, 0, -h);

  cairo_close_path (cr);

  cairo_stroke_preserve(cr);

  cairo_fill(cr);

  cairo_restore(cr);

}

static void krad_vector_render_viper (cairo_t * cr, int x, int y, int size, int direction) {

  cairo_save(cr);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);

  cairo_set_line_width(cr, size/7);
  cairo_set_source_rgb(cr, WHITE);
  cairo_translate (cr, x, y);

  cairo_arc (cr, 0.0, 0.0, size/2, 0, 2*M_PI);
  cairo_stroke(cr);

  cairo_set_source_rgb(cr, GREEN);

  cairo_arc (cr, 0.0, -size/5, size/14, 0, 2*M_PI);
  cairo_stroke(cr);

  cairo_rotate (cr, 40 * (M_PI/180.0));
  cairo_arc (cr, size/3.8, 0.0, size/14, 0, 2*M_PI);
  cairo_stroke(cr);
  cairo_move_to(cr, size/1.8,  0.0);
  cairo_rel_line_to (cr, size/2.3, 0.0);
  cairo_stroke(cr);

  cairo_rotate (cr, 100 * (M_PI/180.0));
  cairo_arc (cr, size/3.8, 0.0, size/14, 0, 2*M_PI);
  cairo_stroke(cr);
  cairo_move_to(cr, size/1.8,  0.0);
  cairo_rel_line_to (cr, size/2.3, 0.0);
  cairo_stroke(cr);

  cairo_rotate (cr, 130 * (M_PI/180.0));
  cairo_move_to(cr, size/1.8,  0.0);
  cairo_rel_line_to (cr, size/2.3, 0.0);
  cairo_stroke(cr);

  cairo_set_source_rgb(cr, WHITE);
  cairo_rotate (cr, (180 + direction - 90) * (M_PI/180.0));
  cairo_move_to(cr, size/1.6,  size/6);
  cairo_rel_line_to (cr, size/6, -size/6);
  cairo_rel_line_to (cr, -size/6, -size/6);
  //cairo_rel_line_to(cr, size/1.6,  size/6);
  cairo_fill(cr);

  cairo_set_line_width(cr, size/7);
  cairo_rotate (cr, 180 * (M_PI/180.0));
  cairo_move_to(cr, size/1.6, 0.0);
  cairo_rel_line_to (cr, size/8, 0.0);
  cairo_stroke(cr);

  cairo_restore(cr);

}

static void krad_vector_render_clock (cairo_t *cr, int x, int y, int width, int height, float opacity) {

  double m_line_width = 0.051;
  double m_radius = 0.42;
  int i;

  cairo_translate(cr, x, y);

  // scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
  // the center of the window

  cairo_scale(cr, width, height);
  cairo_translate(cr, 0.5, 0.5);
  cairo_set_line_width(cr, m_line_width);
  cairo_set_source_rgba(cr,  0.117, 0.337, 0.612, opacity);
 // cairo_save(cr) ;
 // cairo_set_source_rgba(cr, 0.337, 0.612, 0.117, 0.9*opacity);   // green
 // cairo_paint(cr) ;
 // cairo_restore(cr) ;
  cairo_arc(cr, 0, 0, m_radius, 0, 2 * M_PI);
  cairo_save(cr) ;
  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.8*opacity);
  cairo_fill_preserve(cr) ;
  cairo_restore(cr) ;
  cairo_stroke_preserve(cr) ;
  cairo_clip(cr) ;

  //clock ticks
  for (i = 0; i < 12; i++)
  {
    double inset = 0.05;

    cairo_save(cr) ;
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

    if(i % 3 != 0)
    {
      inset *= 0.8;
      cairo_set_line_width(cr, 0.03);
    }

    cairo_move_to(cr,
      (m_radius - inset) * cos (i * M_PI / 6),
      (m_radius - inset) * sin (i * M_PI / 6));
    cairo_line_to (cr,
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

