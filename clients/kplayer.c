#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*
#include <libavutil/time.h>
#include <libavutil/frame.h>
*/
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavresample/avresample.h>

#include "kr_client.h"

#define WIDTH 960
#define HEIGHT 540
#define FSIZE WIDTH * HEIGHT * 4
#define FBUFSIZE 120

static int destroy = 0;

typedef struct krad_player_St krad_player_t;
typedef struct krad_player_St kplayer_t;
typedef struct krad_libav_St krad_libav_t;

struct krad_libav_St {
  AVFormatContext *ctx;
  AVAudioResampleContext *avr;
  struct SwsContext *scaler;
  int aid;
  int vid;
  AVCodecContext* adec;
  AVCodecContext* vdec;
  AVCodec* acodec;
  AVCodec* vcodec;
  AVFrame *aframe;
  AVFrame *vframe;
  int video_frames;
  int video_packets;
  int audio_packets;
};

struct krad_player_St {
  kr_client_t *client;
  kr_videoport_t *videoport;
  kr_audioport_t *audioport;
  char *station;
  uint64_t samples;
  float timecode;
  float ms;
  unsigned char rgba[FSIZE * FBUFSIZE];
  int consumed;
  int produced;
  float audio0[8192];
  float audio1[8192];
  int samples_buffered;
  krad_libav_t avc;
};

void signal_recv (int sig) {
  destroy = 1;
}

void krad_player_close (krad_player_t *player) {
  if (player->videoport) {
    kr_videoport_deactivate (player->videoport);
  }
  if (player->audioport) {
    kr_audioport_deactivate (player->audioport);
  }
  if (player->avc.aframe != NULL) {  
    av_frame_free (&player->avc.aframe);
  }
  if (player->avc.vframe != NULL) {
    av_frame_free (&player->avc.vframe);
  }
  if (player->avc.ctx != NULL) {
    avformat_close_input (&player->avc.ctx);
  }
  if (player->avc.avr != NULL) {
    avresample_free (&player->avc.avr);
  }
  if (player->avc.scaler != NULL) {
    sws_freeContext ( player->avc.scaler );
  }
}

void handle_metadata (krad_player_t *player) {
  if (0) {
    AVDictionary* metadata = player->avc.ctx->metadata;
    AVDictionaryEntry *artist = av_dict_get (metadata, "artist", NULL, 0);
    AVDictionaryEntry *title = av_dict_get (metadata, "title", NULL, 0);
    fprintf (stdout, "Playing: %s - %s\n", artist->value, title->value);
  }
}

int decode_video (krad_player_t *player, AVPacket *packet) {

  int got_frame;
  int err;
  
  err = 0;
  got_frame = 0;
  
  player->avc.video_packets++;

  err = avcodec_decode_video2 (player->avc.vdec,
                               player->avc.vframe,
                               &got_frame,
                               packet);

  av_free_packet (packet);

  if (err < 0) {
    fprintf (stderr, "\nKrad Player: Error decoding video\n");
    return -1;
  }
  
  if (!got_frame) {
    printf ("\nKrad Player: Got No frame from video decoder\n");
    return -1;
  }
  
  player->avc.video_frames++;  

  int rgb_stride_arr[3] = {4*WIDTH, 0, 0};
  uint8_t *dst[4];
  
  player->avc.scaler = sws_getCachedContext ( player->avc.scaler,
                                              player->avc.vframe->width,
                                              player->avc.vframe->height,
                                              player->avc.vframe->format,
                                              WIDTH,
                                              HEIGHT,
                                              PIX_FMT_RGB32, 
                                              SWS_BICUBIC,
                                              NULL, NULL, NULL);
  //av_frame_unref (player->avc.vframe);  

  /*
    printf ("last frame ms: %f kplayer ms: %f", pts, player->ms);

    if (pts < player->ms) {
      return NULL;
    }

    while (pts > player->ms + 0.040) {
      usleep(2000);
    }

  */

  while (player->produced - player->consumed == FBUFSIZE) {
    usleep (100000);
  }

  int pos = (player->produced % FBUFSIZE) * FSIZE;

  dst[0] = (unsigned char *)player->rgba + pos;
  
  
  sws_scale (player->avc.scaler,
             (const uint8_t * const*)player->avc.vframe->data,
             player->avc.vframe->linesize,
             0, player->avc.vframe->height,
             dst, rgb_stride_arr);


  player->produced++;

  //player->timecode = pts;
  
  return 0;
}

