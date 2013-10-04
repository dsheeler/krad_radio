#include <stdio.h>
#include <unistd.h>
#define KRAD_NO_TURBOJPEG
#include <krad_muxponder.h>
#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>
#include <krad_vpx.h>
#include <krad_vorbis.h>
#include <krad_x11.h>
#include <krad_ring.h>
#include <krad_framepool.h>
#include <krad_timer.h>

#include <libswscale/swscale.h>

#include "krad_debug.c"

typedef struct kr_x11s_St kr_x11s_t;
typedef struct kr_x11s_params_St kr_x11s_params_t;

struct kr_x11s_params_St {
  uint32_t width;
  uint32_t height;
  uint32_t fps_numerator;
  uint32_t fps_denominator;
  uint32_t video_bitrate;
  char *host;
  int32_t port;
  char *mount;
  char *password;
  uint32_t window_id;
};

struct kr_x11s_St {
  kr_x11s_params_t *params;
  kr_x11 *x11;
  kr_timer *timer;
  krad_ticker_t *ticker;
  krad_ringbuffer_t *frame_ring;
  krad_framepool_t *framepool;
  krad_vpx_encoder_t *vpx_enc;
  kr_codec_hdr_t header;
  kr_mkv_t *mkv;
  uint64_t frames;
};

static int destruct = 0;

static void term_handler (int sig) {
  destruct = 1;
}

int kr_x11s_destroy (kr_x11s_t **x11s) {

  if ((x11s == NULL) || (*x11s == NULL)) {
    return -1;
  }

  if ((*x11s)->x11 != NULL) {
    kr_x11_disable_capture ((*x11s)->x11);
    kr_x11_destroy ((*x11s)->x11);
    (*x11s)->x11 = NULL;
  }

  krad_vpx_encoder_destroy (&(*x11s)->vpx_enc);
  kr_mkv_destroy (&(*x11s)->mkv);
  krad_ringbuffer_free ((*x11s)->frame_ring);
  krad_framepool_destroy (&(*x11s)->framepool);
  free (*x11s);
  *x11s = NULL;
  return 0;
}

kr_x11s_t *kr_x11s_create (kr_x11s_params_t *params) {

  kr_x11s_t *x11s;

  x11s = calloc (1, sizeof(kr_x11s_t));

  x11s->params = params;

  x11s->x11 = kr_x11_create ();

  if ((x11s->x11->screen_width == 0) || (x11s->x11->screen_height == 0)) {
    printf ("Unable to get X11 screen resolution probably compiled w/o x11\n");
    exit (1);
  } else {
    printf ("X11 Resolution: %dx%d\n",
            x11s->x11->screen_width, x11s->x11->screen_height);
  }

  char file[512];

  snprintf (file, sizeof(file),
            "%s/%s_%"PRIu64".webm",
            getenv ("HOME"), "x11s", krad_unixtime ());

  //x11s->mkv = kr_mkv_create_file (file);

  x11s->mkv = kr_mkv_create_stream (x11s->params->host,
                                    x11s->params->port,
                                    x11s->params->mount,
                                    x11s->params->password);

  if (x11s->mkv == NULL) {
    fprintf (stderr, "failed to stream :/ \n");
    exit (1);
  }

  x11s->vpx_enc = krad_vpx_encoder_create (x11s->params->width,
                                           x11s->params->height,
                                           1000,
                                           1,
                                           x11s->params->video_bitrate);


  //krad_vpx_encoder_set_kf_max_dist (x11s->vpx_enc, 300);


  kr_mkv_add_video_track (x11s->mkv, VP8,
                          x11s->params->fps_numerator,
                          x11s->params->fps_denominator,
                          x11s->params->width,
                          x11s->params->height);

  x11s->frame_ring = krad_ringbuffer_create (90 * sizeof(krad_frame_t *));

  x11s->framepool = krad_framepool_create (x11s->params->width,
                                           x11s->params->height,
                                           1);

  return x11s;
}

