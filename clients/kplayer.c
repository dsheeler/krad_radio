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

#define DEFAULT_FBUFSIZE 120

static int destroy = 0;

typedef struct krad_player_St krad_player_t;
typedef struct krad_player_St kplayer_t;
typedef struct krad_libav_St krad_libav_t;

struct krad_libav_St {
  AVFormatContext *ctx;
  AVAudioResampleContext *avr;
  struct SwsContext *scaler;
  int32_t aid;
  int32_t vid;
  AVCodecContext* adec;
  AVCodecContext* vdec;
  AVCodec* acodec;
  AVCodec* vcodec;
  AVFrame *aframe;
  AVFrame *vframe;
  uint32_t video_frames;
  uint32_t video_packets;
  uint32_t audio_packets;
  char *audio_err;
  char *video_err;
};

struct krad_player_St {
  kr_client_t *client;
  kr_videoport_t *videoport;
  kr_audioport_t *audioport;
  char *station;
  int64_t samples;
  float timecode;
  int64_t ms;
  uint32_t sample_rate;
  uint32_t fps_den;
  uint32_t fps_num;
  uint32_t width;
  uint32_t height;
  int32_t seek;
  uint32_t frame_size;
  uint32_t framebufsize;
  unsigned char *rgba;
  int64_t *frame_time;
  int64_t last_frame_time;
  uint32_t repeated;
  uint32_t skipped;
  uint32_t consumed;
  uint32_t frames_dec;
  float audio0[8192];
  float audio1[8192];
  uint32_t samples_dec;
  uint32_t samples_consumed;
  krad_libav_t avc;
};

void signal_recv (int sig) {
  destroy = 1;
}

void krad_player_free_framebuf (krad_player_t *player) {
  if (player->rgba != NULL) {
    free (player->rgba);
    free (player->frame_time);
    player->rgba = NULL;
    player->frame_time = NULL;
  }
}

void krad_player_alloc_framebuf (krad_player_t *player) {
  player->framebufsize = DEFAULT_FBUFSIZE;
  player->frame_size = player->width * player->height * 4;
  player->rgba = malloc (player->frame_size * player->framebufsize);
  player->frame_time = calloc (player->framebufsize, sizeof(int64_t));
}

int krad_player_check_av_ports (krad_player_t *player) {
  if ((player->audioport == NULL) && (player->videoport == NULL)) {
    return -1;
  }

  if (player->videoport != NULL) {
    if (kr_videoport_error(player->videoport)) {
      printf ("\r\nError: %s\n", "Videoport Error");
      return 1;
    }
  }
  if (player->audioport != NULL) {
    if (kr_audioport_error(player->audioport)) {
      printf ("\r\nError: %s\n", "Audioport Error");
      return 2;
    }
  }
  return 0;
}

