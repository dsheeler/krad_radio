#include "krad_player.h"
#include "kr_client.h"

typedef struct kr_player_msg_St kr_player_msg_t;
typedef struct kr_player_demuxer_msg_St kr_player_demuxer_msg_t;
typedef struct kr_player_user_msg_St kr_player_user_msg_t;

static void kr_player_start (void *actual);
static int32_t kr_player_process (void *msgin, void *actual);
static void kr_player_destroy_actual (void *actual);

typedef enum {
  THEUSER,
  THEDEMUXER
} kr_player_msg_from_t;

struct kr_player_demuxer_msg_St {
  kr_demuxer_state_t state;
};

struct kr_player_user_msg_St {
  kr_player_cmd_t cmd;
  union {
    float real;
    int64_t integer;
  } param;
};

struct kr_player_msg_St {
  kr_player_msg_from_t from;
  union {
    kr_player_user_msg_t user;
    kr_player_demuxer_msg_t demuxer;
  } actual;
};

struct kr_player_St {
  char *url;
  float speed;
  int64_t position;
  kr_player_playback_state_t state;
  kr_direction_t direction;

  kr_demuxer_state_t demuxer_state;  

  kr_machine_t *machine;
  kr_demuxer_t *demuxer;
  kr_decoder_t *decoder;
  
  krad_flac_t *flac;
	float *samples[8];

  char *station;
  kr_client_t *client;
  kr_videoport_t *videoport;
  kr_audioport_t *audioport;
	krad_resample_ring_t *resample_ring[2];
  uint32_t sample_rate;
  int channels;
};

/* Private Functions */

int videoport_process (void *buffer, void *arg) {

  kr_player_t *player;
  player = (kr_player_t *)arg;
  
  if (player->state == PLAYING) {
  
  }
  
  return 0;
}

int audioport_process (uint32_t nframes, void *arg) {

  kr_player_t *player;

  uint8_t *output[2];

  player = (kr_player_t *)arg;

  output[0] = (uint8_t *)kr_audioport_get_buffer (player->audioport, 0);
  output[1] = (uint8_t *)kr_audioport_get_buffer (player->audioport, 1);

  if ((player->state == PLAYING) && 
      ((krad_resample_ring_read_space (player->resample_ring[0]) >= nframes * 4) && 
      (krad_resample_ring_read_space (player->resample_ring[1]) >= nframes * 4))) {
       krad_resample_ring_read (player->resample_ring[0], output[0], nframes * 4);
      krad_resample_ring_read (player->resample_ring[1], output[1], nframes * 4);
  } else {
    //printf ("Got ne\n!");
    //if (player->state == PLAYING) {
    //  kr_player_msg_t msg;
    //  msg.cmd = PLAY;
    //  kr_msgsys_write (player->msgsys, NEEDDATA, &msg);
    //}  
    memset(output[0], '0', nframes * 4);
    memset(output[1], '0', nframes * 4);
  }

  return 0;
}

