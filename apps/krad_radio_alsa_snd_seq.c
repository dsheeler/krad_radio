#include "krad_ipc_client.h"
#include "krad_alsa_seq.h"

int callback (krad_alsa_seq_t *krad_alsa_seq, snd_seq_event_t *ev) {
	
	int value = 0;
	// 10 cross, 13, 14
	
	if ((ev->data.control.param == 39) || ev->data.control.param == 40) {
		value = ((ev->data.control.value / 127.0) * 200.0) - 100.0;
		krad_ipc_set_control (krad_alsa_seq->krad_ipc_client, "XMMS2", 
							  "crossfade", value);
	}
	if (ev->data.control.param == 8) {
		value = (ev->data.control.value / 127.0) * 100.0;
		krad_ipc_set_control (krad_alsa_seq->krad_ipc_client, "XMMS2", 
		                      "volume", value);
	}
	if (ev->data.control.param == 9) {
		value = (ev->data.control.value / 127.0) * 100.0;
		krad_ipc_set_control (krad_alsa_seq->krad_ipc_client, "XMMS2-01", 
		                      "volume", value);
	}
	return value;
}

int callback1 (krad_alsa_seq_t *krad_alsa_seq, snd_seq_event_t *ev) {
	
	int value = 0;
	
	
	if (ev->data.control.param == 10) {
		value = (ev->data.control.value / 127.0) * 100.0;
		krad_ipc_set_control (krad_alsa_seq->krad_ipc_client, "Pulse", 
		                      "volume", value);
	}
	if (ev->data.control.param == 12) {
		value = (ev->data.control.value / 127.0) * 100.0;
		krad_ipc_set_control (krad_alsa_seq->krad_ipc_client, "PS3", 
		                      "volume", value);
	}
	
	return value;
}

int main (int argc, char *argv[]) {
	
	krad_alsa_seq_t *krad_alsa_seq = krad_alsa_seq_create ();
	
	if (argc == 3) {
		if (strncmp(argv[2], "alsa", 4) == 0) {
			krad_alsa_seq_run (krad_alsa_seq, argv[1]);
		}
	}
	
	if (!krad_valid_host_and_port (krad_alsa_seq->name)) {
		if (!krad_valid_sysname(krad_alsa_seq->name)) {
			failfast ("");
		}
	}
	
	krad_ipc_client_t *client = krad_ipc_connect (krad_alsa_seq->name);
	
	if (client == NULL) {
		failfast ("");
	}
	
	krad_alsa_seq_set_ipc_client (krad_alsa_seq, client);
	
	krad_alsa_seq_register_event_callback(krad_alsa_seq, callback);
	krad_alsa_seq_register_event_callback(krad_alsa_seq, callback1);
	
	pthread_exit(NULL);
}


