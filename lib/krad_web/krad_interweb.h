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
#include <netinet/tcp.h>
#ifdef KR_LINUX
#include <ifaddrs.h>
#endif

#include "krad_radio_version.h"
#include "krad_system.h"
#include "krad_ring.h"
#include "krad_io2.h"
#include "krad_radio_client.h"
#include "krad_sha1.h"
#include "krad_base64.h"
#include "kr_client.h"

#ifndef KRAD_INTERWEB_SERVER_H
#define KRAD_INTERWEB_SERVER_H

#define MAX_REMOTES 16
#define KR_IWS_MAX_CLIENTS 64
#define KR_IWS_MAX_KRCLIENTS 64
#define KR_MAX_SDS KR_IWS_MAX_CLIENTS + KR_IWS_MAX_KRCLIENTS + MAX_REMOTES + 1

#define WS_MASK_BIT 0x80  // 10000000
#define WS_FIN_FRM 0x80   // 10000000
#define WS_CONT_FRM 0x00  // 00000000
#define WS_TEXT_FRM 0x01  // 00000001
#define WS_BIN_FRM 0x02   // 00000010
#define WS_CLOSE_FRM 0x08 // 00001000
#define WS_PING_FRM 0x09  // 00001001
#define WS_PONG_FRM 0x0a  // 00001010

enum krad_interweb_shutdown {
  KRAD_INTERWEB_STARTING = -1,
  KRAD_INTERWEB_RUNNING,
  KRAD_INTERWEB_DO_SHUTDOWN,
  KRAD_INTERWEB_SHUTINGDOWN,
};

typedef struct krad_interweb_server_St kr_interweb_server_t;
typedef struct krad_interweb_server_St krad_interweb_t;
typedef struct krad_interweb_server_St kr_interweb_t;
typedef struct krad_interweb_server_St kr_iws_t;
typedef struct krad_interweb_server_St krad_iws_t;
typedef struct krad_interweb_server_client_St krad_interweb_server_client_t;
typedef struct krad_interweb_server_client_St kr_iws_client_t;

struct krad_interweb_server_St {
  char sysname[64];
  int32_t tcp_sd[MAX_REMOTES];
  int32_t tcp_port[MAX_REMOTES];
  char *tcp_interface[MAX_REMOTES];
  int32_t shutdown;
  int32_t socket_count;
  krad_control_t krad_control;
  krad_interweb_server_client_t *clients;
  pthread_t server_thread;
  struct pollfd sockets[KR_MAX_SDS];
  int32_t socket_type[KR_MAX_SDS];
  kr_iws_client_t *sockets_clients[KR_MAX_SDS];
  int32_t uberport;
  char *headcode_source;
  char *htmlheader_source;
  char *htmlfooter_source;
  char *html;
  int32_t html_len;
  char *api_js;
  int32_t api_js_len;
  char *iface_js;
  int32_t iface_js_len;
  char *deviface_js;
  int32_t deviface_js_len;
  char *headcode;
  char *htmlheader;
  char *htmlfooter;
};

enum interweb_client_type {
  INTERWEB_UNKNOWN = 0,
  KR_IWS_WS,
  KR_IWS_FILE,
  KR_IWS_STREAM_IN,
  KR_IWS_STREAM_OUT,
  KR_APP,
  KR_REMOTE_LISTEN,
};

enum interweb_verb {
  KR_IWS_INVALID = 0,
  KR_IWS_GET,
  KR_IWS_PUT,
  KR_IWS_SOURCE,
  KR_IWS_POST,
  KR_IWS_HEAD,
  KR_IWS_PATCH,
  KR_IWS_OPTIONS,
};

typedef struct interwebs_St interwebs_t;

struct interwebs_St {
  uint8_t mask[4];
  uint32_t pos;
  uint64_t len;
  uint8_t *input;
  uint32_t input_len;
  uint8_t *output;
  uint32_t output_len;
  uint64_t frames;
  uint32_t shaked;
  char key[96];
  char proto[96];
  kr_client_t *krclient;
};

struct krad_interweb_server_client_St {
  int32_t sd;
  krad_interweb_t *server;
  kr_io2_t *in;
  kr_io2_t *out;
  int32_t drop_after_sync;
  int32_t type;
  uint32_t hdr_le;
  uint32_t hdr_pos;
  uint32_t hdrs_recvd;
  int32_t verb;
  char get[96];
  char mount[128];
  interwebs_t ws;
};

int32_t krad_interweb_server_listen_off (kr_interweb_server_t *server,
 char *interface, int32_t port);
int32_t krad_interweb_server_listen_on (kr_interweb_server_t *server,
 char *interface, int32_t port);

void krad_interweb_server_disable (kr_interweb_server_t *server);
void krad_interweb_server_destroy (kr_interweb_server_t **server);
void krad_interweb_server_run (kr_interweb_server_t *server);
kr_interweb_server_t *krad_interweb_server_create (char *sysname, int32_t port,
 char *headcode, char *htmlheader, char *htmlfooter);
#endif
