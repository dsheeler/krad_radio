#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <libavutil/avutil.h>
/*
#include <libavutil/time.h>
#include <libavutil/frame.h>
*/
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavresample/avresample.h>
#include "kr_client.h"

typedef struct krad_player_St krad_player_t;
typedef struct krad_player_St kplayer_t;
typedef struct krad_libav_St krad_libav_t;

struct krad_libav_St {
  AVFormatContext *ctx;
  AVAudioResampleContext *avr;
  AVRational sar;
  AVInputFormat *iformat;
  AVStream *audio_st;
  AVPacket audio_pkt_temp;
  AVPacket audio_pkt;
  enum AVPixelFormat pix_fmt;
  AVFrame *frame;
  int video_stream;
  AVStream *video_st;
  AVPacket flush_pkt;
};

struct krad_player_St {
  uint64_t samples;
  float timecode;
  float ms;
  kr_client_t *client;
  kr_videoport_t *videoport;
  kr_audioport_t *audioport;
  struct SwsContext *sws_converter;
  char *station;
  unsigned char *rgba[960 * 540 * 4];
  unsigned char *rgba2[960 * 540 * 4];
  int updated;
  int updated2;
  int old_src_w;
  int old_src_h;
  int old_px_fmt;
  float audio0[8192];
  float audio1[8192];
  int samples_buffered;
  krad_libav_t avc;
};
/*
void *video_decoding_thread (void *arg) {

  krad_player_t *player;
  AVPacket pkt;
  AVFrame *frame = av_frame_alloc();
  int64_t pts_int;
  double pts;
  int ret;
  int frames;
  
  player = (krad_player_t *)arg;
  frames = 0;

  printf ("Video thread!\n");

  for (;;) {
    av_free_packet(&pkt);
    //avcodec_flush_buffers(is->video_st->codec);
    //avcodec_decode_video2 (is->video_st->codec, frame, &got_picture, pkt);
    av_frame_unref(frame);
    if (ret < 0)
        break;
    if (!ret)
        continue;
    pts += 0.0417188;
     // pts = pts_int * av_q2d(is->video_st->time_base);
        printf ("Video frame! %d\n", frames++);


  int rgb_stride_arr[3] = {4*960, 0, 0};
  uint8_t *dst[4];
  
  player->sws_converter = sws_getCachedContext ( player->sws_converter,
                                                frame->width,
                                                frame->height,
                                                frame->format,
                                                960,
                                                540,
                                                PIX_FMT_RGB32, 
                                                SWS_BICUBIC,
                                                NULL, NULL, NULL);
  
    printf ("last frame ms: %f kplayer ms: %f", pts, player->ms);

    if (pts < player->ms) {
      return NULL;
    }

    while (pts > player->ms + 0.040) {
      usleep(2000);
    }

    if (player->updated == 0) {
      dst[0] = player->rgba;
      sws_scale (player->sws_converter, frame->data, frame->linesize,
                 0, frame->height, dst, rgb_stride_arr);
      player->timecode = pts;
      player->updated = 1;
    }
   }
  av_free_packet(&pkt);
  av_frame_free(&frame);
  return NULL;
}
*/
void krad_player_close (krad_player_t *player) {
  avformat_close_input (&player->avc.ctx);
}

void krad_player_open (krad_player_t *player, char *input) {

  int err;
  player->avc.ctx = avformat_alloc_context ();
  
  err = avformat_open_input (&player->avc.ctx, input, NULL, NULL);
  if (err < 0) {
    fprintf (stderr, "Krad Player: Could not open input: %s", input);
    return;
  }
  err = avformat_find_stream_info (player->avc.ctx, NULL);
  if (err < 0) {
    fprintf (stderr, "Krad Player: Could get info on input: %s", input);
    return;
  }

  int stream_id = -1;
  int i;

  for (i = 0; i < player->avc.ctx->nb_streams; i++) {
    if (player->avc.ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
      stream_id = i;
      break;
     }
   }
 
  if (stream_id == -1) {
    fprintf (stderr, "Krad Player: Could get info on input: %s\n", input);
    return;
  }
 
  if (0) {
    AVDictionary* metadata = player->avc.ctx->metadata;
    
    AVDictionaryEntry *artist = av_dict_get (metadata, "artist", NULL, 0);
    AVDictionaryEntry *title = av_dict_get (metadata, "title", NULL, 0);

    fprintf (stdout, "Playing: %s - %s\n", artist->value, title->value);
  }

  AVCodecContext* codec_ctx = player->avc.ctx->streams[stream_id]->codec;
  AVCodec* codec = avcodec_find_decoder (codec_ctx->codec_id);

  if (!avcodec_open2 (codec_ctx, codec, NULL) < 0) {
    fprintf (stderr, "Krad Player: Could not find open the needed codec");
  }
  
  player->avc.avr = avresample_alloc_context();
  av_opt_set_int(player->avc.avr, "in_channel_layout",  codec_ctx->channel_layout, 0);
  av_opt_set_int(player->avc.avr, "out_channel_layout", AV_CH_LAYOUT_STEREO,  0);
  av_opt_set_int(player->avc.avr, "in_sample_rate",     codec_ctx->sample_rate,                0);
  av_opt_set_int(player->avc.avr, "out_sample_rate",    48000,                0);
  av_opt_set_int(player->avc.avr, "in_sample_fmt",      codec_ctx->sample_fmt,   0);
  av_opt_set_int(player->avc.avr, "out_sample_fmt",     AV_SAMPLE_FMT_FLTP,    0);

  avresample_open (player->avc.avr);  
  
  kr_audioport_activate (player->audioport);

  AVPacket packet;
  AVFrame *frame;
  int got_frame;
  
  frame = av_frame_alloc();

  while (1) {
    if (av_read_frame (player->avc.ctx, &packet) < 0) {
      printf ("Got to EOF\n");
      break;
    }

    if (packet.stream_index != stream_id) {
      //printf ("skipping track %d\n", packet.stream_index);
      av_free_packet (&packet);
      continue;
    }

    err = avcodec_decode_audio4 (codec_ctx,
                                 frame,
                                 &got_frame,
                                 &packet);
    if (err < 0) {
      fprintf (stderr, "Krad Player: Error decoding audio\n");
      break;
    }
    
    av_free_packet (&packet);
    
    if (!got_frame) {
      printf ("Krad Player: Got to EOF from decoder\n");
      break;
    }
    
    while (avresample_available(player->avc.avr) >= 48000) {
      usleep (10000);
    }

    player->samples_buffered += avresample_convert (player->avc.avr,
                                                    NULL, 0, 0,
                                                    frame->data,
                                                    frame->linesize[0],
                                                    frame->nb_samples);
    printf ("Krad Player: Playback position: %3.2fs\r",
            (float)player->samples / 48000.0f);
    fflush (stdout);
  }

  av_frame_free (&frame);
  kr_audioport_deactivate (player->audioport);
  avresample_free(&player->avc.avr);
  krad_player_close (player);
}