void krad_player_close (krad_player_t *player) {
  if (player->videoport) {
    kr_videoport_deactivate (player->videoport);
  }
  if (player->audioport) {
    kr_audioport_disconnect(player->audioport);
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

int decode_video(krad_player_t *player, AVPacket *packet) {

  int got_frame;
  int err;
  
  err = 0;
  got_frame = 0;
  
  player->avc.video_packets++;
  if (player->seek) avcodec_flush_buffers(player->avc.vdec);
  err = avcodec_decode_video2 (player->avc.vdec,
                               player->avc.vframe,
                               &got_frame,
                               packet);

  av_free_packet (packet);

  if (err < 0) {
    player->avc.video_err = "Error decoding video";
    return -2;
  }
  
  if (!got_frame) {
    player->avc.video_err = "Got No frame from video decoder";
    return -1;
  }
  
  player->avc.video_frames++;  

  int rgb_stride_arr[3] = {4*player->width, 0, 0};
  uint8_t *dst[4];
  
  player->avc.scaler = sws_getCachedContext ( player->avc.scaler,
                                              player->avc.vframe->width,
                                              player->avc.vframe->height,
                                              player->avc.vframe->format,
                                              player->width,
                                              player->height,
                                              PIX_FMT_RGB32, 
                                              SWS_BICUBIC,
                                              NULL, NULL, NULL);
  //av_frame_unref (player->avc.vframe);  

  while (player->frames_dec - player->consumed == player->framebufsize) {
    usleep (100000);
    if (krad_player_check_av_ports (player)) {
      return -3;
    }
  }
  
  if (destroy == 1) {
    return 0;
  }  

  int pos = (player->frames_dec % player->framebufsize) * player->frame_size;
  dst[0] = (unsigned char *)player->rgba + pos;

  sws_scale (player->avc.scaler,
             (const uint8_t * const*)player->avc.vframe->data,
             player->avc.vframe->linesize,
             0, player->avc.vframe->height,
             dst, rgb_stride_arr);

  player->frame_time[player->frames_dec % player->framebufsize] = 
    av_q2d (player->avc.ctx->streams[player->avc.vid]->time_base) *
            (player->avc.vframe->pkt_pts - player->avc.ctx->streams[player->avc.vid]->start_time) * 1000;
  
  player->frames_dec++;

  return 1;
}

int decode_audio(krad_player_t *player, AVPacket *packet) {

  int err;
  int got_frame;

  player->avc.audio_packets++;
  if (player->seek) avcodec_flush_buffers(player->avc.adec);
  err = avcodec_decode_audio4 (player->avc.adec,
                               player->avc.aframe,
                               &got_frame,
                               packet);
  if (err < 0) {
    player->avc.audio_err = "Decoding audio";
    return -2;
  }

  if (!got_frame) {
    player->avc.audio_err = "Got not audio from decoder";
    return -1;
  }

  if (player->avc.avr == NULL) {
    player->avc.avr = avresample_alloc_context ();
    av_opt_set_int (player->avc.avr, "in_channel_layout",
                    player->avc.aframe->channel_layout, 0);
    av_opt_set_int (player->avc.avr, "out_channel_layout",
                    AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int (player->avc.avr, "in_sample_rate",
                    player->avc.aframe->sample_rate, 0);
    av_opt_set_int (player->avc.avr, "out_sample_rate",
                    player->sample_rate, 0);
    av_opt_set_int (player->avc.avr, "in_sample_fmt",
                    player->avc.aframe->format, 0);
    av_opt_set_int (player->avc.avr, "out_sample_fmt",
                    AV_SAMPLE_FMT_FLTP, 0);
    avresample_open (player->avc.avr);
  }
  av_free_packet (packet);

  while ((avresample_available(player->avc.avr) >= player->sample_rate * 4) && 
        (!destroy)) {
    usleep (5000);
    if (krad_player_check_av_ports (player)) {
      return -3;
    }    
  }

  if (destroy == 1) {
    return 0;
  }

  player->samples_dec += avresample_convert (player->avc.avr,
                                             NULL, 0, 0,
                                             player->avc.aframe->data,
                                             player->avc.aframe->linesize[0],
                                             player->avc.aframe->nb_samples);
  return 1;
}

int videoport_process(void *buffer, void *arg) {

  krad_player_t *player;
  int pos;
  int npos;
  player = (krad_player_t *)arg;
  
  if (player->avc.aid == -1) {
    player->samples_consumed += 1600;
    player->ms = player->samples_consumed / 48;
  }
  
  if (player->frames_dec > player->consumed) {
    pos = (player->consumed % player->framebufsize);
    if (player->ms >= player->frame_time[pos]) {
      while (player->frames_dec > player->consumed + 2) {
        npos = ((player->consumed + 2) % player->framebufsize);
        if (player->ms > player->frame_time[npos]) {
          player->consumed++;
          player->skipped++;
          pos = (player->consumed % player->framebufsize);
        } else {
          break;
        }
      }
      player->last_frame_time = player->frame_time[pos];
      memcpy (buffer,
              player->rgba + (pos * player->frame_size),
              player->width * player->height * 4);
      player->consumed++;
    } else {
      player->repeated++;
    }
  }

  return 0;
}

int audioport_process(uint32_t nframes, void *arg) {

  krad_player_t *player;

  uint8_t *outputs[2];

  player = (krad_player_t *)arg;

  outputs[0] = (uint8_t *)kr_audioport_get_buffer (player->audioport, 0);
  outputs[1] = (uint8_t *)kr_audioport_get_buffer (player->audioport, 1);

  if ((player->avc.avr != NULL) && 
      (avresample_available(player->avc.avr) >= nframes)) {
    avresample_read (player->avc.avr, outputs, nframes);
    player->samples_consumed += nframes;
    player->ms = (float)player->samples_consumed /
                 (float)player->sample_rate * 1000;
  } else {
    memset (outputs[0], 0, nframes * 4);
    memset (outputs[0], 0, nframes * 4);
  }

  return 0;
}

int krad_player_open(krad_player_t *player, char *input) {

  int i;
  int pc;
  int err;
  int last_ms;
  int32_t ret;
  AVPacket packet;
  
  pc = 0;
  err = 0;
  last_ms = -1000;
  
  player->avc.ctx = avformat_alloc_context ();
  
  if (input[0] == '-') {
    input = "pipe:0";
  }
  
  err = avformat_open_input (&player->avc.ctx, input, NULL, NULL);
  if (err < 0) {
    fprintf (stderr, "Krad Player: Could not open input: %s\n", input);
    return -1;
  }
  err = avformat_find_stream_info (player->avc.ctx, NULL);
  if (err < 0) {
    fprintf (stderr, "Krad Player: Couldnt get info on input: %s\n", input);
    return -2;
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
    fprintf (stderr, "Krad Player: Couldnt get info on input: %s\n", input);
    return -3;
  }

  handle_metadata (player); 

  if (player->avc.aid != -1) {

    player->avc.adec = player->avc.ctx->streams[player->avc.aid]->codec;
    player->avc.acodec = avcodec_find_decoder (player->avc.adec->codec_id);

    if (!avcodec_open2 (player->avc.adec, player->avc.acodec, NULL) < 0) {
      fprintf (stderr, "Krad Player: Could not find open the needed codec\n");
    }
    
    player->audioport = kr_audioport_create (player->client, "kplayer", INPUT);

    if (player->audioport == NULL) {
      fprintf (stderr, "Krad Player: could not setup audioport\n");
      return -4;
    }  
    
    kr_audioport_set_callback(player->audioport, audioport_process, player);
    
    kr_audioport_connect(player->audioport);
  }

  if (player->avc.vid != -1) {

    player->avc.vdec = player->avc.ctx->streams[player->avc.vid]->codec;
    player->avc.vcodec = avcodec_find_decoder (player->avc.vdec->codec_id);

    if (!avcodec_open2 (player->avc.vdec, player->avc.vcodec, NULL) < 0) {
      fprintf (stderr, "Krad Player: Could not find open the needed codec\n");
    }
    
    player->videoport = kr_videoport_create (player->client, INPUT);
    
    if (player->videoport == NULL) {
      fprintf (stderr, "could not setup videoport\n");
      return -5;
    }

    krad_player_alloc_framebuf (player);

    kr_videoport_set_callback (player->videoport, videoport_process, player);
    
    kr_videoport_activate (player->videoport);
  }
  printf ("\n");

  if (krad_player_check_av_ports (player)) {
    printf ("Error: av ports error\n");
    return -6;    
  }  
  
  while (!destroy) {
    if (player->seek) {
      int64_t timestamp = rand() % 10000000;
      ret = avformat_seek_file(player->avc.ctx, -1, INT64_MIN, timestamp,
       INT64_MAX, 0);
      if (ret < 0) {
        fprintf(stderr, "%s: could not seek to position %0.3f\n",
         input, (double)timestamp / AV_TIME_BASE);
      } else {
        fprintf(stderr, "%s: seek to position %0.3f\n",
         input, (double)timestamp / AV_TIME_BASE);
      }
    }
    err = av_read_frame (player->avc.ctx, &packet);
    if (err < 0) {
      printf ("\nGot to EOF\n");
      break;
    }
    if ((packet.stream_index != player->avc.aid) && 
        (packet.stream_index != player->avc.vid)) {
      av_free_packet (&packet);
      continue;
    }
    if (packet.stream_index == player->avc.vid) {
      err = decode_video (player, &packet);
      if (err < -1) {
        printf ("\rError: %-*s\n", pc, player->avc.video_err);
        break;
      }
    } else {
      err = decode_audio (player, &packet);
      if (err < -1) {
        printf ("\rError: %-*s\n", pc, player->avc.audio_err);
        break;
      }
    }
    if (player->seek) player->seek = 0;

    if (krad_player_check_av_ports (player)) {
        printf ("\r\nError: %s\n", "Avports error");
      return -7;
    }     
    if (last_ms < (player->ms - 80)) {
      pc = printf ("\rVPOS: %"PRIi64" LFT: %"PRIi64" RPT: %d SKP: %d "
                   "VFRMS %d VPKTS %d :: APOS: %3.2fs APKTS %d",
                  player->ms, player->last_frame_time,
                  player->repeated, player->skipped,
                  player->avc.video_frames, player->avc.video_packets,
                  (float)player->samples_consumed / (float)player->sample_rate,
                  player->avc.audio_packets);
      fflush (stdout);
      last_ms = player->ms;
    }
  }
  printf ("\n");
  
  // Handling draining / fadeout here
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

void krad_player_destroy (krad_player_t **destroy_player) {

  krad_player_t *player;

  if ((destroy_player == NULL) || (*destroy_player == NULL)) {
    return;
  } else {
    player = *destroy_player;
    *destroy_player = NULL;
  }

  printf ("Krad Player: Self Destructing\n");
  
  if (player->videoport != NULL) {
    kr_videoport_deactivate (player->videoport);
    kr_videoport_destroy (player->videoport);
  }
  if (player->audioport != NULL) {
    kr_audioport_deactivate (player->audioport);
    kr_audioport_destroy (player->audioport);
  }

  krad_player_free_framebuf (player);
  kr_client_destroy (&player->client);
  free (player->station);
  free (player);
  libav_shutdown ();

  printf ("See you next time space cowboy!\n");

  exit (0);
}

krad_player_t *krad_player_create (char *station) {

  krad_player_t *player;

  player = calloc (1, sizeof(krad_player_t));

  libav_init ();

  player->station = strdup (station);
  player->client = kr_client_create ("Krad Player");

  if (player->client == NULL) {
    fprintf (stderr, "Krad Player: Could create client\n");
    return NULL;
  }

  if (!kr_connect (player->client, player->station)) {
    fprintf (stderr, "Krad Player: Could not connect to %s krad radio daemon\n",
             player->station);
    kr_client_destroy (&player->client);
    return NULL;
  }
  printf ("Krad Player Connected to %s!\n", player->station);
  
  if (kr_mixer_get_info_wait (player->client,
                              &player->sample_rate, NULL) != 1) {
    fprintf (stderr, "Krad Player: Could not get mixer info!\n");
	  kr_client_destroy (&player->client);
	  return NULL;
  }
  
  if (kr_compositor_get_info_wait (player->client,
                                   &player->width, &player->height,
                                   &player->fps_num, &player->fps_den) != 1) {
    fprintf (stderr, "Krad Player: Could not get compositor info!\n");
	  kr_client_destroy (&player->client);
	  return NULL;
  }
    
  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);  
  
  return player;
}

void krad_player (char *station, char *input) {

  krad_player_t *player;

  if ((station == NULL) || (input == NULL)) {
    return;
  }

  printf ("Krad Player Starting for station %s and input %s\n",
          station, input);

  player = krad_player_create (station);

  if (player != NULL) {
    krad_player_open (player, input);
    krad_player_close (player);
    krad_player_destroy (&player);
  }
}

int main (int argc, char **argv) {

  if ((!argv[1]) || (!argv[2])) {
    fprintf(stderr, "Krad Player: [station] [url]\n");
    exit(1);
  }

  krad_player (argv[1], argv[2]);

  return 1;
}
