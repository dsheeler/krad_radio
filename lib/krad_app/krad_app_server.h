#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>

#ifdef KR_LINUX
#include <ifaddrs.h>
#endif

#include "krad_radio_version.h"
#include "krad_system.h"
#include "krad_ring.h"
#include "krad_io.h"
#include "krad_radio_ipc.h"

#include "krad_radio_client.h"

#ifndef KRAD_APP_SERVER_H
#define KRAD_APP_SERVER_H

#define KRAD_APP_CLIENT_DOCTYPE "krad_app_client"
#define KRAD_APP_SERVER_DOCTYPE "krad_app_server"
#define KRAD_APP_DOCTYPE_VERSION KR_VERSION
#define KRAD_APP_DOCTYPE_READ_VERSION KR_VERSION

#define EBML_ID_KRAD_APP_CMD 0x4444

#define MAX_REMOTES 16
#define KRAD_APP_SERVER_MAX_CLIENTS 16
#define MAX_BROADCASTS 128
#define MAX_BROADCASTERS 16

enum krad_app_shutdown {
  KRAD_APP_STARTING = -1,
  KRAD_APP_RUNNING,
  KRAD_APP_DO_SHUTDOWN,
  KRAD_APP_SHUTINGDOWN,
};

typedef struct kr_app_server kr_app_server;
typedef struct kr_app_server kr_as;
typedef struct kr_app_server_client kr_app_server_client;
typedef struct kr_app_server_client kr_as_client;
typedef struct kr_app_broadcaster krad_app_broadcaster_t;
typedef struct kr_app_broadcaster kr_app_broadcaster;
typedef struct krad_broadcast_msg_St krad_broadcast_msg_t;

struct krad_broadcast_msg_St {
  unsigned char *buffer;
  uint32_t size;
  kr_app_server_client *skip_client;
};

struct kr_app_broadcaster {
  kr_app_server *app;
  krad_ringbuffer_t *msg_ring;
  int sockets[2];
};

struct kr_app_server {

  struct sockaddr_un saddr;
  struct utsname unixname;
  int on_linux;
  int sd;
  int tcp_sd[MAX_REMOTES];
  uint16_t tcp_port[MAX_REMOTES];
  char *tcp_interface[MAX_REMOTES];
  int shutdown;

  int socket_count;

  krad_control_t krad_control;
  uint32_t num_clients;
  kr_app_server_client *clients;
  kr_app_server_client *current_client;

  void *(*client_create)(void *);
  void (*client_destroy)(void *);
  int (*client_handler)(kr_io2_t *in, kr_io2_t *out, void *);
  void *pointer;

  pthread_t server_thread;

  struct pollfd sockets[KRAD_APP_SERVER_MAX_CLIENTS + MAX_BROADCASTERS + MAX_REMOTES + 2];
  kr_app_server_client *sockets_clients[KRAD_APP_SERVER_MAX_CLIENTS + MAX_BROADCASTERS + MAX_REMOTES + 2];

  krad_app_broadcaster_t *sockets_broadcasters[MAX_BROADCASTERS + MAX_REMOTES + 2];
  krad_app_broadcaster_t *broadcasters[MAX_BROADCASTERS];
  int broadcasters_count;
  uint32_t broadcasts[MAX_BROADCASTS];
  int broadcasts_count;

  krad_app_broadcaster_t *app_broadcaster;
};

struct kr_app_server_client {
  int sd;
  void *ptr;
  int broadcasts;
  kr_io2_t *in;
  kr_io2_t *out;
};

void krad_app_server_add_client_to_broadcast(kr_app_server *app, uint32_t broadcast_ebml_id);
int krad_broadcast_msg_destroy(krad_broadcast_msg_t **broadcast_msg);
krad_broadcast_msg_t *krad_broadcast_msg_create(krad_app_broadcaster_t *broadcaster, unsigned char *buffer, uint32_t size);
int krad_app_server_broadcaster_broadcast(krad_app_broadcaster_t *broadcaster, krad_broadcast_msg_t **broadcast_msg);
void krad_app_server_broadcaster_register_broadcast(krad_app_broadcaster_t *broadcaster, uint32_t broadcast_ebml_id);
krad_app_broadcaster_t *krad_app_server_broadcaster_register(kr_app_server *app_server);
int krad_app_server_broadcaster_unregister(krad_app_broadcaster_t **broadcaster);
int krad_app_server_current_client_is_subscriber(kr_app_server *app);

int krad_app_server_recvfd(kr_app_server_client *client);

int krad_app_server_disable_remote(kr_app_server *app_server,
                                   char *interface,
                                   int port);
int krad_app_server_enable_remote(kr_app_server *app_server,
                                  char *interface,
                                  uint16_t port);
uint32_t krad_app_server_num_clients(kr_app_server *app_server);
void krad_app_server_disable(kr_app_server *krad_app_server);
void krad_app_server_destroy(kr_app_server *app_server);
void krad_app_server_run(kr_app_server *krad_app_server);
kr_app_server *
krad_app_server_create(char *appname, char *sysname,
                       void *client_create(void *),
                       void client_destroy(void *),
                       int client_handler(kr_io2_t *in,
                                          kr_io2_t *out,
                                          void *),
                       void *pointer);

#endif

