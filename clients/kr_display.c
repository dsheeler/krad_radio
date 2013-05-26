#include <cairo/cairo.h>
#include <libswscale/swscale.h>

#include "kr_client.h"
#include "krad_udp.h"
#include "krad_vpx.h"
#include "krad_wayland.h"

#define GREY  0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255   * 1.0
#define BLUE 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255   * 1.0, 0.255 / 0.255   * 1.0
#define ORANGE  0.255 / 0.255 * 1.0, 0.080 / 0.255 * 1.0, 0.0

#define DEFAULT_FBUFSIZE 120

typedef struct kr_display_St kr_display_t;

struct kr_display_St {
  uint32_t width;
  uint32_t height;
	krad_wayland_t *krad_wayland;
	kr_videoport_t *videoport;
	kr_client_t *client;
  char *station;
  uint32_t frame_size;
  uint32_t framebufsize;
  uint8_t *rgba;
  void *buffer;
  uint64_t dframes;
  uint64_t frames;
};

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

int display_cb (void *user, uint32_t time) {

  kr_display_t *display;
  int32_t pos;

  display = (kr_display_t *)user;

  if (display->frames > display->dframes) {
    pos = (display->dframes % display->framebufsize);
    memcpy (display->buffer,
            display->rgba + (pos * display->frame_size),
            display->frame_size);
    display->dframes++;
  	return 1;
  }
	return 0;
}

void kr_display_free_framebuf (kr_display_t *display) {
  if (display->rgba != NULL) {
    free (display->rgba);
    display->rgba = NULL;
  }
}

void kr_display_alloc_framebuf (kr_display_t *display) {
  display->framebufsize = DEFAULT_FBUFSIZE;
  display->frame_size = display->width * display->height * 4;
  display->rgba = malloc (display->frame_size * display->framebufsize);
}

int new_frame (void *buffer, void *user) {

  int32_t pos;
  kr_display_t *display;
	
  display = (kr_display_t *)user;

  pos = (display->frames % display->framebufsize);

  memcpy (display->rgba + (pos * display->frame_size),
          buffer,
          display->frame_size);

  display->frames++;

	return 0;
}

void kr_display (kr_display_t *display) {

  int32_t ret;
  struct pollfd pollfds[4];

	while (1) {
	
	  if (destroy == 1) {
		  printf ("Got signal!\n");
	    break;
	  }
	  
    pollfds[0].fd = display->krad_wayland->display_fd;
    pollfds[0].events = POLLIN;

    //pollfds[1].fd = sd;
    //pollfds[1].events = POLLIN;

    //ret = poll (pollfds, 2, 1);
    ret = poll (pollfds, 1, -1);

	  if (ret < 0) {
	    break;
	  }

    if (pollfds[0].revents == POLLIN) { 
      krad_wayland_iterate (display->krad_wayland);
    }
	}

  printf ("Frames In: %"PRIu64" Frames Displayed: %"PRIu64"\n",
          display->frames, display->dframes);

}

int main (int argc, char *argv[]) {

  kr_display_t *display;
  
  printf ("Options: station width height\n");

  display = calloc (1, sizeof (kr_display_t));

  display->width = 960;
  display->height = 540;
	
	if (argc > 1) {
	  display->station = argv[1];
	} else {
    fprintf (stderr, "Need station name\n");
    exit (1);
  }

	if (argc > 3) {
    display->width = atoi (argv[2]);
    display->height = atoi (argv[3]);
	}

	signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);	

	display->client = kr_client_create ("krad videoport client");

	if (display->client == NULL) {
		fprintf (stderr, "Could not create KR client.\n");
		return 1;
	}	

  kr_connect (display->client, display->station);
  
  if (!kr_connected (display->client)) {
		fprintf (stderr, "Could not connect to %s krad radio daemon.\n",
             display->station);
	  kr_client_destroy (&display->client);
	  return 1;
  }
  
  if (kr_compositor_get_info_wait (display->client, &display->width, &display->height, NULL, NULL) != 1) {
    fprintf (stderr, "Could not get compositor info!\n");
	  kr_client_destroy (&display->client);
	  return 1;
  }

	display->videoport = kr_videoport_create (display->client, OUTPUT);

	if (display->videoport == NULL) {
		fprintf (stderr, "Could not make videoport.\n");
	  kr_client_destroy (&display->client);
	  return 1;
	} else {
		printf ("Working!\n");
	}

  kr_display_alloc_framebuf (display);

	display->krad_wayland = krad_wayland_create ();

	//display->krad_wayland->render_test_pattern = 1;

	krad_wayland_prepare_window (display->krad_wayland, display->width, display->height, &display->buffer);

  krad_wayland_set_frame_callback (display->krad_wayland, display_cb, display);

  krad_wayland_prepare_window (display->krad_wayland,
                 display->width,
                 display->height,
                 &display->buffer);

  printk("Wayland display prepared");

  krad_wayland_open_window (display->krad_wayland);
	
  kr_videoport_set_callback (display->videoport, new_frame, display);
	
  kr_videoport_activate (display->videoport);

  kr_display (display);

	kr_videoport_deactivate (display->videoport);
	
	kr_videoport_destroy (display->videoport);

	kr_client_destroy (&display->client);

  krad_wayland_close_window (display->krad_wayland);

	krad_wayland_destroy (display->krad_wayland);

  kr_display_free_framebuf (display);

  free (display);

	return 0;	
}
