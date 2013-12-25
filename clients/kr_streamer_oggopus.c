#include <stdio.h>
#include <unistd.h>

#include <kr_client.h>
#include <krad_transmitter.h>
#include <krad_ogg2.h>
#include <krad_stream.h>
#include <krad_file.h>
#include <krad_opus.h>
#include <krad_ring.h>

typedef struct kr_streamer_St kr_streamer_t;
typedef struct kr_streamer_params_St kr_streamer_params_t;

struct kr_streamer_params_St {
  uint32_t channels;
  uint32_t sample_rate;
  uint32_t bitrate;
  char *station;
  char *file;
  char *host;
  int32_t port;
  char *mount;
  char *password;
};

struct kr_streamer_St {
  kr_streamer_params_t *params;
  kr_audioport_t *audioport;
  krad_ringbuffer_t *audio_ring[4];
  kr_client_t *client;
  krad_opus_t *opus;
  kr_codec_hdr_t header;
  kr_ogg_io_t *ogg_io;
  uint64_t samples;
};

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

int audioport_process (uint32_t nframes, void *arg) {

  int c;
  float *buffer;
  kr_streamer_t *streamer;

  streamer = (kr_streamer_t *)arg;

  for (c = 0; c < streamer->params->channels; c++) {
    buffer = kr_audioport_get_buffer (streamer->audioport, c);
    krad_ringbuffer_write (streamer->audio_ring[c],
                           (char *)buffer,
                           nframes * 4);
  }

  return 0;
}

int kr_streamer_destroy (kr_streamer_t **streamer) {

  int c;

  if ((streamer == NULL) || (*streamer == NULL)) {
    return -1;
  }

   kr_audioport_disconnect((*streamer)->audioport);
  kr_audioport_destroy ((*streamer)->audioport);
  kr_client_destroy (&(*streamer)->client);
  kr_ogg_io_destroy (&(*streamer)->ogg_io);
  krad_opus_encoder_destroy ((*streamer)->opus);
  for (c = 0; c < (*streamer)->params->channels; c++) {
    krad_ringbuffer_free ((*streamer)->audio_ring[c]);
  }
  free (*streamer);
  *streamer = NULL;
  return 0;
}

kr_streamer_t *kr_streamer_create (kr_streamer_params_t *params) {

  kr_streamer_t *streamer;
  uint32_t c;

  streamer = calloc (1, sizeof(kr_streamer_t));

  streamer->params = params;

  streamer->params->channels = 2;
  streamer->params->sample_rate = 48000;

  streamer->client = kr_client_create ("krad streamer client");

  if (streamer->client == NULL) {
    fprintf (stderr, "Could not create KR client.\n");
    exit (1);
  }

  kr_connect (streamer->client, streamer->params->station);

  if (!kr_connected (streamer->client)) {
    fprintf (stderr, "Could not connect to %s krad radio daemon.\n",
             streamer->params->station);
    kr_client_destroy (&streamer->client);
    exit (1);
  }

  for (c = 0; c < streamer->params->channels; c++) {
    streamer->audio_ring[c] = krad_ringbuffer_create (48000 * 4 * 8);
  }

  if (kr_mixer_get_info_wait (streamer->client, &streamer->params->sample_rate, NULL) != 1) {
    fprintf (stderr, "Could not get mixer info!\n");
    kr_client_destroy (&streamer->client);
    exit (1);
  }

  streamer->opus = krad_opus_encoder_create (streamer->params->channels,
   streamer->params->sample_rate, streamer->params->bitrate,
   OPUS_APPLICATION_AUDIO);

  if (streamer->opus == NULL) {
    fprintf (stderr, "failed to opus encoder :/ \n");
    exit (1);
  }

  if (params->file != NULL) {
    streamer->ogg_io = kr_ogg_io_create_file (params->file);
  } else {
    streamer->ogg_io = kr_ogg_io_create_stream (streamer->params->host,
     streamer->params->port, streamer->params->mount, "audio/ogg",
     streamer->params->password);
  }

  if (streamer->ogg_io == NULL) {
    fprintf (stderr, "failed to ogg stream :/ \n");
    exit (1);
  }

  kr_ogg_add_track (streamer->ogg_io->ogg, &streamer->opus->krad_codec_header);
  //FIXME
  streamer->audioport = kr_audioport_create (streamer->client, "opusstreamer", 0);
  kr_audioport_set_callback (streamer->audioport, audioport_process, streamer);

  return streamer;
}

