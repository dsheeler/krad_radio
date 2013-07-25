#include "kr_client.h"
#include "krad_tone.h"

static int destroy = 0;

krad_tone_t *krad_tone;
krad_tone_t *krad_tone2;

void signal_recv(int sig) {
  destroy = 1;
}

int audioport_process(uint32_t nframes, void *arg) {

	int s;
	float *buffer;
	kr_audioport_t *audioport;
	static int32_t total_frames = 0;

	total_frames += nframes;

	if (total_frames > 35000) {
		krad_tone_add_preset (krad_tone, "dialtone");
		krad_tone_add_preset (krad_tone2, "6");
	  total_frames = 0;
	}

	audioport = (kr_audioport_t *)arg;

	buffer = kr_audioport_get_buffer(audioport, 0);
	if (1) {
		krad_tone_run(krad_tone, buffer, nframes);
		buffer = kr_audioport_get_buffer(audioport, 1);
		krad_tone_run(krad_tone2, buffer, nframes);
	} else {
		for (s = 0; s < 1600; s++) {
			if (buffer[s] > 0.3) {
				printf("signals!\n");
				break;
			}
		}
	}

	return 0;
}

int main (int argc, char *argv[]) {

  int i;
  int ret;
	kr_client_t *client;
	kr_audioport_t *audioport;
  //FIXME
  int direction;
  uint32_t sample_rate;

  ret = 0;
	direction = KR_MXR_INPUT;

	if (argc != 2) {
		if (argc > 2) {
			fprintf (stderr, "Only takes station argument.\n");
		} else {
			fprintf (stderr, "No station specified.\n");
		}
		return 1;
	}

	client = kr_client_create("krad audioport client");

	if (client == NULL) {
		fprintf (stderr, "Could not create KR client.\n");
		return 1;
	}

  kr_connect(client, argv[1]);

  if (!kr_connected(client)) {
		fprintf (stderr, "Could not connect to %s krad radio daemon.\n", argv[1]);
	  kr_client_destroy(&client);
	  return 1;
  }

  if (kr_mixer_get_info_wait(client, &sample_rate, NULL) != 1) {
    fprintf (stderr, "Could not get mixer info!\n");
	  kr_client_destroy(&client);
	  return 1;
  }

	if (direction == KR_MXR_INPUT) {
		krad_tone = krad_tone_create(sample_rate);
		krad_tone_add_preset(krad_tone, "3");
		krad_tone2 = krad_tone_create(sample_rate);
		krad_tone_add_preset(krad_tone2, "3");
	}

  kr_mixer_create_portgroup(client, "testport", "input", "krad", 2);

	audioport = kr_audioport_create(client, "testport", direction);

	if (audioport == NULL) {
		fprintf (stderr, "Could not make audioport.\n");
	  kr_client_destroy(&client);
	  return 1;
	} else {
		printf("Audioport created!\n");
	}

	kr_audioport_set_callback(audioport, audioport_process, audioport);

  signal(SIGINT, signal_recv);
  signal(SIGTERM, signal_recv);

	kr_audioport_connect(audioport);

	for (i = 0; i < 40; i++) {
	  usleep (30000);
	  if (destroy == 1) {
		  printf ("Got signal!\n");
	    break;
	  }
    if (kr_audioport_error (audioport)) {
      printf ("Error: %s\n", "Audioport Error");
      ret = 1;
      break;
    }
	}

	kr_audioport_disconnect(audioport);

	kr_audioport_destroy(audioport);

	if (direction == KR_MXR_INPUT) {
		krad_tone_destroy (krad_tone);
		krad_tone_destroy (krad_tone2);
	}

	kr_client_destroy (&client);

	if (ret == 0) {
		printf ("Worked!\n");
	}

	return ret;
}
