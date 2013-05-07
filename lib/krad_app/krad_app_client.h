#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <inttypes.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/mman.h>

#include "krad_radio_version.h"
#include "krad_radio_ipc.h"
#include "krad_system.h"

#include "krad_ebml2.h"
#include "krad_io2.h"

#ifndef KRAD_APP_CLIENT_H
#define KRAD_APP_CLIENT_H

#define KRAD_APP_CLIENT_DOCTYPE "krad_app_client"
#define KRAD_APP_SERVER_DOCTYPE "krad_app_server"
#define KRAD_APP_DOCTYPE_VERSION KRAD_VERSION
#define KRAD_APP_DOCTYPE_READ_VERSION KRAD_VERSION
#define EBML_ID_KRAD_APP_CMD 0x4444

typedef struct krad_app_client_St krad_app_client_t;

struct krad_app_client_St {
	char sysname[64];
	int sd;
	int tcp_port;
	char host[256];
	char api_path[256];
	int api_path_pos;
	int on_linux;
	struct stat info;
	struct utsname unixname;
};

krad_app_client_t *krad_app_connect (char *sysname, int timeout_ms);
void krad_app_disconnect (krad_app_client_t *client);
int krad_app_client_send_fd (krad_app_client_t *client, int fd);

#endif
