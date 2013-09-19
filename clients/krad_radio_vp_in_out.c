#include "kr_client.h"

#define FILENAME "/tmp/detect"

#define GREY  0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255 * 1.0
#define BLUE 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.255 / 0.255   * 1.0
#define ORANGE  0.255 / 0.255 * 1.0, 0.080 / 0.255 * 1.0, 0.0

typedef struct kr_snapshot {
  uint32_t width;
  uint32_t height;
  uint8_t *rgba;
	kr_videoport_t *videoport_in;
	kr_client_t *client_in;
  int got_frame;
} kr_snapshot;

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

int videoport_process_out (void *buffer, void *user) {

  kr_snapshot *snapshot;
  
  snapshot = (kr_snapshot *)user;

  if (snapshot->got_frame == 1) {

    memcpy(buffer, snapshot->rgba, snapshot->width
     * snapshot->height * 4);
    snapshot->got_frame = 0;
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
  ret = 0;

	if (argc != 3) {
    fprintf (stderr, "Takes two station arguments.\n");
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
  
  snapshot->rgba = malloc(snapshot->width * snapshot->height * 4);
 
	kr_videoport_set_callback (videoport_in, videoport_process_in, snapshot);
	kr_videoport_set_callback (videoport_out, videoport_process_out, snapshot);

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);	
	
	kr_videoport_activate (videoport_in);
	kr_videoport_activate (videoport_out);

	for (i = 0; i < 3000; i++) {
	  usleep (3000000);
    
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
