#include "hex.h"

static void render_hex (cairo_t *cr, int x, int y, int w) {

	cairo_pattern_t *pat;
	static float hexrot = 0;
	int r1;
	float scale;
		
	cairo_save(cr);
	cairo_set_line_width(cr, 1);
	cairo_set_source_rgb(cr, ORANGE);

	scale = 2.5;
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
	cairo_rel_line_to (cr, w, 0);
	hexrot += 2.5;
	cairo_fill (cr);
	
	cairo_restore(cr);
	cairo_save(cr);
		
	cairo_set_line_width(cr, 1.5);
	cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
	cairo_set_source_rgb(cr, GREY);


	cairo_translate (cr, x, y);
	cairo_rotate (cr, hexrot * (M_PI/180.0));
	cairo_translate (cr, -((w * scale)/2), -r1 * scale);
	cairo_scale(cr, scale, scale);

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
	
	cairo_rotate (cr, 60 * (M_PI/180.0));

	cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
	pat = cairo_pattern_create_radial (w/2, r1, 3, w/2, r1, r1*scale);
	cairo_pattern_add_color_stop_rgba (pat, 0, 0, 0, 1, 1);
	cairo_pattern_add_color_stop_rgba (pat, 0.4, 0, 0, 0, 0);
	cairo_set_source (cr, pat);
	
	cairo_fill (cr);
	cairo_pattern_destroy (pat);
	cairo_restore(cr);

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
	//cairo_save (cr);
	//cairo_set_source_rgba (cr, BGCOLOR_CLR);
	//cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);	
	//cairo_paint (cr);
	//cairo_restore (cr);
	
	
	uint32_t *p;
	int i, end, offset;

	p = buffer;
	end = width * height;
	offset = time >> 4;
	for (i = 0; i < end; i++) {
		p[i] = (i + offset) * 0x0080401;
	}
	
	if ((x > 0) && (y > 0)) {
	  render_hex (cr, x, y, 66);
	}
	cairo_surface_flush (cst);
	cairo_destroy (cr);
	cairo_surface_destroy (cst);
}
