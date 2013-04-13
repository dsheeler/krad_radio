#include "krad_player.h"

#include "kr_client.h"

#define CTRLMP 0
#define NEEDDATA 1
#define AUDIOMP 2

typedef struct kr_player_msg_St kr_player_msg_t;

struct kr_player_msg_St {
  kr_player_cmd_t cmd;
  union {
    float real;
    int64_t integer;
  } param;
};

struct kr_player_St {
  pthread_t thread;
  kr_msgsys_t *msgsys;
  char *url;
  float speed;
  int64_t position;
  kr_player_state_t state;
  kr_direction_t direction;
  krad_container_t *input;
  
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
  
    if (player->state == PLAYING) {
      kr_player_msg_t msg;
      msg.cmd = PLAY;
      kr_msgsys_write (player->msgsys, NEEDDATA, &msg);
    }  
    memset(output[0], '0', nframes * 4);
    memset(output[1], '0', nframes * 4);
  }

  return 0;
}

int kr_player_process (void *arg) {

  kr_player_t *player;
  kr_player_msg_t msg;
  int ret;

  player = (kr_player_t *)arg;

  ret = kr_msgsys_read (player->msgsys, CTRLMP, &msg); 
  if (ret == 0) {
    return 0;
  }
  switch (msg.cmd) {
    case TICKLE:
      printf ("\nGot TICKLE command!\n");
      krad_container_destroy (&player->input);   
      break;
    case PLAYERDESTROY:
      printf ("\nGot PLAYERDESTROY command!\n");
      krad_container_destroy (&player->input);
      break;
    case PAUSE:
      printf ("\nGot PAUSE command!\n");
      if (player->input != NULL) {
        player->state = CUED;
      }
      break;
    case STOP:
      printf ("\nGot STOP command!\n");
      if (player->input != NULL) {
        player->state = CUED;
      }
      break;
    case PLAY:
      printf ("\nGot PLAY command!\n");
      if (player->input == NULL) {
        player->input = krad_container_open_file (player->url,
                                                  KRAD_IO_READONLY);      
      }
      if (player->input != NULL) {
        player->state = PLAYING;
      }
      break;
    case SEEK:
      printf ("\nGot SEEK %"PRIi64" command!\n", msg.param.integer);
      break;
    case SETSPEED:
      printf ("\nGot SETSPEED %0.3f command!\n", msg.param.real);
      player->speed = msg.param.real;
      break;
    case SETDIR:
      printf ("\nGot SETDIR %"PRIi64" command!\n", msg.param.integer);
      player->direction = msg.param.integer;
      break;    
  }

  return 0;
}

/* Public Functions */

void kr_player_destroy (kr_player_t **player) {
  if ((player != NULL) && (*player != NULL)) {
    printf ("kr_player_destroy()!\n");
    if ((*player)->videoport != NULL) {
      kr_videoport_deactivate ((*player)->videoport);
      kr_videoport_destroy ((*player)->videoport);
    }
    if ((*player)->audioport != NULL) {
      kr_audioport_deactivate ((*player)->audioport);
      kr_audioport_destroy ((*player)->audioport);
    }
    krad_container_destroy (&(*player)->input);
    kr_msgsys_destroy (&(*player)->msgsys);  
    free ((*player)->station);
    free ((*player)->url);
    free (*player);
  }
}

kr_player_t *kr_player_create (char *station, char *url) {
  
  int c;
  kr_player_t *player;
  
  player = calloc (1, sizeof(kr_player_t));

  player->direction = FORWARD;
  player->speed = 100.0f;
  player->state = IDLE;
  player->url = strdup (url);
  player->msgsys = kr_msgsys_create (2, sizeof(kr_player_msg_t));

  player->channels = 2;
  
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

  for (c = 0; c < player->channels; c++) {
    player->resample_ring[c] = krad_resample_ring_create (600000, 48000,
                                                          player->sample_rate);
  }
  
  player->audioport = kr_audioport_create (player->client, INPUT);
  if (player->audioport == NULL) {
    fprintf (stderr, "Krad Player: could not setup audioport\n");
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

  printf ("kr_player_create()!\n");
  return player;
}

char *kr_player_state_to_string (kr_player_state_t state) {
  switch (state) {
    case IDLE:
      return "IDLE";
    case CUED:
      return "CUED";
    case PLAYING:
      return "PLAYING";
    case PAUSING:
      return "PAUSING";
    case RESUMING:
      return "RESUMING";
    case LOOPING:
      return "LOOPING";
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

kr_player_state_t kr_player_state_get (kr_player_t *player) {
  return player->state;
}

void kr_player_speed_set (kr_player_t *player, float speed) {
  kr_player_msg_t msg;
  msg.cmd = SETSPEED;
  msg.param.real = roundf (speed*1000.0f)/1000.0f;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_direction_set (kr_player_t *player, kr_direction_t direction) {
  kr_player_msg_t msg;
  msg.cmd = SETDIR;
  msg.param.integer = direction;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_seek (kr_player_t *player, int64_t position) {
  kr_player_msg_t msg;
  msg.cmd = SEEK;
  msg.param.integer = position;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_play (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.cmd = PLAY;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_pause (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.cmd = PAUSE;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}

void kr_player_stop (kr_player_t *player) {
  kr_player_msg_t msg;
  msg.cmd = STOP;
  kr_msgsys_write (player->msgsys, CTRLMP, &msg);
}
