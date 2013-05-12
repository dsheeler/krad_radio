#include <stdio.h>
#include <unistd.h>

#ifdef FRAK_MACOSX
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFstring.h>
#include <CoreFoundation/CFbundle.h>
#include "krad_mach.h"
#endif

#ifdef KR_LINUX
#include <krad_muxponder.h>
#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>
#endif

#ifdef FRAK_MACOSX
#include <krad_ticker.h>
#include <krad_mkv.h>
#endif

#include <krad_vpx.h>
#include <krad_vorbis.h>
#include <krad_decklink.h>
#include <krad_decklink_capture.h>

#include <krad_ring.h>
#include <krad_framepool_nc.h>

#include <libswscale/swscale.h>

#include "krad_debug.c"

typedef struct kr_dlstream_St kr_dlstream_t;
typedef struct kr_dlstream_params_St kr_dlstream_params_t;

struct kr_dlstream_params_St {
  uint32_t input_width;
  uint32_t input_height;
  uint32_t input_fps_numerator;
  uint32_t input_fps_denominator;
  char *input_device;
  char *video_input_connector;
  char *audio_input_connector;
  uint32_t encoding_width;
  uint32_t encoding_height;
  uint32_t encoding_fps_numerator;
  uint32_t encoding_fps_denominator;
  uint32_t video_bitrate;
  float audio_quality;
  char *host;
  int32_t port;
  char *mount;
  char *password;
};

struct kr_dlstream_St {
  kr_dlstream_params_t *params;
  uint32_t channels;
  krad_decklink_t *decklink;
  krad_ringbuffer_t *audio_ring[2];
  krad_ringbuffer_t *frame_ring;
  krad_framepool_t *framepool;
  krad_vpx_encoder_t *vpx_enc;
  krad_vorbis_t *vorbis_enc;
  kr_codec_hdr_t header;
  kr_mkv_t *mkv;
  struct SwsContext *converter;
  int sws_algo;
  int new_sws_algo;
  uint64_t frames;
  uint64_t samples;
  uint64_t droppedframes;
  uint64_t skipsamples;
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
  uint32_t lumasize;
  uint32_t chromasize;
  krad_frame_t *frame;
  krad_frame_t dlframe;

  memset (&dlframe, 0, sizeof(krad_frame_t));

  stride = dlstream->params->input_width + ((dlstream->params->input_width/2) * 2);
  
  frame = krad_framepool_getframe (dlstream->framepool);

