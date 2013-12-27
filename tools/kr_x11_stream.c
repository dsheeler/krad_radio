#include "kr_x11_stream.h"
#include "gen/kr_x11_stream_config.c"
#include "krad_debug.c"

typedef struct kr_x11_stream kr_x11_stream;
typedef struct kr_x11_stream_params kr_x11_stream_params;

struct kr_x11_stream {
  kr_x11_stream_params *params;
  kr_x11 *x11;
  kr_timer *timer;
  krad_vpx_encoder_t *vpx_enc;
  kr_codec_hdr_t header;
  kr_mkv_t *mkv;
  uint64_t frames;
};

static int destruct = 0;

static void term_handler (int sig) {
  destruct = 1;
}

int kr_x11_stream_destroy(kr_x11_stream **stream) {
  if ((stream == NULL) || (*stream == NULL)) {
    return -1;
  }
  if ((*stream)->x11 != NULL) {
    kr_x11_disable_capture((*stream)->x11);
    kr_x11_destroy((*stream)->x11);
    (*stream)->x11 = NULL;
  }
  krad_vpx_encoder_destroy(&(*stream)->vpx_enc);
  kr_mkv_destroy(&(*stream)->mkv);
  free(*stream);
  *stream = NULL;
  return 0;
}

kr_x11_stream *kr_x11_stream_create(kr_x11_stream_params *params) {
  kr_x11_stream *x11s;
  x11s = calloc(1, sizeof(kr_x11_stream));
  x11s->params = params;
  x11s->x11 = kr_x11_create();
  if ((x11s->x11->screen_width == 0) || (x11s->x11->screen_height == 0)) {
    printf("Unable to get X11 screen resolution probably compiled w/o x11\n");
    exit(1);
  } else {
    printf("X11 Resolution: %dx%d\n",
           x11s->x11->screen_width, x11s->x11->screen_height);
  }
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
  kr_mkv_add_video_track (x11s->mkv, VP8,
                          x11s->params->fps_num,
                          x11s->params->fps_den,
                          x11s->params->width,
                          x11s->params->height);
  return x11s;
}

void kr_x11_stream_run(kr_x11_stream *x11s) {
  kr_codeme_t *vcodeme;
  krad_ticker_t *ticker;
  int32_t ret;
  kr_image x11_image;
  kr_image scaled_image;
  uint8_t *pixels;
  kr_convert conv;
  kr_image_convert_init(&conv);
  pixels = malloc(x11s->params->width * x11s->params->height * 4);
  memset(&scaled_image, 0, sizeof(kr_image));
  memset(&x11_image, 0, sizeof(kr_image));
  scaled_image.w = x11s->params->width;
  scaled_image.h = x11s->params->height;
  scaled_image.px = pixels;
  scaled_image.ppx[0] = pixels;
  scaled_image.ppx[1] = pixels
   + (x11s->params->width * x11s->params->height);
  scaled_image.ppx[2] = pixels
   + (x11s->params->width * x11s->params->height)
   + ((x11s->params->width / 2) * (x11s->params->height / 2));
  scaled_image.ppx[3] = 0;
  scaled_image.pps[0] = x11s->params->width;
  scaled_image.pps[1] = x11s->params->width / 2;
  scaled_image.pps[2] = x11s->params->width / 2;
  scaled_image.pps[3] = 0;
  scaled_image.fmt = PIX_FMT_YUV420P;
  signal(SIGINT, term_handler);
  signal(SIGTERM, term_handler);
  vcodeme = kr_codeme_kludge_create();
  kr_x11_enable_capture(x11s->x11, x11s->params->window_id);
  x11s->timer = kr_timer_create();
  ticker = krad_ticker_create(x11s->params->fps_num, x11s->params->fps_den);
  krad_ticker_start(ticker);
  while (!destruct) {
    if (!kr_x11_capture(x11s->x11, &x11_image)) {
      continue;
    }
    if (!kr_timer_started(x11s->timer)) {
      kr_timer_start(x11s->timer);
    }
    scaled_image.tc = kr_timer_current_ms(x11s->timer);
    kr_image_convert(&conv, &scaled_image, &x11_image);
    ret = kr_vpx_encode(x11s->vpx_enc, vcodeme, &scaled_image);
    if (ret == 1) {
      kr_mkv_add_video_tc (x11s->mkv, 1,
                           vcodeme->data, vcodeme->sz,
                           vcodeme->key, vcodeme->tc);
    }
    printf ("\rKrad X11 Stream Frame# %12"PRIu64"",
            x11s->frames++);
    fflush (stdout);
  }
  kr_codeme_kludge_destroy (&vcodeme);
  free(pixels);
  kr_image_convert_clear(&conv);
  kr_timer_destroy(x11s->timer);
  krad_ticker_destroy(ticker);
}

void kr_x11_stream_activate(kr_x11_stream_params *params) {
  kr_x11_stream *x11stream;
  x11stream = kr_x11_stream_create(params);
  kr_x11_stream_run(x11stream);
  kr_x11_stream_destroy(&x11stream);
}

int main(int argc, char *argv[]) {
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
