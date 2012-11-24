#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>

#include <xmms2/xmmsclient/xmmsclient.h>

#include "krad_system.h"
#include "../krad_tags/krad_tags.h"

#define KRAD_XMMS_RECONNECT_SLEEP_INTERVAL 5

typedef enum {
	PREV,
	PLAY,
	PAUSE,
	STOP,
	NEXT
} krad_xmms_playback_cmd_t;

typedef enum {
	krad_xmms_STOPPED,
	krad_xmms_PAUSED,
	krad_xmms_PLAYING
} krad_xmms_playback_state_t;

typedef struct krad_xmms_St krad_xmms_t;

struct krad_xmms_St {

	char sysname[256];
	char ipc_path[256];
	xmmsc_connection_t *connection;
	int fd;
	int connected;
	int destroying;

	pthread_mutex_t lock;

	pthread_t handler_thread;
	int handler_running;
	
	int handler_thread_socketpair[2];
	
	int playback_status;
	int playtime;
	int playing_id;
	char playback_status_string[128];

	int active_playlist_position;
	char active_playlist[256];

	char playtime_string[128];
	char now_playing[1024];
	char title[512];
	char artist[512];

	krad_tags_t *krad_tags;

};


void krad_xmms_playback_cmd (krad_xmms_t *krad_xmms, krad_xmms_playback_cmd_t cmd);
void krad_xmms_destroy (krad_xmms_t *krad_xmms);
krad_xmms_t *krad_xmms_create (char *name, char *ipc_path, krad_tags_t *krad_tags);