  if (frame != NULL) {

    dlframe.format = PIX_FMT_UYVY422;

    dlframe.yuv_pixels[0] = buffer;
    dlframe.yuv_pixels[1] = NULL;
    dlframe.yuv_pixels[2] = NULL;

    dlframe.yuv_strides[0] = stride;
    dlframe.yuv_strides[1] = 0;
    dlframe.yuv_strides[2] = 0;
    dlframe.yuv_strides[3] = 0;

	lumasize = dlstream->params->encoding_width * dlstream->params->encoding_height;
	lumasize += lumasize % 16;
	chromasize = ((dlstream->params->encoding_width * dlstream->params->encoding_height) / 4);
	chromasize += chromasize % 16;

	frame->format = PIX_FMT_YUV420P;
    frame->yuv_strides[0] = dlstream->params->encoding_width;
    frame->yuv_strides[1] = dlstream->params->encoding_width/2;  
    frame->yuv_strides[2] = dlstream->params->encoding_width/2;
    frame->yuv_pixels[0] = frame->pixels;
    frame->yuv_pixels[1] = frame->pixels + lumasize;  
    frame->yuv_pixels[2] = frame->pixels + (lumasize + chromasize);

	dlstream->sws_algo = dlstream->new_sws_algo;

	dlstream->converter = sws_getCachedContext ( dlstream->converter,
	                                             dlstream->params->input_width,
	                                             dlstream->params->input_height,
	                                             dlframe.format,
	                                             dlstream->params->encoding_width,
	                                             dlstream->params->encoding_height,
	                                             frame->format,
	                                             dlstream->sws_algo,
	                                             NULL, NULL, NULL);

	if (dlstream->converter == NULL) {
	  failfast ("Krad dlstream: could not sws_getCachedContext");
	}

	sws_scale (dlstream->converter,
	          (const uint8_t * const*)dlframe.yuv_pixels,
	          dlframe.yuv_strides,
	          0,
	          dlstream->params->input_height,
	          frame->yuv_pixels,
	          frame->yuv_strides);

    krad_ringbuffer_write (dlstream->frame_ring,
                          (char *)&frame,
                          sizeof(krad_frame_t *));

    dlstream->frames++;

  } else {
	dlstream->droppedframes++;
	dlstream->skipsamples += 2;
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

  if (	dlstream->skipsamples > 0) {
	dlstream->skipsamples--;
	return 0;
  }

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
  if ((*dlstream)->converter != NULL) {
    sws_freeContext ((*dlstream)->converter);
    (*dlstream)->converter = NULL;
  }
  free (*dlstream);
  *dlstream = NULL;
  return 0;
}

kr_dlstream_t *kr_dlstream_create (kr_dlstream_params_t *params) {

  kr_dlstream_t *dlstream;

  dlstream = calloc (1, sizeof(kr_dlstream_t));

  int c;
  
  dlstream->params = params;

  dlstream->channels = 2;
  dlstream->skipsamples = 0;
  dlstream->converter = NULL;
  dlstream->sws_algo = SWS_BILINEAR;
  dlstream->new_sws_algo = dlstream->sws_algo;

  char file[512];
  
  snprintf (file, sizeof(file),
            "%s/%s_%"PRIu64".webm",
            getenv ("HOME"), "dlstream", krad_unixtime ());
  
  //dlstream->mkv = kr_mkv_create_file (file);

  dlstream->mkv = kr_mkv_create_stream (dlstream->params->host,
                                        dlstream->params->port,
                                        dlstream->params->mount,
                                        dlstream->params->password);

  if (dlstream->mkv == NULL) {
    fprintf (stderr, "Could not create %s\n", file);
    exit (1);
  }

  //printf ("Created file: %s\n", file);

  dlstream->vpx_enc = krad_vpx_encoder_create (dlstream->params->encoding_width,
                                               dlstream->params->encoding_height,
                                               dlstream->params->encoding_fps_numerator,
                                               dlstream->params->encoding_fps_denominator,
                                               dlstream->params->video_bitrate);

  kr_mkv_add_video_track (dlstream->mkv, VP8,
                          dlstream->params->encoding_fps_numerator,
                          dlstream->params->encoding_fps_denominator,
                          dlstream->params->encoding_width,
                          dlstream->params->encoding_height);

  dlstream->vorbis_enc = krad_vorbis_encoder_create (2,
                                                     48000,
                                                     dlstream->params->audio_quality);

  kr_mkv_add_audio_track (dlstream->mkv, VORBIS, 48000, 2,
                          dlstream->vorbis_enc->hdrdata,
                          3 +
                          dlstream->vorbis_enc->header.sz[0] +
                          dlstream->vorbis_enc->header.sz[1] +
                          dlstream->vorbis_enc->header.sz[2]);

  dlstream->frame_ring = krad_ringbuffer_create (120 * sizeof(krad_frame_t *));

  dlstream->framepool = krad_framepool_create ( dlstream->params->encoding_width,
                                                dlstream->params->encoding_height,
                                                120);

  for (c = 0; c < 2; c++) {
    dlstream->audio_ring[c] = krad_ringbuffer_create (2200000);    
  }

  dlstream->decklink = krad_decklink_create (dlstream->params->input_device);

  krad_decklink_set_video_mode (dlstream->decklink,
                                dlstream->params->input_width,
                                dlstream->params->input_height,
                                dlstream->params->input_fps_numerator,
                                dlstream->params->input_fps_denominator);


  krad_decklink_set_audio_input (dlstream->decklink, dlstream->params->audio_input_connector);
  krad_decklink_set_video_input (dlstream->decklink, dlstream->params->video_input_connector);

  dlstream->decklink->callback_pointer = dlstream;
  dlstream->decklink->audio_frames_callback = dlstream_audio_callback;
  dlstream->decklink->video_frame_callback = dlstream_video_callback;
  
  return dlstream;
}

void kr_dlstream_run (kr_dlstream_t *dlstream) {

  krad_frame_t *frame;
  int32_t frames;
  kr_medium_t *amedium;
  kr_codeme_t *acodeme;
  kr_medium_t *vmedium;
  kr_codeme_t *vcodeme;
  uint32_t c;
  int32_t ret;
  int32_t muxdelay;

  muxdelay = 1;
  frames = 0;

  signal (SIGWINCH, sig_winch_handler);
  signal (SIGINT, term_handler);
  signal (SIGTERM, term_handler);    

  amedium = kr_medium_kludge_create ();
  acodeme = kr_codeme_kludge_create ();
  vmedium = calloc (1, sizeof(kr_medium_t));
  vcodeme = kr_codeme_kludge_create ();

  krad_decklink_start (dlstream->decklink);

  while (!destruct) {
        
    printf ("\rKrad Decklink Stream Frame# %12"PRIu64" Samples: %12"PRIu64" Logjam: %8d Dropped: %6"PRIu64"",
          dlstream->frames, dlstream->samples, frames, dlstream->droppedframes);

    fflush (stdout);

    while (krad_ringbuffer_read_space (dlstream->audio_ring[1]) >= 1024 * 4) {

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
        muxdelay = 0;
        while (1) {
	      ret = kr_vorbis_encode (dlstream->vorbis_enc, acodeme, NULL);
	      if (ret == 1) {
	        kr_mkv_add_audio (dlstream->mkv, 2,
	                          acodeme->data,
	                          acodeme->sz,
	                          acodeme->count);
          } else {
			break;
          }
        }
      }
    }

    if (muxdelay > 0) {
      continue;
    }
   
	frames = krad_ringbuffer_read_space (dlstream->frame_ring) / sizeof(void *);
	
    if (frames > 1) {
      krad_vpx_encoder_deadline_set (dlstream->vpx_enc, 1);
	  dlstream->new_sws_algo = SWS_POINT;
    }
	
    if (frames == 0) {
      krad_vpx_encoder_deadline_set (dlstream->vpx_enc, 10000);
	  dlstream->new_sws_algo = SWS_BILINEAR;
      usleep (2000);
    }
	
	if (frames > 0) {
      krad_ringbuffer_read (dlstream->frame_ring,
                            (char *)&frame,
                            sizeof(krad_frame_t *));

      vmedium->v.pps[0] = frame->yuv_strides[0];
      vmedium->v.pps[1] = frame->yuv_strides[1];  
      vmedium->v.pps[2] = frame->yuv_strides[2];
      /*
      vmedium->v.ppx[0] = frame->yuv_pixels[0];
      vmedium->v.ppx[1] = frame->yuv_pixels[1];  
      vmedium->v.ppx[2] = frame->yuv_pixels[2];
      */

      vmedium->data = frame->pixels;

      ret = kr_vpx_encode (dlstream->vpx_enc, vcodeme, vmedium);
      krad_framepool_unref_frame (frame);
      if (ret == 1) {
        kr_mkv_add_video (dlstream->mkv, 1,
                          vcodeme->data, vcodeme->sz, vcodeme->key);      
      }
    }
  }

