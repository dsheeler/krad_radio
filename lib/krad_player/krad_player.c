#include "krad_player.h"
#include "kr_client.h"

typedef struct kr_player_msg kr_player_msg;
typedef struct kr_player_demuxer_msg kr_player_demuxer_msg;
typedef struct kr_player_user_msg kr_player_user_msg;

static void kr_player_start (void *actual);
static int32 kr_player_process (void *msgin, void *actual);
static void kr_player_destroy_actual (void *actual);
static void kr_player_station_connect(kr_player *player);

typedef enum {
  THEUSER,
  THEDEMUXER
} kr_player_msg_from;

struct kr_player_demuxer_msg {
  kr_demuxer_state state;
};

struct kr_player_user_msg {
  kr_player_cmd cmd;
  union {
    float real;
    int64 integer;
  } param;
};

struct kr_player_msg {
  kr_player_msg_from from;
  union {
    kr_player_user_msg user;
    kr_player_demuxer_msg demuxer;
  } actual;
};

struct kr_player {
  char *url;
  float speed;
  int64 position;
  kr_player_playback_state state;
  kr_direction direction;

  kr_player_playback_state last_state;
  int64 rel_ms;
  kradimer *timer;

  kr_demuxer_state demuxer_state;

  kr_machine *machine;
  kr_demuxer *demuxer;
  kr_decoder *decoder;

  char *station;
  kr_client *client;
  kr_videoport *videoport;
  kr_audioport *audioport;
  krad_resample_ring *resample_ring[2];
  uint32 sample_rate;
  int channels;

  struct SwsContext *scaler;
  krad_vhs *kvhs;
  krad_vpx_decoder *vpx;
  kradheora_decoder *theora;
  krad_flac *flac;
  krad_opus *opus;
  krad_vorbis *vorbis;

  float *samples[8];
  int64 ms;
  uint32 fps_den;
  uint32 fps_num;
  uint32 width;
  uint32 height;
  uint32 frame_size;
  uint32 framebufsize;
  uint8 *rgba;
  int64 *frameime;
  int64 last_frameime;
  uint32 repeated;
  uint32 skipped;
  uint32 consumed;
  uint32 frames_dec;
  uint32 samples_consumed;
};

/* Private Functions */

int krad_player_get_frame(kr_player *player, void *frame) {

  int pos;

  if (player->state == PLAYING) {
    if (player->last_state != PLAYING) {
      player->rel_ms = 0;
      kradimer_start(player->timer);
    } else {
      player->rel_ms = kradimer_current_ms(player->timer);
    }

    if (player->frames_dec > player->consumed) {
      pos = (player->consumed % player->framebufsize);
      player->ms += 1600;
      //if (player->ms >= player->frameime[pos]) {
     //   player->last_frameime = player->frameime[pos];
        memcpy (frame,
                player->rgba + (pos * player->frame_size),
                player->width * player->height * 4);
        player->consumed++;
      //} else {
      //  player->repeated++;
      //}
      return 1;
    }
  }

  return 0;
}

int videoport_process (void *buffer, void *arg) {

  kr_player *player;
  player =(kr_player *)arg;

  int pos;

  if (player->state == PLAYING) {
    if (player->frames_dec > player->consumed) {
      pos = (player->consumed % player->framebufsize);
      //if (player->ms >= player->frameime[pos]) {
     //   player->last_frameime = player->frameime[pos];
        memcpy (buffer,
                player->rgba + (pos * player->frame_size),
                player->width * player->height * 4);
        player->consumed++;
      //} else {
      //  player->repeated++;
      //}
    }
  }

  return 0;
}

