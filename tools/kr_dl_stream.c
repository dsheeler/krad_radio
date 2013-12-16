#include "kr_dl_stream.h"
#define IMAGE_BUFFER_COUNT 10
#include "gen/kr_dl_stream_config.c"

#include "krad_debug.c"

typedef struct kr_dlstream kr_dlstream;

struct kr_dlstream {
  kr_dlstream_params *params;
  uint32_t channels;
  kr_decklink *decklink;
  krad_ringbuffer_t *audio_ring[2];
  krad_ringbuffer_t *image_ring;
  kr_image_pool *image_pool;
  krad_vpx_encoder_t *vpx_enc;
  krad_vorbis_t *vorbis_enc;
  kr_codec_hdr_t header;
  kr_mkv_t *mkv;
  kr_convert converter;
  uint64_t frames;
  uint64_t samples;
  uint64_t droppedframes;
  uint64_t skipsamples;
};

static int destruct = 0;

static void sig_winch_handler(int sig) {
}

static void term_handler(int sig) {
  destruct = 1;
}

int dlstream_video_callback(void *user, kr_image *image) {
  kr_dlstream *dlstream;
  int ret;
  kr_image *converted_image;
  dlstream = (kr_dlstream *)user;
  memset(&converted_image, 0, sizeof(kr_image));
  ret = kr_image_pool_getimage(dlstream->image_pool, converted_image);
  if (ret == 1) {
    kr_image_convert(&dlstream->converter, converted_image, image);
    krad_ringbuffer_write (dlstream->image_ring, (char *)&converted_image,
     sizeof(kr_image *));
    dlstream->frames++;
  } else {
	  dlstream->droppedframes++;
	  dlstream->skipsamples += 2;
    printke("Krad Decklink underflow");
  }
  return 0;
}

void int16_to_float(float *dst, char *src, uint32_t ns, uint32_t src_skip) {
  const float scaling = 1.0/32767.0f;
  while (ns--) {
    *dst = (*((short *) src)) * scaling;
    dst++;
    src += src_skip;
  }
}

int dlstream_audio_callback(void *arg, void *buffer, int frames) {
  kr_dlstream *dlstream;
  int c;
  dlstream = (kr_dlstream *)arg;
  if (dlstream->skipsamples > 0) {
    dlstream->skipsamples--;
    return 0;
  }
  for (c = 0; c < 2; c++) {
    int16_to_float(dlstream->decklink->samples[c], (char *)buffer + (c * 2),
     frames, 4);
    krad_ringbuffer_write(dlstream->audio_ring[c],
     (char *)dlstream->decklink->samples[c], frames * 4);
  }
  dlstream->samples += frames;
  return 0;
}

int kr_dlstream_destroy(kr_dlstream **dlstream) {
  int c;
  if ((dlstream == NULL) || (*dlstream == NULL)) {
    return -1;
  }
  if ((*dlstream)->decklink != NULL) {
    kr_decklink_destroy((*dlstream)->decklink);
    (*dlstream)->decklink = NULL;
  }
  krad_vpx_encoder_destroy(&(*dlstream)->vpx_enc);
  krad_vorbis_encoder_destroy(&(*dlstream)->vorbis_enc);
  kr_mkv_destroy(&(*dlstream)->mkv);
  krad_ringbuffer_free((*dlstream)->image_ring);
  kr_pool_destroy((*dlstream)->image_pool);
  for (c = 0; c < (*dlstream)->channels; c++) {
    krad_ringbuffer_free((*dlstream)->audio_ring[c]);
  }
  kr_image_convert_clear(&(*dlstream)->converter);
  free(*dlstream);
  *dlstream = NULL;
  return 0;
}

kr_dlstream *kr_dlstream_create(kr_dlstream_params *params) {
  int c;
  kr_image image;
  kr_dlstream *dlstream;
  dlstream = calloc(1, sizeof(kr_dlstream));
  dlstream->params = params;
  dlstream->channels = 2;
  dlstream->skipsamples = 0;
  kr_image_convert_init(&dlstream->converter);
  dlstream->mkv = kr_mkv_create_stream(dlstream->params->host,
   dlstream->params->port, dlstream->params->mount,
   dlstream->params->password);
  if (dlstream->mkv == NULL) {
    fprintf(stderr, "Could not create stream\n");
    exit(1);
  }
  dlstream->vpx_enc = krad_vpx_encoder_create(dlstream->params->encoding_width,
   dlstream->params->encoding_height, dlstream->params->encoding_fps_numerator,
   dlstream->params->encoding_fps_denominator,
   dlstream->params->video_bitrate);
  kr_mkv_add_video_track(dlstream->mkv, VP8,
   dlstream->params->encoding_fps_numerator,
   dlstream->params->encoding_fps_denominator,
   dlstream->params->encoding_width,
   dlstream->params->encoding_height);
  dlstream->vorbis_enc = krad_vorbis_encoder_create(2, 48000,
   dlstream->params->audio_quality);
  kr_mkv_add_audio_track(dlstream->mkv, VORBIS, 48000, 2,
   dlstream->vorbis_enc->hdrdata, 3 + dlstream->vorbis_enc->header.sz[0]
   + dlstream->vorbis_enc->header.sz[1]
   + dlstream->vorbis_enc->header.sz[2]);

  image.w = dlstream->params->encoding_width;
  image.h = dlstream->params->encoding_height;
  image.fmt = PIX_FMT_UYVY422;
  image.pps[0] = dlstream->params->encoding_width;
  image.pps[1] = dlstream->params->encoding_width/2;
  image.pps[2] = dlstream->params->encoding_width/2;
  image.pps[3] = 0;

  dlstream->image_pool = kr_image_pool_create(&image, IMAGE_BUFFER_COUNT);
  dlstream->image_ring = krad_ringbuffer_create(10 * sizeof(kr_image *));
  for (c = 0; c < 2; c++) {
    dlstream->audio_ring[c] = krad_ringbuffer_create(2200000);
  }
  dlstream->decklink = kr_decklink_create(dlstream->params->input_device);
  kr_decklink_set_video_mode(dlstream->decklink,
   dlstream->params->input_width, dlstream->params->input_height,
   dlstream->params->input_fps_numerator,
   dlstream->params->input_fps_denominator);
  kr_decklink_set_audio_input(dlstream->decklink,
   dlstream->params->audio_input_connector);
  kr_decklink_set_video_input(dlstream->decklink,
   dlstream->params->video_input_connector);
  dlstream->decklink->user = dlstream;
  dlstream->decklink->audio_cb = dlstream_audio_callback;
  dlstream->decklink->image_cb = dlstream_video_callback;
  return dlstream;
}

