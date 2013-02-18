#include "wayrad.h"

int wayrad_frame (void *pointer, uint32_t time) {

	int updated;

	updated = 0;

	printf ("frame callback time is %u\r", time);
	fflush (stdout);

	return updated;
}

void wayrad_run (wayrad_t *wayrad) {
  
  int count;
  
  count = 0;

  while (count < 2000) {
    krad_wayland_iterate (wayrad->krad_wayland);
    count++;
  }
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
	wayrad->width = 960;
	wayrad->height = 540;

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

	wayrad->krad_wayland->render_test_pattern = 1;

	krad_wayland_prepare_window (wayrad->krad_wayland, wayrad->width, wayrad->height, &wayrad->buffer);

  //krad_wayland_set_frame_callback (krad_compositor_wayland_display->krad_wayland,
  //                 krad_compositor_wayland_display_render_callback,
  //                 krad_compositor_wayland_display);

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