int audioport_process (uint32 nframes, void *arg) {

  kr_player *player;

  uint8 *output[2];

  player =(kr_player *)arg;

  output[0] = (uint8 *)kr_audioport_get_buffer (player->audioport, 0);
  output[1] = (uint8 *)kr_audioport_get_buffer (player->audioport, 1);

  if ((player->state == PLAYING) &&
      ((krad_resample_ring_read_space (player->resample_ring[0]) >= nframes * 4) &&
      (krad_resample_ring_read_space (player->resample_ring[1]) >= nframes * 4))) {

    krad_resample_ring_read (player->resample_ring[0], output[0], nframes * 4);
    krad_resample_ring_read (player->resample_ring[1], output[1], nframes * 4);

    player->samples_consumed += nframes;
    player->ms = (float)player->samples_consumed /
                 (float)player->sample_rate * 1000;
  } else {
    memset(output[0], '0', nframes * 4);
    memset(output[1], '0', nframes * 4);
  }

  return 0;
}

static int32 kr_player_process (void *msgin, void *actual) {

  kr_player *player;
  kr_player_msg *msg;

  msg =(kr_player_msg *)msgin;
  player =(kr_player *)actual;

  if (msg->from == THEDEMUXER) {
    printf ("Got new DEMUXER state! %d - %d\n",
            player->demuxer_state, msg->actual.demuxer.state);
    player->demuxer_state = msg->actual.demuxer.state;
    if ((player->state == IDLE) && (player->demuxer_state == DMCUED)) {
      player->state = CUED;
    }
    if ((player->state == PLAYERDESTROYING) &&
        (player->demuxer_state == DMCUED)) {
      return 0;
    }
    return 1;
  } else {
    switch (msg->actual.user.cmd) {
      /*
      case TICKLE:
        printf ("Got TICKLE command!\n");

        break;
      */
      case PLAYERDESTROY:
        printf ("Got PLAYERDESTROY command!\n");
        if (player->state == PLAYING) {
          kr_demuxer_pause (player->demuxer);
          player->state = PLAYERDESTROYING;
        } else {
          return 0;
        }
        break;
      case PAUSE:
        printf ("Got PAUSE command!\n");
        if (player->state == PLAYING) {
          kr_demuxer_pause (player->demuxer);
          player->state = CUED;
        }
        break;
      /*
      case STOP:
        printf ("Got STOP command!\n");
        //if (player->input != NULL) {
        //  player->state = CUED;
        //}
        break;
      */
      case PLAY:
        printf ("Got PLAY command!\n");
        if (player->state == CUED) {
          kr_demuxer_roll (player->demuxer);
          player->state = PLAYING;
        }
        break;
      case SEEK:
        printf ("Got SEEK %"PRIi64" command!\n", msg->actual.user.param.integer);
        break;
      case SETSPEED:
        printf ("Got SETSPEED %0.3f command!\n", msg->actual.user.param.real);
        player->speed = msg->actual.user.param.real;
        break;
      case SETDIR:
        printf ("Got SETDIR %"PRIi64" command!\n", msg->actual.user.param.integer);
        player->direction = msg->actual.user.param.integer;
        break;
    }
  }
  return 1;
}

int krad_player_check_av_ports(kr_player *player) {
  if ((player->audioport == NULL) && (player->videoport == NULL)) {
    return -1;
  }

  if (player->videoport != NULL) {
    if(kr_videoport_error(player->videoport)) {
      printf ("\r\nError: %s\n", "Videoport Error");
      return 1;
    }
  }
  if (player->audioport != NULL) {
    if(kr_audioport_error(player->audioport)) {
      printf ("\r\nError: %s\n", "Audioport Error");
      return 2;
    }
  }
  return 0;
}

