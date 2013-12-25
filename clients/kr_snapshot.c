#include <cairo/cairo.h>
#include "kr_client.h"

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

void signal_recv(int sig) {
  destroy = 1;
}

int frame_cb(void *buffer, void *user) {
  kr_snapshot *snapshot;
  snapshot = (kr_snapshot *)user;
  if (snapshot->got_frame == 0) {
    memcpy(snapshot->rgba, buffer, snapshot->width * snapshot->height * 4);
    snapshot->got_frame = 1;
    close(snapshot->sd[1]);
  }
  return 0;
}

int kr_snapshot_take(char *station, char *filename) {
  int32_t ret;
  struct pollfd pollfds[1];
  kr_snapshot *snapshot;
  char newfilename[512];
  cairo_surface_t *surface;
  ret = -1;
  if (station == NULL) {
    fprintf(stderr, "No station name.\n");
    return -1;
  }
  if (filename == NULL) {
    snprintf (newfilename, sizeof(newfilename), "%s/%s_%"PRIu64".png",
     getenv ("HOME"), station, krad_unixtime());
    filename = newfilename;
  } else {
    if (strstr(filename, ".png") == NULL) {
      fprintf(stderr, "Its gotta be a .png file\n");
      return -1;
    }
  }
  snapshot = calloc(1, sizeof(kr_snapshot));
  signal(SIGINT, signal_recv);
  signal(SIGTERM, signal_recv);
  snapshot->client = kr_client_create("krad snapshot client");
  if (snapshot->client == NULL) {
    fprintf (stderr, "Could not create KR client.\n");
    return -1;
  }
  kr_connect(snapshot->client, station);
  if (!kr_connected(snapshot->client)) {
    fprintf(stderr, "Could not connect to %s krad radio daemon.\n",
     station);
    kr_client_destroy(&snapshot->client);
    return -1;
  }
  if (kr_compositor_get_info_wait(snapshot->client, &snapshot->width,
      &snapshot->height, NULL, NULL) != 1) {
    fprintf(stderr, "Could not get compositor info!\n");
    kr_client_destroy(&snapshot->client);
    return -1;
  }
  snapshot->rgba = malloc(snapshot->width * snapshot->height * 4);
  //FIXME
  snapshot->videoport = kr_videoport_create(snapshot->client, 0);
  if (snapshot->videoport == NULL) {
    fprintf (stderr, "Could not make videoport.\n");
    kr_client_destroy (&snapshot->client);
    return -1;
  }
  kr_videoport_set_callback(snapshot->videoport, frame_cb, snapshot);
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, snapshot->sd)) {
    fprintf(stderr, "Can't socketpair errno: %d\n", errno);
  } else {
    kr_videoport_activate(snapshot->videoport);
    pollfds[0].fd = snapshot->sd[0];
    pollfds[0].events = POLLIN;
    ret = poll(pollfds, 1, 666);
    if (ret != 1) {
      if (destroy == 1) {
        printf("Got signal!\n");
      } else {
        fprintf(stderr, "It didn't work!\n");
      }
    } else {
      if ((pollfds[0].revents) && (snapshot->got_frame == 1)) {
        surface = cairo_image_surface_create_for_data(snapshot->rgba,
         CAIRO_FORMAT_ARGB32, snapshot->width, snapshot->height,
         snapshot->width * 4);
        if (cairo_surface_status(surface) != 0) {
          fprintf(stderr, "Could not make cairo surface: %s :/\n",
           cairo_status_to_string(cairo_surface_status(surface)));
        } else {
          usleep(50000);
          ret = cairo_surface_write_to_png(surface, filename);
          if (ret == 0) {
            printf("Yay! We took: %s\n", filename);
          } else {
            fprintf(stderr, "Damn! Error %s writing %s\n",
             cairo_status_to_string(ret), filename);
            fprintf(stderr, "Surface Status: %s\n",
             cairo_status_to_string(cairo_surface_status(surface)));
          }
          cairo_surface_destroy(surface);
        }
      }
    }
    close(snapshot->sd[0]);
  }
  kr_videoport_deactivate(snapshot->videoport);
  kr_videoport_destroy(snapshot->videoport);
  kr_client_destroy(&snapshot->client);
  free(snapshot->rgba);
  free(snapshot);
  return ret;
}

int main(int argc, char *argv[]) {
  int ret;
  ret = 0;
  if ((argc < 2) || (argc > 3)) {
    fprintf(stderr, "Need station name\n");
    fprintf(stderr, "Options: station filename\n");
    exit(1);
  } else {
    if (argc == 2) {
      ret = kr_snapshot_take(argv[1], NULL);
    } else if (argc == 3) {
      ret = kr_snapshot_take(argv[1], argv[2]);
    }
    if (ret != 0) {
      fprintf(stderr, "Error taking snapshot, err num was %d\n", ret);
      exit(1);
    }
  }
  return 0;
}
