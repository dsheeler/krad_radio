#include <stdio.h>
#include <unistd.h>

#include <krad_muxponder.h>
#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>

#include <krad_vpx.h>
#include <krad_vorbis.h>
#include <krad_decklink.h>

#include <krad_ring.h>
#include <krad_framepool.h>

#include <libswscale/swscale.h>

#include "krad_debug.c"

typedef struct kr_dlstream_St kr_dlstream_t;

struct kr_dlstream_St {
  uint32_t width;
  uint32_t height;
  uint32_t fps_numerator;
  uint32_t fps_denominator;
  uint32_t video_bitrate;
  float audio_quality;
  char *host;
  int32_t port;
  char *mount;
  char *password;
  uint32_t channels;
  char *device;
  char *video_input;
  char *audio_input;  
  krad_decklink_t *decklink;
  krad_ringbuffer_t *audio_ring[2];
  krad_ringbuffer_t *frame_ring;
  krad_framepool_t *framepool;
  krad_vpx_encoder_t *vpx_enc;
  krad_vorbis_t *vorbis_enc;
  kr_codec_hdr_t header;
  kr_mkv_t *mkv;
  uint64_t frames;
  uint64_t samples;
};

static int destruct = 0;

static void sig_winch_handler (int sig) {
  //printf ("sig_winch_handler!\n");
}

static void term_handler (int sig) {
  destruct = 1;
}

int dlstream_video_callback (void *arg, void *buffer, int length) {

  kr_dlstream_t *dlstream = (kr_dlstream_t *)arg;

  uint32_t stride;
  krad_frame_t *krad_frame;
  
  stride = dlstream->width + ((dlstream->width/2) * 2);
  
  printf ("\rKrad Decklink Stream Frame# %12"PRIu64" Samples: %12"PRIu64"",
          dlstream->frames++, dlstream->samples);

  fflush (stdout);

  krad_frame = krad_framepool_getframe (dlstream->framepool);

  if (krad_frame != NULL) {

    krad_frame->format = PIX_FMT_UYVY422;

    krad_frame->yuv_pixels[0] = buffer;
    krad_frame->yuv_pixels[1] = NULL;
    krad_frame->yuv_pixels[2] = NULL;

    krad_frame->yuv_strides[0] = stride;
    krad_frame->yuv_strides[1] = 0;
    krad_frame->yuv_strides[2] = 0;
    krad_frame->yuv_strides[3] = 0;

    krad_framepool_ref_frame (krad_frame);
    krad_ringbuffer_write (dlstream->frame_ring,
                          (char *)&krad_frame,
                          sizeof(krad_frame_t *));
    krad_framepool_unref_frame (krad_frame);

  } else {
    printke ("Krad Decklink underflow");
  }
  return 0;
}

#define SAMPLE_16BIT_SCALING 32767.0f

void int16_to_float (float *dst, char *src,
                     uint32_t nsamples, uint32_t src_skip) {

  const float scaling = 1.0/SAMPLE_16BIT_SCALING;
  while (nsamples--) {
    *dst = (*((short *) src)) * scaling;
    dst++;
    src += src_skip;
  }
}

int dlstream_audio_callback (void *arg, void *buffer, int frames) {

  kr_dlstream_t *dlstream = (kr_dlstream_t *)arg;

  int c;

  for (c = 0; c < 2; c++) {
    int16_to_float (dlstream->decklink->samples[c],
                    (char *)buffer + (c * 2),
                    frames, 4);
    krad_ringbuffer_write (dlstream->audio_ring[c],
                           (char *)dlstream->decklink->samples[c],
                           frames * 4);
  }
  
  dlstream->samples += frames;
  
  return 0;
}

