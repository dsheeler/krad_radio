#include "krad_player.h"

#include "kr_client.h"


#define CTRLMP 0
#define VIDEOMP 1
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
  krad_xpdr_t *xpdr;
  int xpdr_id;
  
  char *station;
  kr_client_t *client;
  kr_videoport_t *videoport;

  int adec_id;
  kr_audioport_t *audioport;
	krad_resample_ring_t *resample_ring[2];
  uint32_t sample_rate;
	float *au_samples[2];
	float *samples[2];
	kr_xpdr_subunit_t *adec_su;
	krad_vorbis_t *krad_vorbis;
	krad_flac_t *krad_flac;
	krad_opus_t *krad_opus;
	unsigned char *au_header[3];
	int au_header_len[3];
	unsigned char *au_buffer;
	float *au_audio;
	krad_codec_t audio_codec;
	krad_codec_t last_audio_codec;
  int channels;
};

/* Private Functions */

/*
void demuxer_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  printk ("Input/Demuxing thread starting");

  krad_link->demux_header_buffer = malloc (4096 * 512);
  krad_link->demux_buffer = malloc (4096 * 2048);
  
  krad_link->demux_video_packets = 0;
  krad_link->demux_audio_packets = 0;  
  krad_link->demux_current_track = -1;  
  
  if (krad_link->host[0] != '\0') {
    krad_link->krad_container = krad_container_open_stream (krad_link->host, krad_link->port, krad_link->mount, NULL);
  } else {
    krad_link->krad_container = krad_container_open_file (krad_link->input, KRAD_IO_READONLY);
  }
}

int demuxer_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_system_set_thread_name ("kr_demuxer");

  int packet_size;
  uint64_t timecode;
  int header_size;
  int h;
  int total_header_size;
  int writeheaders;
  kr_slice_t *kr_slice;
  
  kr_slice = NULL;
  packet_size = 0;
  header_size = 0;
  total_header_size = 0;  
  writeheaders = 0;

  printk ("Demuxing process");

  packet_size = krad_container_read_packet ( krad_link->krad_container,
                                             &krad_link->demux_current_track,
                                             &timecode,
                                             krad_link->demux_buffer);
  //printk ("packet track %d timecode: %zu size %d", current_track, timecode, packet_size);
  if ((packet_size <= 0) && (timecode == 0) &&
      ((krad_link->demux_video_packets + krad_link->demux_audio_packets) > 20))  {
    //printk ("stream input thread packet size was: %d", packet_size);
    return 1;
  }

  if (krad_container_track_changed (krad_link->krad_container, krad_link->demux_current_track)) {
    printk ("track %d changed! status is %d header count is %d",
            krad_link->demux_current_track, krad_container_track_active(krad_link->krad_container, krad_link->demux_current_track),
            krad_container_track_header_count(krad_link->krad_container, krad_link->demux_current_track));

    krad_link->demux_track_codecs[krad_link->demux_current_track] = krad_container_track_codec (krad_link->krad_container, krad_link->demux_current_track);

    if (krad_link->demux_track_codecs[krad_link->demux_current_track] == NOCODEC) {
      return 1;
    }
    writeheaders = 1;
    for (h = 0; h < krad_container_track_header_count (krad_link->krad_container, krad_link->demux_current_track); h++) {
      printk ("header %d is %d bytes", h, krad_container_track_header_size (krad_link->krad_container, krad_link->demux_current_track, h));
      total_header_size += krad_container_track_header_size (krad_link->krad_container, krad_link->demux_current_track, h);
    }
  }

  if ((krad_link->demux_track_codecs[krad_link->demux_current_track] == Y4M) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == KVHS) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == VP8) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == THEORA)) {

    krad_link->demux_video_packets++;
  }


  if ((krad_link->demux_track_codecs[krad_link->demux_current_track] == VORBIS) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == OPUS) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == FLAC)) {

    krad_link->demux_audio_packets++;
  }

  if (writeheaders == 1) {
    for (h = 0; h < krad_container_track_header_count (krad_link->krad_container, krad_link->demux_current_track); h++) {
      header_size = krad_container_track_header_size (krad_link->krad_container, krad_link->demux_current_track, h);
      krad_container_read_track_header (krad_link->krad_container, krad_link->demux_header_buffer, krad_link->demux_current_track, h);
      if (krad_link->subunit != NULL) {
        kr_slice = kr_slice_create_with_data (krad_link->demux_header_buffer, header_size);
        kr_slice->frames = 0;
        kr_slice->header = h + 1;
        kr_slice->codec = krad_link->demux_track_codecs[krad_link->demux_current_track];
        kr_xpdr_slice_broadcast (krad_link->subunit, &kr_slice);
        kr_slice_unref (kr_slice);
      }
    }
  }
  if (krad_link->subunit != NULL) {
    kr_slice = kr_slice_create_with_data (krad_link->demux_buffer, packet_size);
    kr_slice->frames = 1;
    kr_slice->timecode = timecode;
    kr_slice->codec = krad_link->demux_track_codecs[krad_link->demux_current_track];
    kr_xpdr_slice_broadcast (krad_link->subunit, &kr_slice);
    kr_slice_unref (kr_slice);
  }

  return 0;  
}

void demuxer_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  printk ("Input/Demuxing thread exiting");

  krad_container_destroy (&krad_link->krad_container);

  free (krad_link->demux_buffer);
  free (krad_link->demux_header_buffer);

}
*/