static int kr_player_process_codeme(kr_codeme *codeme, void *actual) {

  kr_player *player;
  uint32 pos;
  player =(kr_player *)actual;

  printf ("Codeme sized %zu track %d!\n", codeme->sz, codeme->trk);

  if (codeme->trk == 1) {

    krad_vpx_decoder_decode (player->vpx, codeme->data, codeme->sz);

    while (player->vpx->img != NULL) {

      if ((player->state == CUED) || (player->state == PLAYERDESTROYING)) {
        return 0;
      }

      if (player->frames_dec - player->consumed + 5 >= player->framebufsize) {
        usleep (100000);
        if (player->station != NULL) {
          if (krad_player_check_av_ports (player)) {
            return -3;
          }
        }
        continue;
      }

      int rgb_stride_arr[3] = {4*player->width, 0, 0};
      uint8 *dst[4];

      player->scaler = sws_getCachedContext ( player->scaler,
                                              player->vpx->width,
                                              player->vpx->height,
                                              PIX_FMT_YUV420P,
                                              player->width,
                                              player->height,
                                              PIX_FMT_RGB32,
                                              SWS_BICUBIC,
                                              NULL, NULL, NULL);

      pos = (player->frames_dec % player->framebufsize) * player->frame_size;
      dst[0] = player->rgba + pos;

      sws_scale (player->scaler,
                 (const uint8 * const*)player->vpx->img->planes,
                 player->vpx->img->stride,
                 0, player->vpx->height,
                 dst, rgb_stride_arr);


      player->frames_dec++;
      krad_vpx_decoder_decode_again (player->vpx);
    }

    if (0) {
      pos = (player->frames_dec + 1 % player->framebufsize) * player->frame_size;

      krad_vhs_decode (player->kvhs,
                       codeme->data,
                       player->rgba + pos);
      player->frames_dec++;
    }
    /*
    player->theora = kradheora_decoder_create (krad_link->vu_header[0], krad_link->vu_header_len[0],
                                                 krad_link->vu_header[1], krad_link->vu_header_len[1],
                                                 krad_link->vu_header[2], krad_link->vu_header_len[2]);
    if (krad_link->video_codec == THEORA) {
      kradheora_decoder_decode (krad_link->kradheora_decoder, kr_slice->data, kr_slice->size);
      kradheora_decoderimecode (krad_link->kradheora_decoder, &timecode2);
      //printk ("timecode1: %zu timecode2: %zu", timecode, timecode2);
      timecode = timecode2;

      krad_frame->format = krad_link->kradheora_decoder->color_depth;

      krad_frame->yuv_pixels[0] = krad_link->kradheora_decoder->ycbcr[0].data + (krad_link->kradheora_decoder->offset_y * krad_link->kradheora_decoder->ycbcr[0].stride);
      krad_frame->yuv_pixels[1] = krad_link->kradheora_decoder->ycbcr[1].data + (krad_link->kradheora_decoder->offset_y * krad_link->kradheora_decoder->ycbcr[1].stride);
      krad_frame->yuv_pixels[2] = krad_link->kradheora_decoder->ycbcr[2].data + (krad_link->kradheora_decoder->offset_y * krad_link->kradheora_decoder->ycbcr[2].stride);

      krad_frame->yuv_strides[0] = krad_link->kradheora_decoder->ycbcr[0].stride;
      krad_frame->yuv_strides[1] = krad_link->kradheora_decoder->ycbcr[1].stride;
      krad_frame->yuv_strides[2] = krad_link->kradheora_decoder->ycbcr[2].stride;
      krad_frame->timecode = timecode;
      krad_compositor_port_push_yuv_frame (krad_link->krad_compositor_port, krad_frame);
    }
    kradheora_decoder_destroy (player->theora);
    */
  }

  if (codeme->trk == 2) {
    /*
    int ret;
    ret = krad_flac_decode (player->flac, codeme->data,
                            codeme->sz, player->samples);
    printf ("flac decode ret is %d\n", ret);

    while ((krad_resample_ring_write_space (player->resample_ring[0]) <= ret * 4) ||
           (krad_resample_ring_write_space (player->resample_ring[1]) <= ret * 4)) {
      usleep (6000);
      if ((player->state == CUED) || (player->state == PLAYERDESTROYING)) {
        return 0;
      }
    }

    krad_resample_ring_write (player->resample_ring[0], (uint8 *)player->samples[0], ret * 4);
    krad_resample_ring_write (player->resample_ring[1], (uint8 *)player->samples[1], ret * 4);

    if ((player->state == CUED) || (player->state == PLAYERDESTROYING)) {
      return 0;
    }
    if (ret > 0) {
      return 1;
    } else {
      return 0;
    }
    */
  }
  return 1;
}