int kr_dlstream_destroy (kr_dlstream_t **dlstream) {

  int c;

  if ((dlstream == NULL) || (*dlstream == NULL)) {
    return -1;
  }

  if ((*dlstream)->decklink != NULL) {
    krad_decklink_destroy ((*dlstream)->decklink);
    (*dlstream)->decklink = NULL;
  }

  krad_vpx_encoder_destroy (&(*dlstream)->vpx_enc);
  krad_vorbis_encoder_destroy (&(*dlstream)->vorbis_enc);
  kr_mkv_destroy (&(*dlstream)->mkv);

  for (c = 0; c < (*dlstream)->channels; c++) {
    krad_ringbuffer_free ((*dlstream)->audio_ring[c]);
  }
  
  krad_ringbuffer_free ((*dlstream)->frame_ring);

  krad_framepool_destroy (&(*dlstream)->framepool);

  free (*dlstream);
  *dlstream = NULL;
  return 0;
}

kr_dlstream_t *kr_dlstream_create () {

  kr_dlstream_t *dlstream;

  dlstream = calloc (1, sizeof(kr_dlstream_t));

  int c;
  
  dlstream->width = 1920;
  dlstream->height = 1080;
  dlstream->fps_numerator = 60000;
  dlstream->fps_denominator = 1001;
  dlstream->device = "0";
  dlstream->channels = 2;
  dlstream->video_input = "hdmi";
  dlstream->audio_input = "analog";
  dlstream->video_bitrate = 1500;
  dlstream->audio_quality = 0.4;
  
  dlstream->host = "europa.kradradio.com";
  dlstream->port = 8008;
  dlstream->mount = "/kr_decklink.webm";
  dlstream->password = "firefox";
  
  char file[512];
  
  snprintf (file, sizeof(file),
            "%s/%s_%"PRIu64".webm",
            getenv ("HOME"), "dlstream", krad_unixtime ());
  
  //dlstream->mkv = kr_mkv_create_file (file);

  dlstream->mkv = kr_mkv_create_stream (dlstream->host,
                                        dlstream->port,
                                        dlstream->mount,
                                        dlstream->password);

  if (dlstream->mkv == NULL) {
    fprintf (stderr, "Could not create %s\n", file);
    exit (1);
  }

  printf ("Created file: %s\n", file);

  dlstream->vpx_enc = krad_vpx_encoder_create (dlstream->width,
                                               dlstream->height,
                                               dlstream->fps_numerator / 2,
                                               dlstream->fps_denominator,
                                               dlstream->video_bitrate);

  kr_mkv_add_video_track (dlstream->mkv, VP8,
                          dlstream->fps_numerator / 2,
                          dlstream->fps_denominator,
                          dlstream->width,
                          dlstream->height);

  dlstream->vorbis_enc = krad_vorbis_encoder_create (2,
                                                     48000,
                                                     dlstream->audio_quality);

  kr_mkv_add_audio_track (dlstream->mkv, VORBIS, 48000, 2,
                          dlstream->vorbis_enc->hdrdata,
                          3 +
                          dlstream->vorbis_enc->header.sz[0] +
                          dlstream->vorbis_enc->header.sz[1] +
                          dlstream->vorbis_enc->header.sz[2]);

  dlstream->frame_ring = krad_ringbuffer_create (90 * sizeof(krad_frame_t *));

  dlstream->framepool = krad_framepool_create ( dlstream->width,
                                                dlstream->height,
                                                90);

  for (c = 0; c < 2; c++) {
    dlstream->audio_ring[c] = krad_ringbuffer_create (1000000);    
  }

  dlstream->decklink = krad_decklink_create (dlstream->device);

  krad_decklink_set_video_mode (dlstream->decklink,
                                dlstream->width,
                                dlstream->height,
                                dlstream->fps_numerator,
                                dlstream->fps_denominator);


  krad_decklink_set_audio_input (dlstream->decklink, dlstream->audio_input);
  krad_decklink_set_video_input (dlstream->decklink, dlstream->video_input);

  dlstream->decklink->callback_pointer = dlstream;
  dlstream->decklink->audio_frames_callback = dlstream_audio_callback;
  dlstream->decklink->video_frame_callback = dlstream_video_callback;
  
  return dlstream;
}