int adec_process (void *arg) {

  kr_player_t *player = (kr_player_t *)arg;
  
  int c;
  int bytes;
  int len;
  kr_slice_t *kr_slice;

  printk ("Audio decoding process");

  /* THE FOLLOWING IS WHERE WE ENSURE WE ARE ON THE RIGHT CODEC AND READ HEADERS IF NEED BE */

  kr_slice = NULL;
  
  kr_slice = kr_xpdr_get_slice (player->adec_su);
  if (kr_slice == NULL) {
    return 1;
  }
  printk ("Got slice!");
  player->audio_codec = kr_slice->codec;

  if ((player->last_audio_codec != player->audio_codec) || (player->audio_codec == NOCODEC)) {
    printk ("audio codec is %d", player->audio_codec);
    if (player->last_audio_codec != NOCODEC)  {
      if (player->last_audio_codec == FLAC) {
        krad_flac_decoder_destroy (player->krad_flac);
        player->krad_flac = NULL;
      }
      if (player->last_audio_codec == VORBIS) {
        krad_vorbis_decoder_destroy (player->krad_vorbis);
        player->krad_vorbis = NULL;
      }      
      if (player->last_audio_codec == OPUS) {
        krad_opus_decoder_destroy (player->krad_opus);
        player->krad_opus = NULL;
      }
    }

    if (player->audio_codec == NOCODEC) {
      player->last_audio_codec = player->audio_codec;
      return 1;
    }

    if (player->audio_codec == FLAC) {
      player->krad_flac = krad_flac_decoder_create();
      // get flac header
      krad_flac_decode (player->krad_flac, player->au_header[0], player->au_header_len[0], NULL);
      for (c = 0; c < player->channels; c++) {
        krad_resample_ring_set_input_sample_rate (player->resample_ring[c], player->krad_flac->sample_rate);
      }
    }

    if (player->audio_codec == VORBIS) {
       // get vorbis headers player->au_header[h], player->au_header_len[h]);
      printk ("Vorbis Header byte sizes: %d %d %d", player->au_header_len[0], player->au_header_len[1], player->au_header_len[2]);
      player->krad_vorbis = krad_vorbis_decoder_create (player->au_header[0], player->au_header_len[0], player->au_header[1], player->au_header_len[1], player->au_header[2], player->au_header_len[2]);

      for (c = 0; c < player->channels; c++) {
        krad_resample_ring_set_input_sample_rate (player->resample_ring[c], player->krad_vorbis->sample_rate);
      }
    }

    if (player->audio_codec == OPUS) {
      // get opus header player->au_header[h], player->au_header_len[h]);
      printk ("Opus Header size: %d", player->au_header_len[0]);
      player->krad_opus = krad_opus_decoder_create (player->au_header[0], player->au_header_len[0], player->sample_rate);
      for (c = 0; c < player->channels; c++) {
        krad_resample_ring_set_input_sample_rate (player->resample_ring[c], player->sample_rate);
      }
    }
  }

  player->last_audio_codec = player->audio_codec;

  /* DECODING HAPPENS HERE */
  
  if (player->audio_codec == VORBIS) {
    krad_vorbis_decoder_decode (player->krad_vorbis, kr_slice->data, kr_slice->size);
    len = 1;
    while (len ) {
      len = krad_vorbis_decoder_read_audio(player->krad_vorbis, 0, (char *)player->au_samples[0], 512);
      if (len) {
        while (krad_resample_ring_write_space (player->resample_ring[0]) < len) {
          usleep(25000);
        }
        krad_resample_ring_write (player->resample_ring[0], (unsigned char *)player->au_samples[0], len);                    
      }
      len = krad_vorbis_decoder_read_audio (player->krad_vorbis, 1, (char *)player->au_samples[1], 512);
      if (len) {
        while (krad_resample_ring_write_space (player->resample_ring[1]) < len) {
          //printk ("wait!");
          usleep(25000);
        }
        krad_resample_ring_write (player->resample_ring[1], (unsigned char *)player->au_samples[1], len);          
      }
    }
  }

  if (player->audio_codec == FLAC) {
    len = krad_flac_decode (player->krad_flac, kr_slice->data, kr_slice->size, player->au_samples);
    for (c = 0; c < player->channels; c++) {
      krad_resample_ring_write (player->resample_ring[c], (unsigned char *)player->au_samples[c], len * 4);
    }
  }
    
  if (player->audio_codec == OPUS) {
    krad_opus_decoder_write (player->krad_opus, kr_slice->data, kr_slice->size);
    bytes = -1;
    while (bytes != 0) {
      for (c = 0; c < 2; c++) {
        bytes = krad_opus_decoder_read (player->krad_opus, c + 1, (char *)player->au_audio, 120 * 4);
        if (bytes) {
          if ((bytes / 4) != 120) {
            failfast ("uh oh crazyto");
          }
          while (krad_resample_ring_write_space (player->resample_ring[c]) < bytes) {
              usleep(20000);
          }
          //krad_ringbuffer_write (player->audio_output_ringbuffer[c], (char *)audio, bytes);
          krad_resample_ring_write (player->resample_ring[c], (unsigned char *)player->au_audio, bytes);
        }
      }
    }
  }

  kr_slice_unref (kr_slice);

  return 0;
}