int videoport_process (void *buffer, void *arg) {

  krad_player_t *player;
  player = (krad_player_t *)arg;

  if (player->updated == 1) {
    memcpy (buffer, player->rgba, 960 * 540 * 4);
    player->updated = 0;
  }
  return 0;
}

int audioport_process (uint32_t nframes, void *arg) {

  krad_player_t *player;

  uint8_t *outputs[2];

  player = (krad_player_t *)arg;

  if (avresample_available(player->avc.avr) >= nframes) {
    outputs[0] = (uint8_t *)kr_audioport_get_buffer (player->audioport, 0);
    outputs[1] = (uint8_t *)kr_audioport_get_buffer (player->audioport, 1);

    avresample_read (player->avc.avr, outputs, nframes);
  }

  player->samples += 1024;
  player->ms = (float)player->samples / 48000.0f;

  return 0;
}

void libav_init () {
  av_log_set_flags (AV_LOG_SKIP_REPEATED);
  avcodec_register_all();
  avdevice_register_all();
  av_register_all();
  avformat_network_init();
}

void libav_shutdown () {
  avformat_network_deinit();
}

void krad_player_destroy (krad_player_t *player) {

  printf ("Krad Player: Self Destructing\n");
  
  if (player->videoport != NULL) {
    kr_videoport_deactivate (player->videoport);
    kr_videoport_destroy (player->videoport);
  }
  if (player->audioport != NULL) {
    kr_audioport_deactivate (player->audioport);
    kr_audioport_destroy (player->audioport);
  }

  kr_client_destroy (&player->client);

  if (player->sws_converter != NULL) {
    sws_freeContext ( player->sws_converter );
  }
  free (player->station);

  libav_shutdown ();

  printf ("See you next time space cowboy!\n");

  exit (0);
}

void krad_player (char *station, char *input) {

  krad_player_t *player;
    
  player = calloc (1, sizeof(krad_player_t));

  printf ("Krad Player Starting for station %s and input %s\n",
          station, input);

  libav_init ();

  player->station = strdup (station);
  player->client = kr_client_create ("Krad Player");

  if (player->client == NULL) {
    fprintf (stderr, "Krad Player: Could create client\n");
    exit (1);
  }

  if (!kr_connect (player->client, player->station)) {
    fprintf (stderr, "Krad Player: Could not connect to %s krad radio daemon\n",
             player->station);
    kr_client_destroy (&player->client);
    exit (1);
  }
  printf ("Krad Player Connected to %s!\n", player->station);
  
  /*
  //kr_compositor_info (client);
  player->videoport = kr_videoport_create (player->client);
  
  if (player->videoport == NULL) {
    fprintf (stderr, "could not setup videoport\n");
    exit (1);
  }

  kr_videoport_set_callback (player->videoport, videoport_process, player);

  //kr_videoport_activate (player->videoport);

  */
  
  player->audioport = kr_audioport_create (player->client, INPUT);

  if (player->audioport == NULL) {
    fprintf (stderr, "Krad Player: could not setup audioport\n");
    exit (1);
  }  
  
  kr_audioport_set_callback (player->audioport, audioport_process, player);
  
  krad_player_open (player, input);
  krad_player_destroy (player);
}

int main (int argc, char **argv) {

  if ((!argv[1]) || (!argv[2])) {
    fprintf(stderr, "Krad Player: [station] [url]\n");
    exit(1);
  }

  krad_player (argv[1], argv[2]);

  return 1;
}
