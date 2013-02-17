#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <pthread.h>

#ifndef KRAD_HTTP_H
#define KRAD_HTTP_H

typedef struct krad_http_St krad_http_t;
typedef struct krad_http_client_St krad_http_client_t;

#include "kr_client.h"

struct krad_http_St {

  int port;
  
  int listenfd;
  int socketfd;
  char *homedir;
  
  int shutdown;
  
  int websocket_port;

  char *headcode_source;
  char *htmlheader_source;
  char *htmlfooter_source;

  char *html;
  int html_len;
  char *js;
  int js_len;
  
  char *headcode;
  char *htmlheader;
  char *htmlfooter;  
  
  pthread_t server_thread;  
  krad_control_t krad_control;
};

struct krad_http_client_St {
  krad_http_t *krad_http;
  pthread_t client_thread;
  int sd;
};


krad_http_t *krad_http_server_create (int port, int websocket_port,
                                      char *headcode, char *htmlheader, char *htmlfooter);
void krad_http_server_destroy (krad_http_t *krad_http);

#endif
