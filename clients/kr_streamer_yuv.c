#include <stdio.h>
#include <unistd.h>
#include <cairo/cairo.h>
#include <libswscale/swscale.h>

#include "kr_client.h"
#include "krad_udp.h"
#define KRAD_NO_TURBOJPEG
#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>
#include <krad_vpx.h>
#include <krad_vorbis.h>
#include <krad_ring.h>
#include <krad_framepool.h>
#include <krad_timer.h>
#include <krad_y4m.h>

typedef struct kr_streamer_St kr_streamer_t;
typedef struct kr_streamer_params_St kr_streamer_params_t;

struct kr_streamer_params_St {
  uint32_t width;
  uint32_t height;
  uint32_t fps_numerator;
  uint32_t fps_denominator;
  uint32_t video_bitrate;
  int32_t color_depth;
  char *station;
  char *file;
  char *host;
  int32_t port;
  char *mount;
  char *password;
};

struct kr_streamer_St {
  kr_streamer_params_t *params;
  uint32_t width;
  uint32_t height;
  uint32_t fps_numerator;
  uint32_t fps_denominator;
  uint32_t frame_size;
  kr_videoport_t *videoport;
  kr_client_t *client;
  krad_ringbuffer_t *frame_ring;
  krad_framepool_t *framepool;
  krad_timer_t *timer;
  krad_vpx_encoder_t *vpx_enc;
  kr_codec_hdr_t header;
  kr_mkv_t *mkv;
  kr_file_t *y4m_file;
  uint64_t frames;
  uint64_t eframes;
};

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

int new_frame (void *buffer, void *user) {

  kr_streamer_t *streamer;
  krad_frame_t *frame;

  streamer = (kr_streamer_t *)user;

  frame = krad_framepool_getframe (streamer->framepool);

  if (frame != NULL) {

    memcpy (frame->pixels,
            buffer,
            streamer->frame_size);

    krad_ringbuffer_write (streamer->frame_ring,
                          (char *)&frame,
                          sizeof(krad_frame_t *));
  }

  streamer->frames++;

  return 0;
}

int kr_streamer_destroy (kr_streamer_t **streamer) {

  if ((streamer == NULL) || (*streamer == NULL)) {
    return -1;
  }

  //krad_vpx_encoder_destroy (&(*streamer)->vpx_enc);
  //kr_mkv_destroy (&(*streamer)->mkv);
  kr_file_close(&(*streamer)->y4m_file);
  kr_videoport_deactivate ((*streamer)->videoport);
  kr_videoport_destroy ((*streamer)->videoport);
  kr_client_destroy (&(*streamer)->client);
  krad_ringbuffer_free ((*streamer)->frame_ring);
  krad_framepool_destroy (&(*streamer)->framepool);
  free (*streamer);
  *streamer = NULL;
  return 0;
}

kr_streamer_t *kr_streamer_create (kr_streamer_params_t *params) {

  kr_streamer_t *streamer;

  uint8_t y4m_header[256];
  int32_t y4m_header_len;
  ssize_t ret;

  streamer = calloc (1, sizeof(kr_streamer_t));

  streamer->params = params;

  streamer->client = kr_client_create ("krad streamer client");

  if (streamer->client == NULL) {
    fprintf (stderr, "Could not create KR client.\n");
    exit (1);
  }

  kr_connect (streamer->client, streamer->params->station);

  if (!kr_connected (streamer->client)) {
    fprintf (stderr, "Could not connect to %s krad radio daemon.\n",
             streamer->params->station);
    kr_client_destroy (&streamer->client);
    exit (1);
  }

  if (kr_compositor_get_info_wait (streamer->client,
                                   &streamer->width, &streamer->height,
                                   &streamer->fps_numerator, &streamer->fps_denominator) != 1) {
    fprintf (stderr, "Could not get compositor info!\n");
    kr_client_destroy (&streamer->client);
    exit (1);
  }

  streamer->frame_size = streamer->width * streamer->height * 4;
  //FIXME
  streamer->videoport = kr_videoport_create (streamer->client, 0);

  if (streamer->videoport == NULL) {
    fprintf (stderr, "Could not make videoport.\n");
    kr_client_destroy (&streamer->client);
    exit (1);
  } else {
    printf ("Working!\n");
  }

  kr_videoport_set_callback (streamer->videoport, new_frame, streamer);


  streamer->y4m_file = kr_file_create(params->file);

  if (streamer->y4m_file == NULL) {
    fprintf (stderr, "failed to y4m_file %s :/ \n", params->file);
    exit (1);
  }

  y4m_header_len = krad_y4m_generate_header (y4m_header, streamer->width,
   streamer->height, streamer->fps_numerator, streamer->fps_denominator,
   streamer->params->color_depth);

  ret = kr_file_write (streamer->y4m_file, y4m_header, y4m_header_len);

  if (ret != y4m_header_len) {
    fprintf (stderr, "failed to write y4m header! :(\n");
    exit (1);
  }

/*
  if (params->file != NULL) {
    streamer->mkv = kr_mkv_create_file (params->file);
  } else {
    streamer->mkv = kr_mkv_create_stream (streamer->params->host,
                                      streamer->params->port,
                                      streamer->params->mount,
                                      streamer->params->password);
  }

  if (streamer->mkv == NULL) {
    fprintf (stderr, "failed to stream :/ \n");
    exit (1);
  }

  streamer->vpx_enc = krad_vpx_encoder_create (streamer->params->width,
                                           streamer->params->height,
                                           streamer->fps_numerator,
                                           streamer->fps_denominator,
                                           streamer->params->video_bitrate);

  if (params->file != NULL) {
    krad_vpx_encoder_set_kf_max_dist (streamer->vpx_enc, 600);
  }

  kr_mkv_add_video_track (streamer->mkv, VP8,
                          streamer->fps_numerator,
                          streamer->fps_denominator,
                          streamer->params->width,
                          streamer->params->height);
*/
  streamer->frame_ring = krad_ringbuffer_create (90 * sizeof(krad_frame_t *));

  streamer->framepool = krad_framepool_create (streamer->width,
                                               streamer->height,
                                               8);

  return streamer;
}

