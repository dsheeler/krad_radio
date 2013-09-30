#include <cairo.h>
#include "kr_client.h"
#include <ccv.h>

typedef struct kr_snapshot {
  uint32_t width;
  uint32_t height;
  uint8_t *rgba;
	kr_videoport_t *videoport_in;
	kr_client_t *client_in;
  ccv_array_t* seq;
  int got_frame;
} kr_snapshot;

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

void render_smiley (cairo_t *cr, int x_in, int y_in, int width, int height) {
  double pi = 3.14f;
  double w = 1.5 * (double) width;
  double h = 1.5 * (double) height;
  
  double x = x_in -  0.18*w;
  double y = y_in -  0.18*h;


  cairo_save(cr);
  cairo_set_line_width(cr, 6);

  cairo_set_source_rgb(cr, 0.2, 0.4, 0.8 );

  //eyes
  cairo_move_to(cr, x + w/3.f, y + h/3.f );
  cairo_rel_line_to(cr, 0.f, h/6.f );
  cairo_move_to(cr, x + 2*(w/3.f), y + h/3.f );
  cairo_rel_line_to(cr, 0.f, h/6.f );
  cairo_stroke(cr);

  cairo_set_source_rgb(cr, 1.f, 1.f, 0.f );

  double rad = (w > h) ? h : w;
  //face
  cairo_arc(cr, x + w/2.f, y + h/2.f, (rad/2.f) - 20.f,0.f,2.f * pi );
  cairo_stroke(cr);
  //smile  
  cairo_arc(cr, x + w/2.f, y + h/2.f, (rad/3.f) -10.f, pi/3.f, 2.f * (pi/3.f));
  cairo_stroke(cr);
  cairo_restore(cr);

}
 
int videoport_process_out (void *buffer, void *user) {

  cairo_surface_t *surface;
	cairo_t *cr;
  int j;

  kr_snapshot *snapshot;
  snapshot = (kr_snapshot *)user;
  while (snapshot->got_frame != 1) { usleep(500); } 

  if (snapshot->got_frame == 1) {
    memcpy(buffer, snapshot->rgba, snapshot->width
     * snapshot->height * 4);
    snapshot->got_frame = 0;
    if (snapshot->seq && snapshot->seq->rnum > 0) {
      surface = cairo_image_surface_create_for_data(buffer,
       CAIRO_FORMAT_ARGB32, snapshot->width, snapshot->height,
       snapshot->width * 4);
     	cr = cairo_create (surface);
	    for (j = 0; j < snapshot->seq->rnum; j++) {
			  ccv_comp_t* comp = (ccv_comp_t*)ccv_array_get(snapshot->seq, j);
        render_smiley(cr, comp->rect.x, comp->rect.y, comp->rect.width,
         comp->rect.height);
      }
      cairo_surface_flush (surface);
		  //ccv_array_free(snapshot->seq);
 
      cairo_surface_destroy(surface);
      cairo_destroy (cr);
    }
  }
      
	return 0;
}

int videoport_process_in (void *buffer, void *user) {

  kr_snapshot *snapshot;
  
  snapshot = (kr_snapshot *)user;

  if (snapshot->got_frame == 0) {
    memcpy(snapshot->rgba, buffer, snapshot->width * 
     snapshot->height * 4);
    snapshot->got_frame = 1;
  }

	return 0;
}


int main (int argc, char *argv[]) {

  int ret;
  int i;
	uint32_t width;
	uint32_t height;
	kr_client_t *client_in, *client_out;
	kr_videoport_t *videoport_in;
	kr_videoport_t *videoport_out;
  kr_snapshot *snapshot;
  ccv_dense_matrix_t *image = 0;
  ccv_bbf_classifier_cascade_t *cascade;
  ret = 0;

	if (argc != 4) {
    fprintf (stderr, "Takes two station arguments and cascade-file-directory.\n");
    return 1;
	}
	
	client_in = kr_client_create ("krad in videoport client");
	client_out = kr_client_create ("krad out videoport client");

	if (client_out == NULL) {
		fprintf (stderr, "Could not create output KR client.\n");
		return 1;
	}	

	if (client_in == NULL) {
		fprintf (stderr, "Could not create input KR client.\n");
		return 1;
	}	

  kr_connect(client_in, argv[1]);
  kr_connect(client_out, argv[2]);

  if (!kr_connected (client_in)) {
		fprintf (stderr, "Could not connect to %s krad radio daemon.\n", argv[1]);
	  kr_client_destroy (&client_in);
	  return 1;
  }
	  if (!kr_connected (client_out)) {
		fprintf (stderr, "Could not connect to %s krad radio daemon.\n", argv[2]);
	  kr_client_destroy (&client_out);
	  return 1;
  }
	
  if (kr_compositor_get_info_wait (client_in, &width, &height, NULL, NULL) != 1) {
    fprintf (stderr, "Could not get compositor info!\n");
	  kr_client_destroy (&client_in);
	  return 1;
  }

	videoport_in = kr_videoport_create (client_in, OUTPUT);
	videoport_out = kr_videoport_create (client_out, INPUT);

	if (videoport_in == NULL) {
		fprintf (stderr, "Could not make input videoport.\n");
	  kr_client_destroy (&client_in);
	  return 1;
	} else {
		printf ("Input Working!\n");
	}

	if (videoport_out == NULL) {
		fprintf (stderr, "Could not make output videoport.\n");
	  kr_client_destroy (&client_out);
	  return 1;
	} else {
		printf ("Output Working!\n");
	}

  snapshot = calloc(1, sizeof(kr_snapshot));
  snapshot->width = width;
  snapshot->height = height;
  snapshot->seq = 0; 
  ccv_enable_default_cache();

  cascade = ccv_load_bbf_classifier_cascade(argv[3]);
  
  snapshot->rgba = malloc(snapshot->width * snapshot->height * 4);
 
	kr_videoport_set_callback (videoport_in, videoport_process_in, snapshot);
	kr_videoport_set_callback (videoport_out, videoport_process_out, snapshot);

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);	
	
	kr_videoport_activate (videoport_in);
	kr_videoport_activate (videoport_out);

	while (1) {
    image = 0;
    ccv_read(snapshot->rgba, &image, CCV_IO_ARGB_RAW
     | CCV_IO_GRAY, snapshot->height, snapshot->width, snapshot->width * 4);
    if (image != 0) {
		  snapshot->seq = ccv_bbf_detect_objects(image, &cascade, 1,
       ccv_bbf_default_params);
		  ccv_matrix_free(image);
    }
    if (destroy == 1) {
		  printf ("Got signal!\n");
	    break;
	  }
    if (kr_videoport_error (videoport_in) || kr_videoport_error(videoport_out)) {
      printf ("Error: %s\n", "videoport Error");
      ret = 1;
      break;
    }
	}
	
	kr_videoport_deactivate (videoport_in);
	kr_videoport_deactivate (videoport_out);
	
	kr_videoport_destroy (videoport_in);
	kr_videoport_destroy (videoport_out);

	kr_client_destroy (&client_in);
	kr_client_destroy (&client_out);
  
	return ret;	
}