static void kr_player_demuxer_status(kr_demuxer_state state, void *actual) {

  kr_player *player;

  player =(kr_player *)actual;

  kr_player_msg msg;
  msg.from = THEDEMUXER;
  msg.actual.demuxer.state = state;
  krad_machine_msg2 (player->machine, &msg);
}

static void kr_player_destroy_actual (void *actual) {

  printf ("kr_player_destroy_actual()!\n");
  int c;
  kr_player *player;

  player =(kr_player *)actual;

  if (player->station != NULL) {
    kr_videoport_destroy (player->videoport);
    kr_audioport_destroy (player->audioport);
    kr_client_destroy (&player->client);
    if (player->station != NULL) {
      free (player->station);
      player->station = NULL;
    }
  }

  for (c = 0; c < player->channels; c++) {
    krad_resample_ring_destroy (player->resample_ring[c]);
    free (player->samples[c]);
  }

  //kr_decoder_destroy (&player->decoder);
  kr_demuxer_destroy (&player->demuxer);

  krad_vhs_destroy (player->kvhs);
  krad_flac_decoder_destroy (player->flac);
  krad_vpx_decoder_destroy (&player->vpx);

  sws_freeContext (player->scaler);

  free (player->url);
}

void krad_player_free_framebuf(kr_player *player) {
  if (player->rgba != NULL) {
    free (player->rgba);
    free (player->frameime);
    player->rgba = NULL;
    player->frameime = NULL;
  }
}

void krad_player_alloc_framebuf(kr_player *player) {
  player->framebufsize = 120;
  player->frame_size = player->width * player->height * 4;
  player->rgba = malloc (player->frame_size * player->framebufsize);
  player->frameime = calloc (player->framebufsize, sizeof(int64));
}

static void kr_player_station_connect(kr_player *player) {

  player->client = kr_client_create ("Krad player");

  if (player->client == NULL) {
    fprintf (stderr, "Krad player: Could create client\n");
    exit (1);
  }

  if (!kr_connect (player->client, player->station)) {
    fprintf (stderr, "Krad player: Could not connect to %s krad radio daemon\n",
             player->station);
    kr_client_destroy (&player->client);
    exit (1);
  }
  printf ("Krad player Connected to %s!\n", player->station);

  if(kr_mixer_get_info_wait (player->client,
                              &player->sample_rate, NULL) != 1) {
    fprintf (stderr, "Krad player: Could not get mixer info!\n");
    kr_client_destroy (&player->client);
    exit (1);
  }

  if(kr_compositor_get_info_wait (player->client,
                                   &player->width, &player->height,
                                   &player->fps_num, &player->fps_den) != 1) {
    fprintf (stderr, "Krad Player: Could not get compositor info!\n");
    kr_client_destroy (&player->client);
    exit (1);
  }

  player->audioport = kr_audioport_create (player->client, "krad player", KR_MXR_INPUT);
  if (player->audioport == NULL) {
    fprintf (stderr, "Krad player: could not setup audioport\n");
    exit (1);
  }
  kr_audioport_set_callback (player->audioport, audioport_process, player);
  kr_audioport_connect(player->audioport);
  //FIXME
  player->videoport = kr_videoport_create (player->client, 0);
  if (player->videoport == NULL) {
    fprintf (stderr, "could not setup videoport\n");
    exit (1);
  }

  kr_videoport_set_callback (player->videoport, videoport_process, player);
  kr_videoport_activate (player->videoport);
}

