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
#include "krad_io2.h"
#include "krad_radio_ipc.h"

#include "krad_radio_client.h"

#ifndef KRAD_IPC_SERVER_H
#define KRAD_IPC_SERVER_H

#define KRAD_IPC_CLIENT_DOCTYPE "krad_ipc_client"
#define KRAD_IPC_SERVER_DOCTYPE "krad_ipc_server"
#define KRAD_IPC_DOCTYPE_VERSION KRAD_VERSION
#define KRAD_IPC_DOCTYPE_READ_VERSION KRAD_VERSION

#define EBML_ID_KRAD_IPC_CMD 0x4444

#define MAX_REMOTES 16
#define KRAD_IPC_SERVER_MAX_CLIENTS 16
#define MAX_BROADCASTS 128
#define MAX_BROADCASTERS 16

enum krad_ipc_shutdown {
  KRAD_IPC_STARTING = -1,
  KRAD_IPC_RUNNING,
  KRAD_IPC_DO_SHUTDOWN,
  KRAD_IPC_SHUTINGDOWN,
};

typedef struct krad_ipc_server_St krad_ipc_server_t;
typedef struct krad_ipc_server_client_St krad_ipc_server_client_t;
typedef struct krad_ipc_broadcaster_St krad_ipc_broadcaster_t;
typedef struct krad_broadcast_msg_St krad_broadcast_msg_t;

struct krad_broadcast_msg_St {
  unsigned char *buffer;
  uint32_t size;
  krad_ipc_server_client_t *skip_client;
};

struct krad_ipc_broadcaster_St {
  krad_ipc_server_t *ipc_server;
  krad_ringbuffer_t *msg_ring;
  int sockets[2];
};

struct krad_ipc_server_St {

  struct sockaddr_un saddr;
  struct utsname unixname;
  int on_linux;
  int sd;
  int tcp_sd[MAX_REMOTES];
  int tcp_port[MAX_REMOTES];
  char *tcp_interface[MAX_REMOTES];
  int shutdown;

  int socket_count;
  
  krad_control_t krad_control;
  
  krad_ipc_server_client_t *clients;
  krad_ipc_server_client_t *current_client;

  void *(*client_create)(void *);
  void (*client_destroy)(void *);
  int (*client_handler)(kr_io2_t *in, kr_io2_t *out, void *);
  void *pointer;

  pthread_t server_thread;

  struct pollfd sockets[KRAD_IPC_SERVER_MAX_CLIENTS + MAX_BROADCASTERS + MAX_REMOTES + 2];
  krad_ipc_server_client_t *sockets_clients[KRAD_IPC_SERVER_MAX_CLIENTS + MAX_BROADCASTERS + MAX_REMOTES + 2];  

  krad_ipc_broadcaster_t *sockets_broadcasters[MAX_BROADCASTERS + MAX_REMOTES + 2];
  krad_ipc_broadcaster_t *broadcasters[MAX_BROADCASTERS];
  int broadcasters_count;
  uint32_t broadcasts[MAX_BROADCASTS];
  int broadcasts_count;

  krad_ipc_broadcaster_t *ipc_broadcaster;
};

struct krad_ipc_server_client_St {
  int sd;
  void *ptr;
  int broadcasts;
  struct pollfd *pollptr;
  kr_io2_t *in;
  kr_io2_t *out;
};



void krad_ipc_server_add_client_to_broadcast ( krad_ipc_server_t *krad_ipc_server, uint32_t broadcast_ebml_id );
int krad_broadcast_msg_destroy (krad_broadcast_msg_t **broadcast_msg);
krad_broadcast_msg_t *krad_broadcast_msg_create (krad_ipc_broadcaster_t *broadcaster, unsigned char *buffer, uint32_t size);
int krad_ipc_server_broadcaster_broadcast ( krad_ipc_broadcaster_t *broadcaster, krad_broadcast_msg_t **broadcast_msg );
void krad_ipc_server_broadcaster_register_broadcast ( krad_ipc_broadcaster_t *broadcaster, uint32_t broadcast_ebml_id );
krad_ipc_broadcaster_t *krad_ipc_server_broadcaster_register ( krad_ipc_server_t *ipc_server );
int krad_ipc_server_broadcaster_unregister ( krad_ipc_broadcaster_t **broadcaster );
int krad_ipc_server_current_client_is_subscriber (krad_ipc_server_t *ipc);

int krad_ipc_server_recvfd (krad_ipc_server_client_t *client);

int krad_ipc_server_disable_remote (krad_ipc_server_t *krad_ipc_server, char *interface, int port);
int krad_ipc_server_enable_remote (krad_ipc_server_t *krad_ipc_server, char *interface, int port);
void krad_ipc_server_disable (krad_ipc_server_t *krad_ipc_server);
void krad_ipc_server_destroy (krad_ipc_server_t *ipc_server);
void krad_ipc_server_run (krad_ipc_server_t *krad_ipc_server);
krad_ipc_server_t *krad_ipc_server_create (char *appname, char *sysname,
                                           void *client_create (void *),
                                           void client_destroy (void *),
                                           int client_handler (kr_io2_t *in, kr_io2_t *out, void *),
                                           void *pointer);

#endif