void kr_streamer_run (kr_streamer_t *streamer) {

  krad_frame_t *frame;
  int32_t frames;
  kr_medium_t *vmedium;
  kr_codeme_t *vcodeme;
  struct SwsContext *converter;
  int sws_algo;
  int32_t ret;
  int p;
  int sws_px_fmt;

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);

  converter = NULL;
  sws_algo = SWS_BILINEAR;

  switch (streamer->params->color_depth) {
    case 444:
      sws_px_fmt = PIX_FMT_YUV444P;
      break;
    case 422:
      sws_px_fmt = PIX_FMT_YUV422P;
      break;
    default:
      sws_px_fmt = PIX_FMT_YUV420P;
      break;
  }

  vmedium = kr_medium_kludge_create ();
  vcodeme = kr_codeme_kludge_create ();

  streamer->timer = krad_timer_create ();

  kr_videoport_activate (streamer->videoport);

  while (!destroy) {

    frame = NULL;
    frames = krad_ringbuffer_read_space (streamer->frame_ring) / sizeof(void *);
  /*
    if (frames > 1) {
      krad_vpx_encoder_deadline_set (streamer->vpx_enc, 1);
      sws_algo = SWS_POINT;
    }

    if (frames == 0) {
      krad_vpx_encoder_deadline_set (streamer->vpx_enc, 10000);
      sws_algo = SWS_BILINEAR;
      usleep (2000);
      continue;
    }
  */
    if (frames > 0) {
      krad_ringbuffer_read (streamer->frame_ring,
                            (char *)&frame,
                            sizeof(krad_frame_t *));

      vmedium->v.tc = krad_timer_current_ms (streamer->timer);
      if (!krad_timer_started (streamer->timer)) {
        krad_timer_start (streamer->timer);
      }

      frame->yuv_pixels[0] = (uint8_t *)frame->pixels;
      frame->format = PIX_FMT_RGB32;
      frame->yuv_pixels[1] = NULL;
      frame->yuv_pixels[2] = NULL;
      frame->yuv_strides[0] = streamer->width * 4;
      frame->yuv_strides[1] = 0;
      frame->yuv_strides[2] = 0;
      frame->yuv_strides[3] = 0;

      converter = sws_getCachedContext ( converter,
                                         streamer->width,
                                         streamer->height,
                                         frame->format,
                                         streamer->width,
                                         streamer->height,
                                         sws_px_fmt,
                                         sws_algo,
                                         NULL, NULL, NULL);

      if (converter == NULL) {
        failfast ("Krad streamer: could not sws_getCachedContext");
      }

      switch (streamer->params->color_depth) {
        case 444:
          vmedium->v.pps[0] = streamer->width;
          vmedium->v.pps[1] = streamer->width;
          vmedium->v.pps[2] = streamer->width;
          vmedium->v.size[0] = vmedium->v.pps[0] * streamer->height;
          vmedium->v.size[1] = vmedium->v.pps[1] * streamer->height;
          vmedium->v.size[2] = vmedium->v.pps[2] * streamer->height;
          break;
        case 422:
          vmedium->v.pps[0] = streamer->width;
          vmedium->v.pps[1] = streamer->width/2;
          vmedium->v.pps[2] = streamer->width/2;
          vmedium->v.size[0] = vmedium->v.pps[0] * streamer->height;
          vmedium->v.size[1] = vmedium->v.pps[1] * streamer->height;
          vmedium->v.size[2] = vmedium->v.pps[2] * streamer->height;
          break;
        default:
          vmedium->v.pps[0] = streamer->width;
          vmedium->v.pps[1] = streamer->width/2;
          vmedium->v.pps[2] = streamer->width/2;
          vmedium->v.size[0] = vmedium->v.pps[0] * streamer->height;
          vmedium->v.size[1] = vmedium->v.pps[1] * streamer->height/2;
          vmedium->v.size[2] = vmedium->v.pps[2] * streamer->height/2;
          break;
      }

      vmedium->v.ppx[0] = vmedium->data;
      vmedium->v.ppx[1] = vmedium->data + vmedium->v.size[0];
      vmedium->v.ppx[2] = vmedium->data + vmedium->v.size[0]
                           + vmedium->v.size[1];

      sws_scale (converter,
                (const uint8_t * const*)frame->yuv_pixels,
                frame->yuv_strides,
                0,
                streamer->height,
                vmedium->v.ppx,
                vmedium->v.pps);

      krad_framepool_unref_frame (frame);
/*
      ret = kr_vpx_encode (streamer->vpx_enc, vcodeme, vmedium);
      if (ret == 1) {
        kr_mkv_add_video_tc (streamer->mkv, 1,
                             vcodeme->data, vcodeme->sz,
                             vcodeme->key, vcodeme->tc);
      }
*/


      ret = kr_file_write (streamer->y4m_file, "FRAME\n", 6);

      if (ret != 6) {
        fprintf (stderr, "failed to write y4m frame marker! :(\n");
        exit (1);
      }

      for (p = 0; p < 3; p++) {
        ret = kr_file_write (streamer->y4m_file, vmedium->v.ppx[p],
         vmedium->v.size[p]);

        if (ret != vmedium->v.size[p]) {
          fprintf (stderr, "failed to write y4m plane! :(\n");
          exit (1);
        }
      }

      printf ("\rKrad Streamer Frame# %12"PRIu64"",
              streamer->eframes++);
      fflush (stdout);

      //krad_ticker_wait (streamer->ticker);
    }
  }

  kr_medium_kludge_destroy (&vmedium);
  kr_codeme_kludge_destroy (&vcodeme);

  if (converter != NULL) {
    sws_freeContext (converter);
    converter = NULL;
  }

  krad_timer_destroy (streamer->timer);
}

