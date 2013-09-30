#include <cairo.h>
#include "kr_client.h"
#include <ccv.h>
typedef struct kr_snapshot {
  uint32_t width;
  uint32_t height;
  uint8_t *rgba;
  kr_videoport_t *videoport_in;
  kr_client_t *client_in;
  ccv_comp_t dbox;
  ccv_rect_t bbox;
  ccv_tld_t *tld;
  int got_frame;
} kr_snapshot;

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}


int videoport_process_out(void *buffer, void *user) {
  cairo_surface_t *src; /* The video image from camera */
  cairo_surface_t *dst; /* The video image we are sending to kr */
  cairo_t *cr;          /* The 'paintbrush' or drawing context */
  kr_snapshot *snapshot = (kr_snapshot *)user;

  /* Setup surfaces */
  src = cairo_image_surface_create_for_data((uint8_t *)snapshot->rgba,
   CAIRO_FORMAT_ARGB32, snapshot->width, snapshot->height, snapshot->width * 4);

  dst = cairo_image_surface_create_for_data((uint8_t *)buffer,
   CAIRO_FORMAT_ARGB32, snapshot->width, snapshot->height, snapshot->width * 4);

  cr = cairo_create(dst);

  int target_rect_x = snapshot->dbox.rect.x;
  int target_rect_y = snapshot->dbox.rect.y;
  int target_rect_width = snapshot->bbox.width;
  int target_rect_height = snapshot->bbox.height;

  int src_x;
  int src_y;
  int dst_x;
  int dst_y;

  dst_x = 0;
  dst_y = 0;

  src_x = dst_x - target_rect_x;
  src_y = dst_y - target_rect_y;

  cairo_set_source_surface(cr, src, 0, 0);
  cairo_paint(cr);

  cairo_set_source_surface(cr, src, src_x, src_y);
  cairo_rectangle(cr, dst_x, dst_y, target_rect_width, target_rect_height);
  cairo_fill_preserve(cr);

  cairo_set_source_rgba(cr, 0.2, 0.6, 0.5, 0.5);
  cairo_fill(cr);

  dst_x = snapshot->dbox.rect.x;
  dst_y = snapshot->dbox.rect.y;

  cairo_rectangle(cr, dst_x, dst_y, target_rect_width, target_rect_height);
  cairo_fill(cr);

  /* Teardown */
  cairo_destroy (cr);
  cairo_surface_destroy(src);
  cairo_surface_destroy(dst);
  return 0;
}

int videoport_process_in (void *buffer, void *user) {
  kr_snapshot *snapshot;
  static int first_call = 1;
  static ccv_dense_matrix_t *x = 0;
  static ccv_dense_matrix_t *y = 0;

  snapshot = (kr_snapshot *)user;
  if (snapshot->got_frame == 0) {
    memcpy(snapshot->rgba, buffer, snapshot->width * snapshot->height * 4);
    if (first_call == 1) {
      first_call = 0;
      ccv_read(snapshot->rgba, &x, CCV_IO_ARGB_RAW | CCV_IO_GRAY, 
       snapshot->height, snapshot->width, snapshot->width * 4);
      snapshot->tld = ccv_tld_new(x, snapshot->bbox, ccv_tld_default_params);
    } else {
      ccv_read(snapshot->rgba, &y, CCV_IO_ARGB_RAW | CCV_IO_GRAY, snapshot->height, snapshot->width, snapshot->width * 4);
      ccv_tld_info_t info;
      snapshot->dbox = ccv_tld_track_object(snapshot->tld, x, y, &info);
      x = y;
      y = 0;
    }
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
  if (argc != 7) {
    fprintf (stderr, "args: station_in station_out x, y, w, h.\n");
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
  snapshot->bbox = ccv_rect(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
  snapshot->rgba = malloc(snapshot->width * snapshot->height * 4);

  kr_videoport_set_callback (videoport_in, videoport_process_in, snapshot);
  kr_videoport_set_callback (videoport_out, videoport_process_out, snapshot);

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);
  ccv_enable_default_cache();

  kr_videoport_activate (videoport_in);
  kr_videoport_activate (videoport_out);

  while(1) {
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