  kr_medium_kludge_destroy (&amedium);
  kr_codeme_kludge_destroy (&acodeme);
  free (vmedium);
  kr_codeme_kludge_destroy (&vcodeme);
}

void kr_dlstream_check () {

  int count;

  count = krad_decklink_detect_devices ();

  if (count < 1) {
    printf ("No Decklink devices detected.\n");
    exit (0);
  } else {
    if (count > 1) {
      printf ("%d Decklink devices detected.\n", count);
    } else {
      printf ("%d Decklink device detected.\n", count);
    }
  }
}

void kr_dlstream (kr_dlstream_params_t *params) {

  kr_dlstream_t *dlstream;
  
  kr_dlstream_check ();

  dlstream = kr_dlstream_create (params);

  kr_dlstream_run (dlstream);

  kr_dlstream_destroy (&dlstream);
}

static pthread_t dl_stream_thread_var;

void *kr_dlstream_thread (void *arg) {
	
	kr_dlstream_params_t *params;
	kr_dlstream_params_t myparams;

	params = (kr_dlstream_params_t *)arg;

	memcpy (&myparams, params, sizeof(kr_dlstream_params_t));
	
	kr_dlstream (&myparams);
	return NULL;
}

void kr_dlstream_thread_stop () {
	destruct = 1;
	pthread_join (dl_stream_thread_var, NULL);
	destruct = 0;
}

void kr_dlstream_thread_start (kr_dlstream_params_t *params) {
  pthread_create (&dl_stream_thread_var, NULL, kr_dlstream_thread, params);
}

int kr_dlstream_device_count (int device_num, char *device_name) {
  return krad_decklink_detect_devices ();
}

int kr_dlstream_device_name (int device_num, char *device_name) {
  return krad_decklink_get_device_name (device_num, device_name);
}


#ifdef KR_LINUX
int main (int argc, char *argv[]) {

  krad_debug_init ("dl_stream");

  kr_dlstream_params_t params;
  char mount[256];

  memset (&params, 0, sizeof(kr_dlstream_params_t));
  snprintf (mount, sizeof(mount),
            "/kr_x11s_%"PRIu64".webm",
            krad_unixtime ());

  params.input_device = "0";
  params.input_width = 1280;
  params.input_height = 720;
  params.input_fps_numerator = 60000;
  params.input_fps_denominator = 1000;
  params.video_bitrate = 450;
  params.video_input_connector = "hdmi";
  params.audio_input_connector = "hdmi";
  params.video_bitrate = 800;
  params.audio_quality = 0.4;

  params.host = "europa.kradradio.com";
  params.port = 8008;
  params.mount = mount;
  params.password = "firefox";

  printf ("Streaming with: %s at %ux%u %u fps (max)\n",
          params.input_device, params.input_width, params.input_height,
          params.input_fps_numerator/params.input_fps_denominator);
  printf ("To: %s:%u%s\n", params.host, params.port, params.mount);
  printf ("Encoding Resolution: %dx%d FPS: %d/%d\n",
          params.encoding_width, params.encoding_height,
          params.encoding_fps_numerator, params.encoding_fps_denominator);
  printf ("VP8 Bitrate: %uk Vorbis Quality: %.2f\n",
		  params.video_bitrate, params.audio_quality);


  kr_dlstream (&params);

  krad_debug_shutdown ();

  return 0;
}
#endif