void kr_streamer (kr_streamer_params_t *params) {

  kr_streamer_t *streamer;

  streamer = kr_streamer_create (params);
  kr_streamer_run (streamer);
  kr_streamer_destroy (&streamer);
}

int main (int argc, char *argv[]) {

  kr_streamer_params_t params;
  char mount[256];
  char file[256];

  memset (&params, 0, sizeof(kr_streamer_params_t));

  if (argc < 2) {
    fprintf (stderr, "Need station name.\n");
    exit (1);
  }

  params.station = argv[1];

  //params.width = 1920;
  //params.height = 1080;
  //params.width = 960;
  //params.height = 540;
  params.color_depth = 420;
  params.fps_numerator = 30;
  params.fps_denominator = 1;
  //params.video_bitrate = 1450;
  params.video_bitrate = 850;
  params.host = "europa.kradradio.com";
  params.port = 8008;

  if (argc == 3) {
    snprintf (mount, sizeof(mount), "/%s.webm", argv[2]);
  }

  params.mount = mount;
  params.password = "firefox";

  if (argc == 2) {
    snprintf (file, sizeof(file),
              "%s/%s_%"PRIu64".y4m",
              getenv ("HOME"), "kr", krad_unixtime ());
    params.file = file;
  } else {
    params.file = NULL;
  }

  printf ("Encoding: %s\n", "kr");

  if (params.file != NULL) {
    printf ("Recording to: %s\n", params.file);
  } else {
    printf ("Streaming to: %s:%u%s\n", params.host, params.port, params.mount);
  }
  printf ("VP8 Bitrate: %uk\n", params.video_bitrate);

  kr_streamer (&params);

  return 0;
}
