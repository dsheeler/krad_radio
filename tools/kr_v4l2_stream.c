#include "kr_v4l2_stream.h"

#include "krad_debug.c"
#include "gen/kr_v4l2_stream_config.c"

typedef struct kr_v4l2s kr_v4l2s;

struct kr_v4l2s {
  kr_v4l2s_params *params;
  kr_v4l2 *v4l2;
  kr_timer *timer;
  krad_vpx_encoder_t *vpx_enc;
  kr_mkv_t *mkv;
  uint64_t frames;
};

static int destruct = 0;

static void term_handler(int sig) {
  destruct = 1;
}

int kr_v4l2s_destroy(kr_v4l2s **v4l2s) {
  if ((v4l2s == NULL) || (*v4l2s == NULL)) {
    return -1;
  }
  if ((*v4l2s)->v4l2 != NULL) {
    kr_v4l2_capture((*v4l2s)->v4l2, 0);
    kr_v4l2_destroy((*v4l2s)->v4l2);
    (*v4l2s)->v4l2 = NULL;
  }
  krad_vpx_encoder_destroy(&(*v4l2s)->vpx_enc);
  kr_mkv_destroy(&(*v4l2s)->mkv);
  free(*v4l2s);
  *v4l2s = NULL;
  return 0;
}

kr_v4l2s *kr_v4l2s_create(kr_v4l2s_params *params) {
  kr_v4l2s *v4l2s;
  kr_v4l2_setup setup;
  kr_v4l2_mode mode;
  v4l2s = calloc (1, sizeof(kr_v4l2s));
  v4l2s->params = params;
  char file[512];
  snprintf(file, sizeof(file), "%s/%s_%"PRIu64".webm", getenv("HOME"), "v4l2s",
   krad_unixtime());
  /*v4l2s->mkv = kr_mkv_create_file (file);*/
  v4l2s->mkv = kr_mkv_create_stream(v4l2s->params->host, v4l2s->params->port,
   v4l2s->params->mount, v4l2s->params->password);
  if (v4l2s->mkv == NULL) {
    fprintf(stderr, "failed to stream :/ \n");
    exit(1);
  }
  v4l2s->vpx_enc = krad_vpx_encoder_create(v4l2s->params->width,
   v4l2s->params->height, 1000, 1, v4l2s->params->video_bitrate);
  kr_mkv_add_video_track(v4l2s->mkv, VP8, v4l2s->params->fps_numerator,
   v4l2s->params->fps_denominator, v4l2s->params->width, v4l2s->params->height);
  /*v4l2_setup.dev = v4l2s->params->device;*/
  /*FIXME this is a lie */
  setup.dev = 0;
  setup.priority = 0;
  mode.width = v4l2s->params->width;
  mode.height = v4l2s->params->height;
  mode.num = v4l2s->params->fps_numerator;
  mode.den = v4l2s->params->fps_denominator;
  mode.format = 0;
  v4l2s->v4l2 = kr_v4l2_create(&setup);
  kr_v4l2_mode_set(v4l2s->v4l2, &mode);
  return v4l2s;
}

void kr_v4l2s_run(kr_v4l2s *v4l2s) {
  kr_image cam_image;
  kr_image scaled_image;
  uint8_t *pixels;
  kr_convert conv;
  kr_codeme_t *vcodeme;
  int32_t ret;
  kr_image_convert_init(&conv);
  vcodeme = kr_codeme_kludge_create();
  pixels = malloc(v4l2s->params->width * v4l2s->params->height * 2);
  scaled_image.w = v4l2s->params->width;
  scaled_image.h = v4l2s->params->height;
  scaled_image.px = pixels;
  scaled_image.ppx[0] = pixels;
  scaled_image.ppx[1] = pixels
   + (v4l2s->params->width * v4l2s->params->height);
  scaled_image.ppx[2] = pixels
   + (v4l2s->params->width * v4l2s->params->height)
   + ((v4l2s->params->width / 2) * (v4l2s->params->height / 2));
  scaled_image.ppx[3] = 0;
  scaled_image.pps[0] = v4l2s->params->width;
  scaled_image.pps[1] = v4l2s->params->width / 2;
  scaled_image.pps[2] = v4l2s->params->width / 2;
  scaled_image.pps[3] = 0;
  scaled_image.fmt = PIX_FMT_YUV420P;
  kr_v4l2_capture(v4l2s->v4l2, 1);
  v4l2s->timer = kr_timer_create();
  krad_vpx_encoder_deadline_set(v4l2s->vpx_enc, 1);
  while (!destruct) {
    ret = kr_v4l2_poll(v4l2s->v4l2, 15);
    if (ret < 1) {
      continue;
    }
    ret = kr_v4l2_read(v4l2s->v4l2, &cam_image);
    cam_image.tc = kr_timer_current_ms(v4l2s->timer);
    if (!kr_timer_started(v4l2s->timer)) {
      kr_timer_start(v4l2s->timer);
    }
    kr_image_convert(&conv, &scaled_image, &cam_image);
    kr_image_unref(&cam_image);
    ret = kr_vpx_encode(v4l2s->vpx_enc, vcodeme, &scaled_image);
    if (ret == 1) {
      kr_mkv_add_video_tc(v4l2s->mkv, 1, vcodeme->data, vcodeme->sz,
       vcodeme->key, vcodeme->tc);
    }
    printf("\rKrad V4L2 Stream Frame# %12"PRIu64"", v4l2s->frames++);
    fflush(stdout);
  }
  kr_codeme_kludge_destroy(&vcodeme);
  kr_image_convert_clear(&conv);
  free(pixels);
  kr_timer_destroy(v4l2s->timer);
}

int main (int argc, char *argv[]) {
  int ret;
  kr_v4l2s *v4l2s;
  kr_v4l2s_params params;
  krad_debug_init("v4l2_stream");
  memset(&params, 0, sizeof(kr_v4l2s_params));
  ret = handle_config(&params, argv[1]);
  if (ret != 0) {
    printf("Config file %s error.\n", argv[1]);
    exit(1);
  }
  printf("Streaming with: %s at %ux%u %u fps (max)\n",
         params.device, params.width, params.height,
         params.fps_numerator/params.fps_denominator);
  printf("To: %s:%u%s\n", params.host, params.port, params.mount);
  printf("VP8 Bitrate: %uk\n", params.video_bitrate);
  v4l2s = kr_v4l2s_create(&params);
  if (v4l2s != NULL) {
    signal(SIGINT, term_handler);
    signal(SIGTERM, term_handler);
    kr_v4l2s_run(v4l2s);
    kr_v4l2s_destroy(&v4l2s);
  }
  krad_debug_shutdown();
  return 0;
}
