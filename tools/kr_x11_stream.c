#include "kr_x11_stream.h"
#include "gen/kr_x11_stream_config.c"
#include "krad_debug.c"

typedef struct kr_x11_stream kr_x11_stream;
typedef struct kr_x11_stream_params kr_x11_stream_params;

struct kr_x11_stream {
  kr_x11_stream_params *params;
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

int kr_x11_stream_destroy(kr_x11_stream **x11s) {

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

kr_x11_stream *kr_x11_stream_create(kr_x11_stream_params *params) {

  kr_x11_stream *x11s;

  x11s = calloc (1, sizeof(kr_x11_stream));

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
                                           x11s->params->bitrate);


  //krad_vpx_encoder_set_kf_max_dist (x11s->vpx_enc, 300);


  kr_mkv_add_video_track (x11s->mkv, VP8,
                          x11s->params->fps_num,
                          x11s->params->fps_den,
                          x11s->params->width,
                          x11s->params->height);

  x11s->frame_ring = krad_ringbuffer_create (90 * sizeof(krad_frame_t *));

  x11s->framepool = krad_framepool_create (x11s->params->width,
                                           x11s->params->height,
                                           1);

  return x11s;
}

void kr_x11_stream_run(kr_x11_stream *x11s) {

  krad_frame_t *frame;
  kr_image image;
  kr_medium_t *vmedium;
  kr_codeme_t *vcodeme;
  struct SwsContext *converter;
  int sws_algo;
  uint8_t *screen_image;
  int32_t ret;

  signal (SIGINT, term_handler);
  signal (SIGTERM, term_handler);

  screen_image = NULL;
  converter = NULL;
  sws_algo = SWS_BILINEAR;

  vmedium = kr_medium_kludge_create ();
  vcodeme = kr_codeme_kludge_create ();

  kr_x11_enable_capture (x11s->x11, x11s->params->window_id);

  x11s->ticker = krad_ticker_create (x11s->params->fps_num,
                                     x11s->params->fps_den);

  x11s->timer = kr_timer_create();

  krad_ticker_start (x11s->ticker);

  while (!destruct) {

    frame = NULL;

    if (!kr_x11_capture_getptr (x11s->x11, &screen_image)) {
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
    frame->yuv_pixels[0] = screen_image;
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

    image.w = x11s->params->width;
    image.h = x11s->params->height;
    image.pps[0] = vmedium->v.pps[0];
    image.pps[1] = vmedium->v.pps[1];
    image.pps[2] = vmedium->v.pps[2];
    image.pps[3] = vmedium->v.pps[3];
    image.px = vmedium->v.ppx[0];
    image.ppx[0] = vmedium->v.ppx[0];
    image.ppx[1] = vmedium->v.ppx[1];
    image.ppx[2] = vmedium->v.ppx[2];
    image.ppx[3] = vmedium->v.ppx[3];
    image.tc = vmedium->v.tc;
    image.fmt = PIX_FMT_YUV420P;

    ret = kr_vpx_encode(x11s->vpx_enc, vcodeme, &image);
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

void kr_x11_stream_activate(kr_x11_stream_params *params) {
  kr_x11_stream *x11stream;
  x11stream = kr_x11_stream_create(params);
  kr_x11_stream_run(x11stream);
  kr_x11_stream_destroy(&x11stream);
}

int main (int argc, char *argv[]) {
  int ret;
  kr_x11_stream_params params;
  krad_debug_init("v4l2_stream");
  memset(&params, 0, sizeof(kr_x11_stream_params));
  ret = handle_config(&params, argv[1]);
  if (ret != 0) {
    printf("Config file %s error.\n", argv[1]);
    exit(1);
  }
  printf("X11 streaming at %ux%u %u fps (max)\n", params.width, params.height,
   params.fps_num/params.fps_den);
  printf("To: %s:%u%s\n", params.host, params.port, params.mount);
  printf("VP8 Bitrate: %uk\n", params.bitrate);
  kr_x11_stream_activate(&params);
  return 0;
}