void kr_dlstream_run (kr_dlstream_t *dlstream) {

  krad_frame_t *frame;
  kr_medium_t *amedium;
  kr_codeme_t *acodeme;
  kr_medium_t *vmedium;
  kr_codeme_t *vcodeme;
  struct SwsContext *converter;
  int sws_algo;
  uint32_t c;
  int32_t ret;

  signal (SIGWINCH, sig_winch_handler);
  signal (SIGINT, term_handler);
  signal (SIGTERM, term_handler);    

  converter = NULL;
  sws_algo = SWS_BILINEAR;

  amedium = kr_medium_kludge_create ();
  acodeme = kr_codeme_kludge_create ();
  vmedium = kr_medium_kludge_create ();
  vcodeme = kr_codeme_kludge_create ();

  krad_decklink_start (dlstream->decklink);

  while (!destruct) {
    usleep (5000);
    
    while (krad_ringbuffer_read_space(dlstream->audio_ring[1]) >= 1024 * 4) {

      for (c = 0; c < dlstream->channels; c++) {
        krad_ringbuffer_read (dlstream->audio_ring[c],
        (char *)amedium->a.samples[c], 1024 * 4);
      }

      amedium->a.count = 1024;
      amedium->a.channels = 2;
      
      ret = kr_vorbis_encode (dlstream->vorbis_enc, acodeme, amedium);
      if (ret == 1) {
        kr_mkv_add_audio (dlstream->mkv, 2,
                          acodeme->data,
                          acodeme->sz,
                          acodeme->count);
      }
    }
   
    if (krad_ringbuffer_read_space (dlstream->frame_ring) >= sizeof(void *)) {
      krad_ringbuffer_read (dlstream->frame_ring,
                            (char *)&frame,
                            sizeof(krad_frame_t *));


  
      converter = sws_getCachedContext ( converter,
                                         dlstream->width,
                                         dlstream->height,
                                         frame->format,
                                         dlstream->width,
                                         dlstream->height,
                                         PIX_FMT_YUV420P, 
                                         sws_algo,
                                         NULL, NULL, NULL);

      if (converter == NULL) {
        failfast ("Krad dlstream: could not sws_getCachedContext");
      }

      vmedium->v.pps[0] = dlstream->width;
      vmedium->v.pps[1] = dlstream->width/2;  
      vmedium->v.pps[2] = dlstream->width/2;
      vmedium->v.ppx[0] = vmedium->data;
      vmedium->v.ppx[1] = vmedium->data + dlstream->width * (dlstream->height);  
      vmedium->v.ppx[2] = vmedium->data + dlstream->width *
                          (dlstream->height) +
                          ((dlstream->width * (dlstream->height)) /4);

      sws_scale (converter,
                (const uint8_t * const*)frame->yuv_pixels,
                frame->yuv_strides,
                0,
                dlstream->height,
                vmedium->v.ppx,
                vmedium->v.pps);
      krad_framepool_unref_frame (frame);

      ret = kr_vpx_encode (dlstream->vpx_enc, vcodeme, vmedium);
      if (ret == 1) {
        kr_mkv_add_video (dlstream->mkv, 1,
                          vcodeme->data, vcodeme->sz, vcodeme->key);      
      }
    }
  }

  kr_medium_kludge_destroy (&amedium);
  kr_codeme_kludge_destroy (&acodeme);
  kr_medium_kludge_destroy (&vmedium);
  kr_codeme_kludge_destroy (&vcodeme);

  if (converter != NULL) {
    sws_freeContext ( converter );
    converter = NULL;
  }

}

void kr_dlstream () {

  kr_dlstream_t *dlstream;
  
  dlstream = kr_dlstream_create ();

  kr_dlstream_run (dlstream);

  kr_dlstream_destroy (&dlstream);
}
  
int main (int argc, char *argv[]) {

  krad_debug_init ("dl_stream");

  kr_dlstream ();

  krad_debug_shutdown ();

  return 0;
}