static int32_t kr_player_process (void *msgin, void *actual) {

  kr_player_t *player;
  kr_player_msg_t *msg;

  msg = (kr_player_msg_t *)msgin;
  player = (kr_player_t *)actual;

  //printf ("kr_player_process cmd %d\n", msg->cmd);

  if (msg->from == THEDEMUXER) {
    printf ("Got new DEMUXER state! %d - %d\n",
            player->demuxer_state, msg->actual.demuxer.state);
    player->demuxer_state = msg->actual.demuxer.state;
    if ((player->state == IDLE) && (player->demuxer_state == DMCUED)) {
      player->state = CUED;
    }
    if ((player->state == PLAYERDESTROYING) && (player->demuxer_state == DMCUED)) {
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
  
  //printf ("kr_player_process done\n");  
  
  return 1;
}

static int kr_player_demuxer_packet (kr_packet_t *packet, void *actual) {

  kr_player_t *player;

  player = (kr_player_t *)actual;

  printf ("wee packet sized %zu track %d!\n", packet->size, packet->track);
  
  if (packet->track == 1) {
    int ret;
    ret = krad_flac_decode (player->flac, packet->buffer,
                            packet->size, player->samples);
    printf ("flac decode ret is %d\n", ret);
    
    while ((krad_resample_ring_write_space (player->resample_ring[0]) <= ret * 4) || 
          (krad_resample_ring_write_space (player->resample_ring[1]) <= ret * 4)) {
      usleep (60000);
         if ((player->state == CUED) || (player->state == PLAYERDESTROYING)) {
          return 0;
        }
    }

    krad_resample_ring_write (player->resample_ring[0], (uint8_t *)player->samples[0], ret * 4);
    krad_resample_ring_write (player->resample_ring[1], (uint8_t *)player->samples[1], ret * 4);
    
    
         if ((player->state == CUED) || (player->state == PLAYERDESTROYING)) {
          return 0;
        }
    
    
    if (ret > 0) {
      return 1;
    } else {
      return 0;
    }
  }
  
  return 1;
}

static void kr_player_demuxer_status (kr_demuxer_state_t state, void *actual) {

  kr_player_t *player;

  player = (kr_player_t *)actual;

  kr_player_msg_t msg;
  msg.from = THEDEMUXER;
  msg.actual.demuxer.state = state;
  krad_machine_msg2 (player->machine, &msg);
}

static void kr_player_destroy_actual (void *actual) {

  printf ("kr_player_destroy_actual()!\n");
  int c;
  kr_player_t *player;

  player = (kr_player_t *)actual;

  kr_audioport_destroy (player->audioport);
  
  for (c = 0; c < player->channels; c++) {
    krad_resample_ring_destroy (player->resample_ring[c]);
    free (player->samples[c]);
  }
  
  kr_client_destroy (&player->client);

  //kr_decoder_destroy (&player->decoder);
  kr_demuxer_destroy (&player->demuxer);
  
  krad_flac_decoder_destroy (player->flac);
  
  free (player->station);
  free (player->url);
}

static void kr_player_start (void *actual) {
  
  kr_player_t *player;

  player = (kr_player_t *)actual;

  player->direction = FORWARD;
  player->speed = 100.0f;
  player->state = IDLE;

  int c;

  player->channels = 2;
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
  
  if (kr_mixer_get_info_wait (player->client,
                              &player->sample_rate, NULL) != 1) {
    fprintf (stderr, "Krad player: Could not get mixer info!\n");
	  kr_client_destroy (&player->client);
    exit (1);
  }

  for (c = 0; c < player->channels; c++) {
    player->resample_ring[c] = krad_resample_ring_create (600000, player->sample_rate,
                                                          player->sample_rate);
    player->samples[c] = malloc(4 * 8192);
  }
  
  player->audioport = kr_audioport_create (player->client, INPUT);
  if (player->audioport == NULL) {
    fprintf (stderr, "Krad player: could not setup audioport\n");
    exit (1);
  }
  kr_audioport_set_callback (player->audioport, audioport_process, player);
  kr_audioport_activate (player->audioport);
  /*
  player->videoport = kr_videoport_create (player->client);
  if (player->videoport == NULL) {
    fprintf (stderr, "could not setup videoport\n");
    exit (1);
  }
  //krad_player_alloc_framebuf (player);
  kr_videoport_set_callback (player->videoport, videoport_process, player);
  kr_videoport_activate (player->videoport);
  */
  
  player->flac = krad_flac_decoder_create ();

  kr_demuxer_params_t demuxer_params;

  demuxer_params.url = player->url;
  demuxer_params.controller = player;
  demuxer_params.status_cb = kr_player_demuxer_status;
  demuxer_params.packet_cb = kr_player_demuxer_packet;
  player->demuxer = kr_demuxer_create (&demuxer_params);
  //player->decoder = kr_decoder_create ();

  printf ("kr_player_start()!\n");
}

/* Public Functions */

void kr_player_destroy (kr_player_t **player) {
  kr_player_msg_t msg;
  if ((player != NULL) && (*player != NULL)) {
    printf ("kr_player_destroy()!\n");
    msg.from = THEUSER;
    msg.actual.user.cmd = PLAYERDESTROY;
    krad_machine_msg ((*player)->machine, &msg);
    krad_machine_destroy (&(*player)->machine);
    free (*player);
    *player = NULL;
  }
}

kr_player_t *kr_player_create (char *station, char *url) {
  
  kr_player_t *player;
  kr_machine_params_t machine_params;

  player = calloc (1, sizeof(kr_player_t));

  player->url = strdup (url);
  player->station = strdup (station);

  machine_params.actual = player;
  machine_params.msg_sz = sizeof (kr_player_msg_t);
  machine_params.start = kr_player_start;
  machine_params.process = kr_player_process;
  machine_params.destroy = kr_player_destroy_actual;

  player->machine = krad_machine_create (&machine_params);
  
  return player;
};

char *kr_player_playback_state_to_string (kr_player_playback_state_t state) {
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

char *kr_direction_to_string (kr_direction_t direction) {
  switch (direction) {
    case FORWARD:
      return "Forward";
    case REVERSE:
      return "Reverse";
  }
  return "Unknown";
}

float kr_player_speed_get (kr_player_t *player) {
  return player->speed;
}

kr_direction_t kr_player_direction_get (kr_player_t *player) {
  return player->direction;
}

int64_t kr_player_position_get (kr_player_t *player) {
  return player->position;
}

kr_player_playback_state_t kr_player_playback_state_get (kr_player_t *player) {
  return player->state;
}

void kr_player_speed_set (kr_player_t *player, float speed) {
  kr_player_msg_t msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = SETSPEED;
  msg.actual.user.param.real = roundf (speed*1000.0f)/1000.0f;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_direction_set (kr_player_t *player, kr_direction_t direction) {
  kr_player_msg_t msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = SETDIR;
  msg.actual.user.param.integer = direction;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_seek (kr_player_t *player, int64_t position) {
  kr_player_msg_t msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = SEEK;
  msg.actual.user.param.integer = position;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_play (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = PLAY;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_pause (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.from = THEUSER;
  msg.actual.user.cmd = PAUSE;
  krad_machine_msg (player->machine, &msg);
}

void kr_player_stop (kr_player_t *player) {
//  kr_player_msg_t msg;
//  msg.from = THEUSER;
//  msg.actual.user.cmd = STOP;
//  krad_machine_msg (player->machine, &msg);
}
