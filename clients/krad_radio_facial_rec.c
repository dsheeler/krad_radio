#include <cairo.h>
#include "kr_client.h"
#include <ccv.h>

#define FILENAME "/tmp/detect"

#define GREY  0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255 * 1.0
#define BLUE 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.255 / 0.255   * 1.0
#define ORANGE  0.255 / 0.255 * 1.0, 0.080 / 0.255 * 1.0, 0.0

typedef struct kr_snapshot {
  uint32_t width;
  uint32_t height;
  uint8_t *rgba;
	kr_videoport_t *videoport;
	kr_client_t *client;
  int sd[2];
  int got_frame;
} kr_snapshot;

static int destroy = 0;

int kr_snapshot_take(kr_snapshot *snapshot, char *filename) {

  int32_t ret;
  struct pollfd pollfds[1];
  cairo_surface_t *surface;

  ret = -1;

  if (strstr(filename, ".png") == NULL) {
    fprintf(stderr, "Its gotta be a .png file\n");
    return -1;
  }
  
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, snapshot->sd)) {
    fprintf(stderr, "Can't socketpair errno: %d\n", errno);
  } else {
    snapshot->got_frame = 0;
    pollfds[0].fd = snapshot->sd[0];
    pollfds[0].events = POLLIN;
    ret = poll(pollfds, 1, 666);
    if (ret != 1) {
	    if (destroy == 1) {
		    printf("Got signal!\n");
	    } else {
        fprintf(stderr, "It didn't work!\n");
      }
	  } 
    close(snapshot->sd[0]);
  }
	  return ret;
}
  
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
  
int videoport_process (void *buffer, void *user) {

	cairo_surface_t *cst;
	cairo_t *cr;
  kr_snapshot *snapshot;
  
  snapshot = (kr_snapshot *)user;

  if (snapshot->got_frame == 0) {
    memcpy(snapshot->rgba, buffer, snapshot->width * 
     snapshot->height * 4);
    snapshot->got_frame = 1;
    close(snapshot->sd[1]);
  }

	return 0;
}


int main (int argc, char *argv[]) {

  int i,j;
  int ret;
	uint32_t width;
	uint32_t height;
	kr_client_t *client;
	kr_videoport_t *videoport;
  kr_snapshot *snapshot;
  ccv_bbf_classifier_cascade_t* cascade;
  ret = 0;
  ccv_dense_matrix_t *image = 0;
  cairo_surface_t *surface;
	cairo_t *cr;

	if (argc != 3) {
    fprintf (stderr, "Takes station and face_file directory arguments.\n");
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
    fprintf (stderr, "Could not get compositor info!\n");
	  kr_client_destroy (&client);
	  return 1;
  }

	videoport = kr_videoport_create (client, OUTPUT);

	if (videoport == NULL) {
		fprintf (stderr, "Could not make videoport.\n");
	  kr_client_destroy (&client);
	  return 1;
	} else {
		printf ("Working!\n");
	}

  ccv_enable_default_cache();

  cascade = ccv_load_bbf_classifier_cascade(argv[2]);
 
  snapshot = calloc(1, sizeof(kr_snapshot));
  snapshot->client = client;
  snapshot->width = width;
  snapshot->height = height;
  
  snapshot->rgba = malloc(snapshot->width * snapshot->height * 4);
	snapshot->videoport = videoport;
 
	kr_videoport_set_callback (videoport, videoport_process, snapshot);

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);	
	
	kr_videoport_activate (videoport);

  char filename[1024];   
	for (i = 0; i < 3000; i++) {
	  usleep (30000);
    sprintf(filename, "%s_%d.png", FILENAME, i); 
    kr_snapshot_take(snapshot, filename);
    usleep (300000);
    image = 0;
    ccv_read(snapshot->rgba, &image, CCV_IO_ARGB_RAW
     | CCV_IO_GRAY, snapshot->height, snapshot->width, snapshot->width * 4);
    if (image != 0) {
		ccv_array_t* seq = ccv_bbf_detect_objects(image, &cascade, 1,
     ccv_bbf_default_params);
    for (j = 0; j < seq->rnum; j++) {
			ccv_comp_t* comp = (ccv_comp_t*)ccv_array_get(seq, j);
			printf("%d %d %d %d %f\n", comp->rect.x, comp->rect.y, comp->rect.width,
       comp->rect.height, comp->confidence);
      surface = cairo_image_surface_create_for_data(snapshot->rgba,
       CAIRO_FORMAT_ARGB32, snapshot->width, snapshot->height,
       snapshot->width * 4);
     	cr = cairo_create (surface);
	    render_smiley(cr, comp->rect.x, comp->rect.y, comp->rect.width,
      comp->rect.height);
      cairo_surface_flush (surface);
	    cairo_destroy (cr);
      if (cairo_surface_status(surface) != 0) {
        fprintf(stderr, "Could not make cairo surface: %s :/\n",
        cairo_status_to_string(cairo_surface_status(surface)));
      } else {
        usleep (50000);

        ret = cairo_surface_write_to_png(surface, filename);
        if (ret == 0) {
          printf("Yay! We rewrote: %s\n", filename);
        } else {
          fprintf(stderr, "Damn! Error %s writing %s\n",
           cairo_status_to_string(ret), filename);
          fprintf(stderr, "Surface Status: %s\n",
           cairo_status_to_string(cairo_surface_status(surface)));
        }
        cairo_surface_destroy(surface);
      }
    }
		printf("total : %d\n", seq->rnum);
		ccv_array_free(seq);
		ccv_matrix_free(image);
    }
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
  
  free(snapshot->rgba);
  free(snapshot);

	return ret;	
}
