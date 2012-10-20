#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#include <alsa/asoundlib.h>

#include "krad_system.h"

#define KLUDGE 1
#define MAX_CALLBACKS 128

#ifdef KLUDGE
#include "krad_ipc_client.h"
#endif

typedef struct krad_alsa_seq_St krad_alsa_seq_t;
typedef int (t_alsa_seq_event_callback)(krad_alsa_seq_t *krad_alsa_seq, snd_seq_event_t *ev);

struct krad_alsa_seq_St {

	#ifdef KLUDGE
	krad_ipc_client_t *krad_ipc_client;
	
	#endif
	
	
	unsigned char *buffer;
	char name[128];
	int running;
	int stop;
	pthread_t running_thread;

	int port_id;
	snd_seq_t *seq_handle;
	
	int num_callbacks;
	t_alsa_seq_event_callback *callbacks[MAX_CALLBACKS];
	
};

#ifdef KLUDGE
void krad_alsa_seq_set_ipc_client (krad_alsa_seq_t *krad_alsa_seq, krad_ipc_client_t *krad_ipc_client);
#endif

//void krad_alsa_seq_parse_message (krad_alsa_seq_t *krad_alsa_seq, unsigned char *message, int size);

void *krad_alsa_seq_listening_thread (void *arg);

void krad_alsa_seq_stop (krad_alsa_seq_t *krad_alsa_seq);
int krad_alsa_seq_run (krad_alsa_seq_t *krad_alsa_seq, char *name);

void krad_alsa_seq_destroy (krad_alsa_seq_t *krad_alsa_seq);
krad_alsa_seq_t *krad_alsa_seq_create ();


void krad_alsa_seq_register_event_callback (krad_alsa_seq_t *krad_alsa_seq, t_alsa_seq_event_callback *event_callback);

