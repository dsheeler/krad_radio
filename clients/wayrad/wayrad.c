#include "wayrad.h"

static void render_cratezone (wayrad_t *wayrad, uint32_t time, void *buffer);

int wayrad_frame (void *pointer, uint32_t time) {

	int updated;

	updated = 0;
	
	wayrad_t *wayrad = (wayrad_t *)pointer;
	
	//hexagon (wayrad->width, wayrad->height,
	//         wayrad->krad_wayland->display->pointer_x,
	//         wayrad->krad_wayland->display->pointer_y,
	//         time, wayrad->buffer);


  if (wayrad->cratezone.crates != wayrad->cratezone.crates_last) {
    render_cratezone (wayrad, time, wayrad->buffer);
    wayrad->cratezone.crates_last = wayrad->cratezone.crates;
    updated = 1;
  }

	//printf ("frame callback time is %u\r", time);
	//fflush (stdout);

	return updated;
}

static void render_cratezone (wayrad_t *wayrad, uint32_t time, void *buffer) {

	cairo_surface_t *cst;
	cairo_t *cr;
	char text[256];

	cst = cairo_image_surface_create_for_data ((unsigned char *)buffer,
												 CAIRO_FORMAT_ARGB32,
												 wayrad->width,
												 wayrad->height,
												 wayrad->width * 4);
	
	cr = cairo_create (cst);

  cairo_save (cr);
	cairo_set_source_rgba (cr, BGCOLOR_CLR);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);	
	cairo_paint (cr);
	cairo_restore (cr);


  sprintf (text, "Crates: %"PRIu64"", wayrad->cratezone.crates);

  cairo_select_font_face (cr, "Helvetica", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, 100);
  cairo_set_source_rgba (cr, GREY2, 0.8);
  
  cairo_move_to (cr, 128, 320);
  cairo_show_text (cr, text);
  cairo_stroke (cr);

	cairo_surface_flush (cst);
	cairo_destroy (cr);
	cairo_surface_destroy (cst);
}

void get_delivery (wayrad_t *wayrad) {

  kr_crate_t *crate;
  crate = NULL;

  kr_delivery_get (wayrad->client, &crate);

  if (crate != NULL) {
    //if (kr_crate_loaded (crate)) {
      //if ((crate->addr->path.unit == KR_MIXER) && 
      //    (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP)) {
        wayrad->cratezone.crates++;
      //}
    //}
    kr_crate_recycle (&crate);
  }
}

void *deliveries_thread (void *arg) {

  int b;
  int ret;
  uint64_t max;
  unsigned int timeout_ms;
  wayrad_t *wayrad;
  
  wayrad = (wayrad_t *)arg;
    
  ret = 0;
  b = 0;
  max = 1000;
  timeout_ms = 3000;
  

  kr_subscribe_all (wayrad->client);
  
  printf ("Waiting for up to %"PRIu64" deliveries for up to %ums each\n",
          max, timeout_ms);
  
  for (b = 0; b < max; b++) {
    ret = kr_wait (wayrad->client, timeout_ms);
    if (ret > 0) {
      get_delivery (wayrad);
    } else {
      printf (".");
      fflush (stdout);
    }
  }
  
  wayrad->done = 1;
  return NULL;  
}

void wayrad_run (wayrad_t *wayrad) {
  
  int count;
  kr_unit_control_t uc;
  char address[128];
  
  count = 0;


  pthread_create (&wayrad->deliveries_thread,
                  NULL,
                  deliveries_thread,
                  (void *)wayrad);

  pthread_detach (wayrad->deliveries_thread);

  while (!wayrad->done) {
    krad_wayland_iterate (wayrad->krad_wayland);
    
    if ((wayrad->krad_wayland->mousein) && (wayrad->krad_wayland->click)) {
      memset (&uc, 0, sizeof (uc));
      strcpy (address, "m/Music/c");
      if (kr_string_to_address (address, &uc.address)) {
        kr_unit_control_data_type_from_address (&uc.address, &uc.data_type);
        uc.value.real = (wayrad->krad_wayland->display->pointer_x / (float)wayrad->width);
        //vol
        //uc.value.real = ((uc.value.real * 100.0f) - 0.0f);
        //cross
        uc.value.real = ((uc.value.real * 200.0f) - 100.0f);
        uc.duration = 0;
        kr_unit_control_set (wayrad->client, &uc);
      }
    }

    count++;
  }
    
  printf ("bye bye now!\n");
    
}

void wayrad_destroy (wayrad_t *wayrad) {

  krad_wayland_close_window (wayrad->krad_wayland);

	krad_wayland_destroy (wayrad->krad_wayland);
  
  printf ("Disconnecting from %s..\n", wayrad->sysname);
  kr_disconnect (wayrad->client);
  printf ("Disconnected from %s.\n", wayrad->sysname);
  printf ("Destroying client..\n");
  kr_client_destroy (&wayrad->client);

  free (wayrad);
}

wayrad_t *wayrad_create (char *sysname) {

  wayrad_t *wayrad;
  
  wayrad = calloc (1, sizeof (wayrad_t));
  
  strncpy (wayrad->sysname, sysname, sizeof(wayrad->sysname));
	wayrad->width = 1280;
	wayrad->height = 720;

	wayrad->client = kr_client_create ("wayrad");

  if (wayrad->client == NULL) {
    fprintf (stderr, "Could create client\n");
    exit (1);
  }

  if (!kr_connect (wayrad->client, wayrad->sysname)) {
    fprintf (stderr, "Could not connect to %s krad radio daemon\n", wayrad->sysname);
    kr_client_destroy (&wayrad->client);
    exit (1);
  }

  printf ("Connected to %s!\n", wayrad->sysname);

	wayrad->krad_wayland = krad_wayland_create ();

	//wayrad->krad_wayland->render_test_pattern = 1;

	krad_wayland_prepare_window (wayrad->krad_wayland, wayrad->width, wayrad->height, &wayrad->buffer);

  krad_wayland_set_frame_callback (wayrad->krad_wayland, wayrad_frame, wayrad);

  krad_wayland_prepare_window (wayrad->krad_wayland,
                 wayrad->width,
                 wayrad->height,
                 &wayrad->buffer);

  printk("Wayland display prepared");

  krad_wayland_open_window (wayrad->krad_wayland);

  printk("Wayland display running");

  return wayrad;  
  
}

int main (int argc, char *argv[]) {

  wayrad_t *wayrad;

  if (argc == 2) {
    wayrad = wayrad_create (argv[1]);
    wayrad_run (wayrad);
    wayrad_destroy (wayrad);
  } else {
    printf ("need station name\n");
  }

	return 0;

}
