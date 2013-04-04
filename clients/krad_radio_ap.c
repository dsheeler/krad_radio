#include "kr_client.h"
#include "krad_tone.h"

krad_tone_t *krad_tone;
krad_tone_t *krad_tone2;

int audioport_process (uint32_t nframes, void *arg) {

	int s;
	float *buffer;
	kr_audioport_t *audioport;
	static int32_t total_frames = 0;
	
	total_frames += nframes;
	
	if (total_frames > 44100) {
		krad_tone_add_preset (krad_tone, "dialtone");
		krad_tone_add_preset (krad_tone2, "6");
	  total_frames = 0;
	}
	
	audioport = (kr_audioport_t *)arg;
	
	buffer = kr_audioport_get_buffer (audioport, 0);
	if (1) {
		krad_tone_run (krad_tone, buffer, nframes);
		buffer = kr_audioport_get_buffer (audioport, 1);
		krad_tone_run (krad_tone2, buffer, nframes);
	} else {
		for (s = 0; s < 1600; s++) {
			if (buffer[s] > 0.3) {
				printf("signal!\n");
				break;
			}
		}
	}
	
	return 0;

}

int main (int argc, char *argv[]) {

	kr_client_t *client;
	kr_audioport_t *audioport;
	krad_mixer_portgroup_direction_t direction;

	direction = INPUT;

	if (argc != 2) {
		if (argc > 2) {
			fprintf (stderr, "Only takes station argument.\n");
		} else {
			fprintf (stderr, "No station specified.\n");
		}
		return 1;
	}
	
	client = kr_client_create ("krad audioport client");
  kr_connect (client, argv[1]);
	
	if (client == NULL) {
		fprintf (stderr, "Could not connect to %s krad radio daemon.\n", argv[1]);
		return 1;
	}	

	if (direction == INPUT) {
		krad_tone = krad_tone_create(44100);
		krad_tone_add_preset (krad_tone, "3");
		krad_tone2 = krad_tone_create(44100);
		krad_tone_add_preset (krad_tone2, "3");
	}

	audioport = kr_audioport_create (client, direction);

	if (audioport != NULL) {
		printf ("i worked real good\n");
	}
	
	kr_audioport_set_callback (audioport, audioport_process, audioport);
	
	kr_audioport_activate (audioport);
	
	usleep (8000000);
	
	kr_audioport_deactivate (audioport);
	
	kr_audioport_destroy (audioport);

	if (direction == INPUT) {
		krad_tone_destroy (krad_tone);
		krad_tone_destroy (krad_tone2);
	}
	
	kr_disconnect (client);

	return 0;
	
}