int decode_audio (krad_player_t *player, AVPacket *packet) {

  int err;
  int got_frame;

  player->avc.audio_packets++;

  err = avcodec_decode_audio4 (player->avc.adec,
                               player->avc.aframe,
                               &got_frame,
                               packet);
  if (err < 0) {
    fprintf (stderr, "Krad Player: Error decoding audio\n");
    return -1;
  }

  if (player->avc.avr == NULL) {
    player->avc.avr = avresample_alloc_context ();
    av_opt_set_int(player->avc.avr, "in_channel_layout", player->avc.aframe->channel_layout, 0);
    av_opt_set_int(player->avc.avr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(player->avc.avr, "in_sample_rate", player->avc.aframe->sample_rate, 0);
    av_opt_set_int(player->avc.avr, "out_sample_rate", 48000, 0);
    av_opt_set_int(player->avc.avr, "in_sample_fmt", player->avc.aframe->format, 0);
    av_opt_set_int(player->avc.avr, "out_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);
    avresample_open (player->avc.avr);
  }
  av_free_packet (packet);

  if (!got_frame) {
    printf ("Krad Player: Got to EOF from audio decoder\n");
    return -1;
  }

  while ((avresample_available(player->avc.avr) >= 48000 * 4) && 
        (!destroy)) {
    usleep (5000);
  }

  if (destroy == 1) {
    return -1;
  }

  player->samples_buffered += avresample_convert (player->avc.avr,
                                                  NULL, 0, 0,
                                                  player->avc.aframe->data,
                                                  player->avc.aframe->linesize[0],
                                                  player->avc.aframe->nb_samples);

  //av_frame_unref (player->avc.aframe);

  return 1;
}

void krad_player_open (krad_player_t *player, char *input) {

  int i;
  int err;
  AVPacket packet;
  
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

  player->avc.vid = -1;
  player->avc.aid = -1;

  for (i = 0; i < player->avc.ctx->nb_streams; i++) {
    if ((player->avc.vid == -1) && 
        (player->avc.ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)) {
      player->avc.vid = i;
      player->avc.vframe = av_frame_alloc();
      printf ("Got a Video Track\n");
    }
    if ((player->avc.aid == -1) && 
        (player->avc.ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)) {
      player->avc.aid = i;
      player->avc.aframe = av_frame_alloc();
      printf ("Got an Audio Track\n");
     }
   }
 
  if ((player->avc.aid == -1) && (player->avc.vid == -1)) {
    fprintf (stderr, "Krad Player: Could get info on input: %s\n", input);
    return;
  }

  handle_metadata (player); 

  if (player->avc.aid != -1) {

    player->avc.adec = player->avc.ctx->streams[player->avc.aid]->codec;
    player->avc.acodec = avcodec_find_decoder (player->avc.adec->codec_id);

    if (!avcodec_open2 (player->avc.adec, player->avc.acodec, NULL) < 0) {
      fprintf (stderr, "Krad Player: Could not find open the needed codec");
    }
    
    kr_audioport_activate (player->audioport);
  }

  if (player->avc.vid != -1) {

    player->avc.vdec = player->avc.ctx->streams[player->avc.vid]->codec;
    player->avc.vcodec = avcodec_find_decoder (player->avc.vdec->codec_id);

    if (!avcodec_open2 (player->avc.vdec, player->avc.vcodec, NULL) < 0) {
      fprintf (stderr, "Krad Player: Could not find open the needed codec");
    }
    
    kr_videoport_activate (player->videoport);
  }

  while (!destroy) {
    if (av_read_frame (player->avc.ctx, &packet) < 0) {
      printf ("Got to EOF\n");
      break;
    }

    if ((packet.stream_index != player->avc.aid) && 
        (packet.stream_index != player->avc.vid)) {
      av_free_packet (&packet);
      continue;
    }

    if (packet.stream_index == player->avc.vid) {
      decode_video (player, &packet);
    } else {
      decode_audio (player, &packet);
    }

    printf ("Krad Player: Playback position: %3.2fs - %d VPKTS %d APKTS %d VFRMS\r",
            (float)player->samples / 48000.0f, player->avc.video_packets,
            player->avc.audio_packets, player->avc.video_frames);
    fflush (stdout);
  }
}

int videoport_process (void *buffer, void *arg) {

  krad_player_t *player;
  player = (krad_player_t *)arg;
  
  if (player->produced > player->consumed) {
    int pos = (player->consumed % FBUFSIZE) * FSIZE;
    memcpy (buffer, player->rgba + pos, WIDTH * HEIGHT * 4);
    player->consumed++;
  }

  return 0;
}

int audioport_process (uint32_t nframes, void *arg) {

  krad_player_t *player;

  uint8_t *outputs[2];

  player = (krad_player_t *)arg;

  if (player->avc.avr == NULL) {
    return 0;
  }

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
  
  //kr_compositor_info (client);
  player->videoport = kr_videoport_create (player->client);
  
  if (player->videoport == NULL) {
    fprintf (stderr, "could not setup videoport\n");
    exit (1);
  }

  kr_videoport_set_callback (player->videoport, videoport_process, player);
  
  player->audioport = kr_audioport_create (player->client, INPUT);

  if (player->audioport == NULL) {
    fprintf (stderr, "Krad Player: could not setup audioport\n");
    exit (1);
  }  
  
  kr_audioport_set_callback (player->audioport, audioport_process, player);
  
  signal (SIGINT, signal_recv);
  
  krad_player_open (player, input);
  krad_player_close (player);

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