static void kr_player_start (void *actual) {

  kr_player *player;
  int c;

  player =(kr_player *)actual;

  player->direction = FORWARD;
  player->speed = 100.0f;
  player->state = IDLE;

  player->channels = 2;

  krad_player_alloc_framebuf (player);

  for (c = 0; c < player->channels; c++) {
    player->resample_ring[c] = krad_resample_ring_create (1600000, 48000,
                                                          player->sample_rate);
    player->samples[c] = malloc(4 * 8192);
  }

  if (player->station != NULL) {
    kr_player_station_connect(player);
  }

  player->kvhs = krad_vhs_create_decoder ();
  player->flac = krad_flac_decoder_create (NULL);
  player->vpx = krad_vpx_decoder_create ();

  kr_demuxer_params demuxer_params;

  demuxer_params.url = player->url;
  demuxer_params.controller = player;
  demuxer_params.status_cb = kr_player_demuxer_status;
  demuxer_params.codeme_cb = kr_player_process_codeme;
  player->demuxer = kr_demuxer_create (&demuxer_params);
  //player->decoder = kr_decoder_create ();

  printf ("kr_player_start()!\n");
}

/* Public Functions */

void kr_player_destroy(kr_player **player) {
  kr_player_msg msg;
  if ((player != NULL) && (*player != NULL)) {
    printf ("kr_player_destroy()!\n");
    msg.from = THEUSER;
    msg.actual.user.cmd = PLAYERDESTROY;
    krad_machine_msg ((*player)->machine, &msg);
    krad_machine_destroy (&(*player)->machine);
    krad_player_free_framebuf (*player);
    free (*player);
    *player = NULL;
  }
}

kr_player *kr_player_create(char *station, char *url) {

  kr_player *player;
  kr_machine_params machine_params;

  player = calloc (1, sizeof(kr_player));
  player->url = strdup (url);
  if (station == NULL) {
    player->station = NULL;
  } else {
    player->station = strdup (station);
  }

  machine_params.actual = player;
  machine_params.msg_sz = sizeof(kr_player_msg);
  machine_params.start = kr_player_start;
  machine_params.process = kr_player_process;
  machine_params.destroy = kr_player_destroy_actual;

  player->machine = krad_machine_create (&machine_params);

  return player;
};

kr_player *kr_player_create_custom_cb(char *url) {

  kr_player *player;

  player = kr_player_create(NULL, url);

  if (player == NULL) {
    return NULL;
  }

  player->width = 1280;
  player->height = 720;

  return player;
}

char *kr_player_playback_stateo_string(kr_player_playback_state state) {
  switch (state) {
    case IDLE:
      return "IDLE";
    case CUED:
      return "CUED";
    case PLAYING:
      return "PLAYING";
    case PLAYERDESTROYING:
      return "PLAYERDESTROYING";
    //case PAUSING:
    //  return "PAUSING";
    //case RESUMING:
    //  return "RESUMING";
    //case LOOPING:
    //  return "LOOPING";
  }
  return "Unknown";
}

char *kr_directiono_string(kr_direction direction) {
  switch (direction) {
    case FORWARD:
      return "Forward";
    case REVERSE:
      return "Reverse";
  }
  return "Unknown";
}

float kr_player_speed_get(kr_player *player) {
  return player->speed;
}

kr_direction kr_player_direction_get(kr_player *player) {
  return player->direction;
}

int64 kr_player_position_get(kr_player *player) {
  return player->position;
}

kr_player_playback_state kr_player_playback_state_get(kr_player *player) {
  return player->state;
}

void kr_player_speed_set(kr_player *player, float speed) {
  kr_player_msg msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = SETSPEED;
  msg.actual.user.param.real = roundf (speed*1000.0f)/1000.0f;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_direction_set(kr_player *player, kr_direction direction) {
  kr_player_msg msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = SETDIR;
  msg.actual.user.param.integer = direction;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_seek(kr_player *player, int64 position) {
  kr_player_msg msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = SEEK;
  msg.actual.user.param.integer = position;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_play(kr_player *player) {
  kr_player_msg msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = PLAY;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_pause(kr_player *player) {
  kr_player_msg msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = PAUSE;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_stop(kr_player *player) {
//  kr_player_msg msg;
//  msg.from = THEUSER;
//  msg.actual.user.cmd = STOP;
//  krad_machine_msg (player->machine, &msg);
}