void audio_decoding_unit_destroy (kr_player_t *player) {

  int c;
  int h;

  if (player->krad_vorbis != NULL) {
    krad_vorbis_decoder_destroy (player->krad_vorbis);
    player->krad_vorbis = NULL;
  }
  
  if (player->krad_flac != NULL) {
    krad_flac_decoder_destroy (player->krad_flac);
    player->krad_flac = NULL;
  }

  if (player->krad_opus != NULL) {
    krad_opus_decoder_destroy(player->krad_opus);
    player->krad_opus = NULL;
  }

  free (player->au_audio);
  
  for (c = 0; c < player->channels; c++) {
    free (player->samples[c]);
    free (player->au_samples[c]);
    krad_resample_ring_destroy ( player->resample_ring[c] );    
  }  

  for (h = 0; h < 3; h++) {
    free (player->au_header[h]);
  }

  printk ("Audio decoding unit exiting");
}

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
    memset(output[0], '0', nframes * 4);
    memset(output[1], '0', nframes * 4);
  }

  return 0;
}

void kr_player_destroy_internal (void *arg) {

  kr_player_t *player;

  player = (kr_player_t *)arg;
  krad_container_destroy (&player->input);
  kr_msgsys_destroy (&player->msgsys);
  
  audio_decoding_unit_destroy (player);
  
  free (player->station);
  free (player->url);
  free (player);
  printf ("kr_player_destroy_internal()!\n");    
}

int kr_player_process (void *arg) {

  kr_player_t *player;
  kr_player_msg_t msg;
  int ret;

  player = (kr_player_t *)arg;

  ret = kr_msgsys_read_watch_fd (player->msgsys, CTRLMP, &msg); 
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
    kr_xpdr_subunit_remove ((*player)->xpdr, (*player)->xpdr_id);
  }
}

kr_player_t *kr_player_create (char *station, krad_xpdr_t *xpdr, char *url) {
  
  int h;
  int c;
  kr_player_t *player;
  kr_xpdr_su_spec_t spec;
  
  memset (&spec, 0, sizeof(kr_xpdr_su_spec_t));
  player = calloc (1, sizeof(kr_player_t));

  player->xpdr = xpdr;
  player->direction = FORWARD;
  player->speed = 100.0f;
  player->state = IDLE;
  player->url = strdup (url);
  player->msgsys = kr_msgsys_create (1, sizeof(kr_player_msg_t));

  spec.fd = kr_msgsys_get_watch_fd (player->msgsys, CTRLMP);
  spec.ptr = player;
  spec.readable_callback = kr_player_process;
  spec.destroy_callback = kr_player_destroy_internal;
  
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
    player->au_samples[c] = malloc(4 * 8192);
    player->samples[c] = malloc(4 * 8192);  
    player->resample_ring[c] = krad_resample_ring_create (600000, 48000,
                                                          player->sample_rate);
  }
  for (h = 0; h < 3; h++) {
    player->au_header[h] = malloc(100000);
    player->au_header_len[h] = 0;
  }
  player->au_audio = calloc(1, 8192 * 4 * 4);
  player->last_audio_codec = NOCODEC;
  player->audio_codec = NOCODEC;
  
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
  player->xpdr_id = kr_xpdr_add_player (player->xpdr, &spec);


  spec.fd = -1;
  spec.ptr = player;
  spec.readable_callback = adec_process;
  spec.destroy_callback = NULL;
  player->adec_id = kr_xpdr_add_decoder (player->xpdr, &spec);

  player->adec_su = kr_xpdr_get_subunit (player->xpdr, player->adec_id);

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