void kr_streamer_run (kr_streamer_t *streamer) {

  kr_medium_t *amedium;
  kr_codeme_t *acodeme;
  int32_t ret;
  int32_t frames;
  uint32_t c;

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);

  amedium = kr_medium_kludge_create ();
  acodeme = kr_codeme_kludge_create ();

  frames = DEFAULT_OPUS_FRAME_SIZE;

  ret = kr_ogg_io_push_header (streamer->ogg_io);
  if (ret != 0) {
    printf("\r\nFailed to push ogg header!\n");
    destroy = 1;
  } else {
    kr_audioport_connect(streamer->audioport);
  }

  while (!destroy) {
    if (kr_audioport_error(streamer->audioport)) {
      printf ("\r\nError: %s\n", "Audioport Error\n");
      break;
    }
    while (krad_ringbuffer_read_space (streamer->audio_ring[1]) >= frames * 4) {
      for (c = 0; c < streamer->params->channels; c++) {
        krad_ringbuffer_read (streamer->audio_ring[c],
                              (char *)amedium->a.samples[c],
                              frames * 4);
        krad_opus_encoder_write (streamer->opus, c + 1,
         (char *)amedium->a.samples[c], frames * 4);
      }

      amedium->a.count = frames;
      amedium->a.channels = streamer->params->channels;

      acodeme->sz = krad_opus_encoder_read (streamer->opus, acodeme->data, &acodeme->count);

      while (acodeme->sz > 0) {

        ret = kr_ogg_io_push (streamer->ogg_io, 0, streamer->samples,
         acodeme->data, acodeme->sz);
        if (ret != 0) {
          printf("\r\nFailed to push ogg data!\n");
          destroy = 1;
          break;
        }
        acodeme->sz = krad_opus_encoder_read (streamer->opus, acodeme->data, &acodeme->count);
      }

      streamer->samples += frames;

      printf ("\rKrad Streamer Seconds: %12"PRIu64" Samples: %12"PRIu64"",
       streamer->samples / streamer->params->sample_rate, streamer->samples);
      fflush (stdout);

    }
    usleep(31337);
  }

  printf ("\r\n");

  kr_medium_kludge_destroy (&amedium);
  kr_codeme_kludge_destroy (&acodeme);
}

void kr_streamer (kr_streamer_params_t *params) {

  kr_streamer_t *streamer;

  streamer = kr_streamer_create (params);
  kr_streamer_run (streamer);
  kr_streamer_destroy (&streamer);
}

int main (int argc, char *argv[]) {

  kr_streamer_params_t params;
  char mount[256];
  char file[256];

  memset (&params, 0, sizeof(kr_streamer_params_t));

  if (argc < 2) {
    fprintf (stderr, "Need station name.\n");
    exit (1);
  }

  params.station = argv[1];

  params.bitrate = 128000;
  params.host = "europa.kradradio.com";
  params.port = 8008;

  if (argc == 3) {
    snprintf (mount, sizeof(mount), "/%s.opus",argv[2]);
  }

  params.mount = mount;
  params.password = "firefox";

  if (argc == 2) {
    snprintf (file, sizeof(file),
              "%s/%s_%"PRIu64".opus",
              getenv ("HOME"), "streamer", krad_unixtime ());
    params.file = file;
  } else {
    params.file = NULL;
  }

  printf ("Encoding: %s at %u\n", "opus", params.bitrate / 1000);

  if (params.file != NULL) {
    printf ("Recording to: %s\n", params.file);
  } else {
    printf ("Streaming to: %s:%u%s\n", params.host, params.port, params.mount);
  }

  kr_streamer (&params);

  return 0;
}
