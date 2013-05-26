#include <stdio.h>
#include <unistd.h>
#define KRAD_NO_TURBOJPEG
#include <krad_muxponder.h>
#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>
#include <krad_vpx.h>
#include <krad_vorbis.h>
#include <krad_v4l2.h>
#include <krad_ring.h>
#include <krad_framepool.h>
#include <krad_timer.h>

#include <libswscale/swscale.h>

#include "krad_debug.c"

typedef struct kr_v4l2s_St kr_v4l2s_t;
typedef struct kr_v4l2s_params_St kr_v4l2s_params_t;

struct kr_v4l2s_params_St {
  uint32_t width;
  uint32_t height;
  uint32_t fps_numerator;
  uint32_t fps_denominator;
  uint32_t video_bitrate;
  char *host;
  int32_t port;
  char *mount;
  char *password;
  char *device;
};

struct kr_v4l2s_St {
  kr_v4l2s_params_t *params;
  krad_v4l2_t *v4l2;
  krad_timer_t *timer;
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

int kr_v4l2s_destroy (kr_v4l2s_t **v4l2s) {

  if ((v4l2s == NULL) || (*v4l2s == NULL)) {
    return -1;
  }

  if ((*v4l2s)->v4l2 != NULL) {
    krad_v4l2_stop_capturing ((*v4l2s)->v4l2);
    krad_v4l2_close ((*v4l2s)->v4l2);
    krad_v4l2_destroy ((*v4l2s)->v4l2);
    (*v4l2s)->v4l2 = NULL;
  }

  krad_vpx_encoder_destroy (&(*v4l2s)->vpx_enc);
  kr_mkv_destroy (&(*v4l2s)->mkv);
  krad_ringbuffer_free ((*v4l2s)->frame_ring);
  krad_framepool_destroy (&(*v4l2s)->framepool);
  free (*v4l2s);
  *v4l2s = NULL;
  return 0;
}

kr_v4l2s_t *kr_v4l2s_create (kr_v4l2s_params_t *params) {

  kr_v4l2s_t *v4l2s;

  v4l2s = calloc (1, sizeof(kr_v4l2s_t));

  v4l2s->params = params;
 
  char file[512];
  
  snprintf (file, sizeof(file),
            "%s/%s_%"PRIu64".webm",
            getenv ("HOME"), "v4l2s", krad_unixtime ());
  
  //v4l2s->mkv = kr_mkv_create_file (file);

  v4l2s->mkv = kr_mkv_create_stream (v4l2s->params->host,
                                     v4l2s->params->port,
                                     v4l2s->params->mount,
                                     v4l2s->params->password);

  if (v4l2s->mkv == NULL) {
    fprintf (stderr, "failed to stream :/ \n");
    exit (1);
  }

  v4l2s->vpx_enc = krad_vpx_encoder_create (v4l2s->params->width,
                                            v4l2s->params->height,
                                            1000,
                                            1,
                                            v4l2s->params->video_bitrate);

  kr_mkv_add_video_track (v4l2s->mkv, VP8,
                          v4l2s->params->fps_numerator,
                          v4l2s->params->fps_denominator,
                          v4l2s->params->width,
                          v4l2s->params->height);

  v4l2s->frame_ring = krad_ringbuffer_create (90 * sizeof(krad_frame_t *));

  v4l2s->framepool = krad_framepool_create (v4l2s->params->width,
                                            v4l2s->params->height,
                                            10);
  v4l2s->v4l2 = krad_v4l2_create ();

  krad_v4l2_open (v4l2s->v4l2, v4l2s->params->device,
                  v4l2s->params->width, v4l2s->params->height,
                  v4l2s->params->fps_numerator);
  return v4l2s;
}

void kr_v4l2s_run (kr_v4l2s_t *v4l2s) {

  krad_frame_t *frame;
  uint8_t *captured_frame;
  kr_medium_t *vmedium;
  kr_codeme_t *vcodeme;
  struct SwsContext *converter;
  int sws_algo;
  int32_t ret;

  signal (SIGINT, term_handler);
  signal (SIGTERM, term_handler);    

  converter = NULL;
  sws_algo = SWS_BILINEAR;

  vmedium = kr_medium_kludge_create ();
  vcodeme = kr_codeme_kludge_create ();

  krad_v4l2_start_capturing (v4l2s->v4l2);
  v4l2s->timer = krad_timer_create ();

  while (!destruct) {

    captured_frame = NULL;
    frame = NULL;

    captured_frame = (uint8_t *)krad_v4l2_read (v4l2s->v4l2);    

    if (captured_frame == NULL) {
      continue;
    }
    
    vmedium->v.tc = krad_timer_current_ms (v4l2s->timer);
    if (!krad_timer_started (v4l2s->timer)) {
      krad_timer_start (v4l2s->timer);
    }
    
    frame = krad_framepool_getframe (v4l2s->framepool);
    if (frame == NULL) {
      continue;
    }

    frame->format = PIX_FMT_YUYV422;
    frame->yuv_pixels[0] = captured_frame;
    frame->yuv_pixels[1] = NULL;
    frame->yuv_pixels[2] = NULL;
    frame->yuv_strides[0] = v4l2s->params->width + (v4l2s->params->width/2) * 2;
    frame->yuv_strides[1] = 0;
    frame->yuv_strides[2] = 0;
    frame->yuv_strides[3] = 0;
    
    converter = sws_getCachedContext ( converter,
                                       v4l2s->params->width,
                                       v4l2s->params->height,
                                       frame->format,
                                       v4l2s->params->width,
                                       v4l2s->params->height,
                                       PIX_FMT_YUV420P, 
                                       sws_algo,
                                       NULL, NULL, NULL);

    if (converter == NULL) {
      failfast ("Krad v4l2s: could not sws_getCachedContext");
    }

    vmedium->v.pps[0] = v4l2s->params->width;
    vmedium->v.pps[1] = v4l2s->params->width/2;  
    vmedium->v.pps[2] = v4l2s->params->width/2;
    vmedium->v.ppx[0] = vmedium->data;
    vmedium->v.ppx[1] = vmedium->data +
                        v4l2s->params->width * (v4l2s->params->height);
    vmedium->v.ppx[2] = vmedium->data + v4l2s->params->width *
                        (v4l2s->params->height) +
                        ((v4l2s->params->width * (v4l2s->params->height)) /4);

    sws_scale (converter,
              (const uint8_t * const*)frame->yuv_pixels,
              frame->yuv_strides,
              0,
              v4l2s->params->height,
              vmedium->v.ppx,
              vmedium->v.pps);

    krad_framepool_unref_frame (frame);
    krad_v4l2_frame_done (v4l2s->v4l2);

    ret = kr_vpx_encode (v4l2s->vpx_enc, vcodeme, vmedium);
    if (ret == 1) {
      kr_mkv_add_video_tc (v4l2s->mkv, 1,
                           vcodeme->data, vcodeme->sz,
                           vcodeme->key, vcodeme->tc);      
    }
    
    printf ("\rKrad V4L2 Stream Frame# %12"PRIu64"",
            v4l2s->frames++);
    fflush (stdout);
  }

  kr_medium_kludge_destroy (&vmedium);
  kr_codeme_kludge_destroy (&vcodeme);

  if (converter != NULL) {
    sws_freeContext (converter);
    converter = NULL;
  }

  krad_timer_destroy (v4l2s->timer);
}

void kr_v4l2s (kr_v4l2s_params_t *params) {

  kr_v4l2s_t *v4l2s;
  
  v4l2s = kr_v4l2s_create (params);
  kr_v4l2s_run (v4l2s);
  kr_v4l2s_destroy (&v4l2s);
}
  
int main (int argc, char *argv[]) {

  kr_v4l2s_params_t params;
  char mount[256];
  krad_debug_init ("v4l2_stream");

  memset (&params, 0, sizeof(kr_v4l2s_params_t));

  params.width = 640;
  params.height = 360;
  params.fps_numerator = 30;
  params.fps_denominator = 1;
  params.device = "/dev/video0";
  params.video_bitrate = 450;
  params.host = "europa.kradradio.com";
  params.port = 8008;

  snprintf (mount, sizeof(mount),
            "/kr_v4l2s_%"PRIu64".webm",
            krad_unixtime ());

  params.mount = mount;
  params.password = "firefox";

  printf ("Streaming with: %s at %ux%u %u fps (max)\n",
          params.device, params.width, params.height,
          params.fps_numerator/params.fps_denominator);
  printf ("To: %s:%u%s\n", params.host, params.port, params.mount);
  printf ("VP8 Bitrate: %uk\n", params.video_bitrate);

  kr_v4l2s (&params);

  krad_debug_shutdown ();

  return 0;
}