void kr_dlstream_run(kr_dlstream *dlstream) {
  int32_t frames;
  kr_medium_t *amedium;
  kr_codeme_t *acodeme;
  kr_image *image;
  kr_codeme_t *vcodeme;
  uint32_t c;
  int32_t ret;
  int32_t muxdelay;
  muxdelay = 1;
  frames = 0;
  amedium = kr_medium_kludge_create();
  acodeme = kr_codeme_kludge_create();
  vcodeme = kr_codeme_kludge_create();
  kr_decklink_start(dlstream->decklink);
  while (!destruct) {
    printf("\rKrad Decklink Stream Frame# %12"PRIu64" Samples: %12"PRIu64" "
     "Logjam: %8d Dropped: %6"PRIu64"", dlstream->frames, dlstream->samples,
     frames, dlstream->droppedframes);
    fflush(stdout);
    while(krad_ringbuffer_read_space(dlstream->audio_ring[1]) >= 1024 * 4) {
      for (c = 0; c < dlstream->channels; c++) {
        krad_ringbuffer_read (dlstream->audio_ring[c],
        (char *)amedium->a.samples[c], 1024 * 4);
      }
      amedium->a.count = 1024;
      amedium->a.channels = 2;
      ret = kr_vorbis_encode(dlstream->vorbis_enc, acodeme, amedium);
      if (ret == 1) {
        kr_mkv_add_audio(dlstream->mkv, 2, acodeme->data, acodeme->sz,
         acodeme->count);
        muxdelay = 0;
        while (1) {
          ret = kr_vorbis_encode(dlstream->vorbis_enc, acodeme, NULL);
          if (ret == 1) {
            kr_mkv_add_audio(dlstream->mkv, 2, acodeme->data, acodeme->sz,
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
//  frames = krad_ringbuffer_read_space(dlstream->frame_ring) / sizeof(void *);
    if (frames == 0) {
      krad_vpx_encoder_deadline_set(dlstream->vpx_enc, 50000);
      usleep(1000);
    }
    if (frames > 1) {
      krad_vpx_encoder_deadline_set(dlstream->vpx_enc, 30000);
    }
    if (frames > 15) {
      krad_vpx_encoder_deadline_set(dlstream->vpx_enc, 1);
    }
    if (frames > 0) {
//    krad_ringbuffer_read(dlstream->frame_ring, (char *)&frame,
//     sizeof(krad_frame_t *));
      ret = kr_vpx_encode(dlstream->vpx_enc, vcodeme, image);
      //krad_framepool_unref_frame(frame);
      if (ret == 1) {
        kr_mkv_add_video(dlstream->mkv, 1, vcodeme->data, vcodeme->sz,
         vcodeme->key);
      }
    }
  }
  kr_medium_kludge_destroy(&amedium);
  kr_codeme_kludge_destroy(&acodeme);
  kr_codeme_kludge_destroy(&vcodeme);
}

void kr_dlstream_check() {
  int count;
  count = kr_decklink_detect_devices();
  if (count < 1) {
    printf("No Decklink devices detected.\n");
    exit(0);
  } else {
    if (count > 1) {
      printf("%d Decklink devices detected.\n", count);
    } else {
      printf("%d Decklink device detected.\n", count);
    }
  }
}

int main (int argc, char *argv[]) {

  kr_dlstream *dlstream;
  kr_dlstream_params params;
  int ret;

  krad_debug_init("dl_stream");
  kr_dlstream_check();
  memset(&params, 0, sizeof(kr_dlstream_params));

  ret = handle_config(&params, argv[1]);
  if (ret != 0) {
    printf("Config file %s error.\n", argv[1]);
    exit(1);
  }

  printf("Streaming with: %s at %ux%u %u fps (max)\n",
   params.input_device, params.input_width, params.input_height,
   params.input_fps_numerator/params.input_fps_denominator);
  printf("To: %s:%u%s\n", params.host, params.port, params.mount);
  printf("Encoding Resolution: %dx%d FPS: %d/%d\n",
   params.encoding_width, params.encoding_height,
   params.encoding_fps_numerator, params.encoding_fps_denominator);
  printf("VP8 Bitrate: %uk Vorbis Quality: %.2f\n",
   params.video_bitrate, params.audio_quality);

  dlstream = kr_dlstream_create(&params);
  if (dlstream != NULL) {
    signal(SIGWINCH, sig_winch_handler);
    signal(SIGINT, term_handler);
    signal(SIGTERM, term_handler);
    kr_dlstream_run(dlstream);
    kr_dlstream_destroy(&dlstream);
  }
  krad_debug_shutdown();

  return 0;
}