void kr_x11s_run (kr_x11s_t *x11s) {

  krad_frame_t *frame;
  kr_medium_t *vmedium;
  kr_codeme_t *vcodeme;
  struct SwsContext *converter;
  int sws_algo;
  uint8_t *image;
  int32_t ret;

  signal (SIGINT, term_handler);
  signal (SIGTERM, term_handler);

  image = NULL;
  converter = NULL;
  sws_algo = SWS_BILINEAR;

  vmedium = kr_medium_kludge_create ();
  vcodeme = kr_codeme_kludge_create ();

  kr_x11_enable_capture (x11s->x11, x11s->params->window_id);

  x11s->ticker = krad_ticker_create (x11s->params->fps_numerator,
                                     x11s->params->fps_denominator);

  x11s->timer = kr_timer_create();

  krad_ticker_start (x11s->ticker);

  while (!destruct) {

    frame = NULL;

    if (!kr_x11_capture_getptr (x11s->x11, &image)) {
      continue;
    }

    vmedium->v.tc = kr_timer_current_ms(x11s->timer);
    if (!kr_timer_started(x11s->timer)) {
      kr_timer_start(x11s->timer);
    }

    frame = krad_framepool_getframe (x11s->framepool);
    if (frame == NULL) {
      continue;
    }

    frame->format = PIX_FMT_RGB32;
    frame->yuv_pixels[0] = image;
    frame->yuv_pixels[1] = NULL;
    frame->yuv_pixels[2] = NULL;
    frame->yuv_strides[0] = x11s->x11->stride;
    frame->yuv_strides[1] = 0;
    frame->yuv_strides[2] = 0;
    frame->yuv_strides[3] = 0;

    converter = sws_getCachedContext ( converter,
                                       x11s->x11->width,
                                       x11s->x11->height,
                                       frame->format,
                                       x11s->params->width,
                                       x11s->params->height,
                                       PIX_FMT_YUV420P,
                                       sws_algo,
                                       NULL, NULL, NULL);

    if (converter == NULL) {
      failfast ("Krad x11s: could not sws_getCachedContext");
    }

    vmedium->v.pps[0] = x11s->params->width;
    vmedium->v.pps[1] = x11s->params->width/2;
    vmedium->v.pps[2] = x11s->params->width/2;
    vmedium->v.ppx[0] = vmedium->data;
    vmedium->v.ppx[1] = vmedium->data +
                        x11s->params->width * (x11s->params->height);
    vmedium->v.ppx[2] = vmedium->data + x11s->params->width *
                        (x11s->params->height) +
                        ((x11s->params->width * (x11s->params->height)) /4);

    sws_scale (converter,
              (const uint8_t * const*)frame->yuv_pixels,
              frame->yuv_strides,
              0,
              x11s->x11->screen_height,
              vmedium->v.ppx,
              vmedium->v.pps);

    krad_framepool_unref_frame (frame);

    ret = kr_vpx_encode (x11s->vpx_enc, vcodeme, vmedium);
    if (ret == 1) {
      kr_mkv_add_video_tc (x11s->mkv, 1,
                           vcodeme->data, vcodeme->sz,
                           vcodeme->key, vcodeme->tc);
    }

    printf ("\rKrad X11 Stream Frame# %12"PRIu64"",
            x11s->frames++);
    fflush (stdout);

    krad_ticker_wait (x11s->ticker);
  }

  kr_medium_kludge_destroy (&vmedium);
  kr_codeme_kludge_destroy (&vcodeme);

  if (converter != NULL) {
    sws_freeContext (converter);
    converter = NULL;
  }

  krad_ticker_destroy (x11s->ticker);
  kr_timer_destroy(x11s->timer);
}

void kr_x11s (kr_x11s_params_t *params) {

  kr_x11s_t *x11s;

  x11s = kr_x11s_create (params);
  kr_x11s_run (x11s);
  kr_x11s_destroy (&x11s);
}

int main (int argc, char *argv[]) {

  kr_x11s_params_t params;
  char mount[256];
  krad_debug_init ("v4l2_stream");

  memset (&params, 0, sizeof(kr_x11s_params_t));

  //params.width = 1920;
  //params.height = 1080;
  params.width = 640;
  params.height = 360;
  params.fps_numerator = 30;
  params.fps_denominator = 1;
  //params.video_bitrate = 1450;
  params.video_bitrate = 450;
  params.host = "europa.kradradio.com";
  params.port = 8008;
  params.window_id = 0;

  //params.window_id = 0x2409f5c;

  snprintf (mount, sizeof(mount),
            "/kr_x11s_%"PRIu64".webm",
            krad_unixtime ());

  params.mount = mount;
  params.password = "firefox";

  printf ("Streaming with: %s at %ux%u %u fps (max)\n",
          "x11", params.width, params.height,
          params.fps_numerator/params.fps_denominator);
  printf ("To: %s:%u%s\n", params.host, params.port, params.mount);
  printf ("VP8 Bitrate: %uk\n", params.video_bitrate);

  kr_x11s (&params);

  krad_debug_shutdown ();

  return 0;
}
