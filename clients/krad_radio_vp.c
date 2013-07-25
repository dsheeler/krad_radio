#include <cairo.h>
#include "kr_client.h"

#define GREY  0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255 * 1.0
#define BLUE 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.255 / 0.255   * 1.0
#define ORANGE  0.255 / 0.255 * 1.0, 0.080 / 0.255 * 1.0, 0.0

typedef struct videoport_demo_St videoport_demo_t;

struct videoport_demo_St {
  uint32_t width;
  uint32_t height;
  uint32_t frames;
};

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

void render_hex (cairo_t *cr, int x, int y, int w) {

	//cairo_pattern_t *pat;
	static float hexrot = 0;
	int r1;
	//float scale;

	cairo_save(cr);
	cairo_set_line_width(cr, 1);
	cairo_set_source_rgb(cr, ORANGE);

	//scale = 2.5;
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
	hexrot += 1.5;
	cairo_fill (cr);

	cairo_restore(cr);

/*
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
*/
}


int videoport_process (void *buffer, void *user) {

	cairo_surface_t *cst;
	cairo_t *cr;
  videoport_demo_t *demo;

  demo = (videoport_demo_t *)user;

	cst = cairo_image_surface_create_for_data ((unsigned char *)buffer,
												 CAIRO_FORMAT_ARGB32,
												 demo->width,
												 demo->height,
												 demo->width * 4);

	cr = cairo_create (cst);
	cairo_save (cr);
	cairo_set_source_rgba (cr, BGCOLOR_CLR);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint (cr);
	cairo_restore (cr);
	render_hex (cr, demo->width / 2, demo->height / 2, 66);
	cairo_surface_flush (cst);
	cairo_destroy (cr);
	cairo_surface_destroy (cst);

  demo->frames++;

	return 0;
}

int main (int argc, char *argv[]) {

  int i;
  int ret;
	uint32_t width;
	uint32_t height;
	kr_client_t *client;
	kr_videoport_t *videoport;
  videoport_demo_t *demo;

  ret = 0;

	if (argc != 2) {
		if (argc > 2) {
			fprintf (stderr, "Only takes station argument.\n");
		} else {
			fprintf (stderr, "No station specified.\n");
		}
		return 1;
	}

	client = kr_client_create ("krad videoport client");

	if (client == NULL) {
		fprintf (stderr, "Could not create KR client.\n");
		return 1;
	}

  kr_connect (client, argv[1]);

  if (!kr_connected (client)) {
		fprintf (stderr, "Could not connect to %s krad radio daemon.\n", argv[1]);
	  kr_client_destroy (&client);
	  return 1;
  }

  if (kr_compositor_get_info_wait (client, &width, &height, NULL, NULL) != 1) {
	  kr_client_destroy (&client);
	  return 1;
  }

  if (kr_compositor_get_info_wait (client, &width, &height, NULL, NULL) != 1) {
    fprintf (stderr, "Could not get compositor info!\n");
	  kr_client_destroy (&client);
	  return 1;
  }
  //FIXME
	videoport = kr_videoport_create (client, 0);

	if (videoport == NULL) {
		fprintf (stderr, "Could not make videoport.\n");
	  kr_client_destroy (&client);
	  return 1;
	} else {
		printf ("Working!\n");
	}

  demo = calloc (1, sizeof (videoport_demo_t));

  demo->width = width;
  demo->height = height;

	kr_videoport_set_callback (videoport, videoport_process, demo);

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);

	kr_videoport_activate (videoport);

	for (i = 0; i < 3000; i++) {
	  usleep (30000);
	  if (destroy == 1) {
		  printf ("Got signal!\n");
	    break;
	  }
    if (kr_videoport_error (videoport)) {
      printf ("Error: %s\n", "videoport Error");
      ret = 1;
      break;
    }
	}

	kr_videoport_deactivate (videoport);

	kr_videoport_destroy (videoport);

	kr_client_destroy (&client);

	if (demo->frames > 0) {
		printf ("Rendered %d frames!\n", demo->frames);
	}

  free (demo);

	if (ret == 0) {
		printf ("Worked!\n");
	}

	return ret;
}
