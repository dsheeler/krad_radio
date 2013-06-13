#include "krad_interweb.h"

#include "krad_radio.html.h"
#include "kr_api.js.h"
#include "kr_dev_interface.js.h"
#include "kr_interface.js.h"

uint32_t interweb_ws_pack_frame_header(uint8_t *out, uint32_t size);
int32_t interweb_client_get_stream(kr_iws_client_t *client);
static void krad_interweb_pack_buffer(kr_iws_client_t *client, void *buffer,
 size_t length);

static void krad_interweb_pack_buffer(kr_iws_client_t *client, void *buffer,
 size_t length) {
  kr_io2_pack(client->out, buffer, length);
}

void interweb_ws_pack(kr_iws_client_t *client, uint8_t *buffer, size_t len);
static int handle_json(kr_iws_client_t *client, char *json, size_t len);

int strmatch(char *string1, char *string2) {
  
  int len1;
  
  if ((string1 == NULL) || (string2 == NULL)) {
    if ((string1 == NULL) && (string2 == NULL)) {
      return 1;
    }
    return 0;
  } 

  len1 = strlen (string1);

  if (len1 == strlen(string2)) {
    if (strncmp(string1, string2, len1) == 0) {
      return 1;
    }
  }
  return 0;
}

#include "socket.c"
#include "websocket.c"
#include "json.c"
#include "setup.c"
#include "header_out.c"
#include "request.c"
#include "stream.c"
#include "file.c"

int32_t krad_interweb_client_handle(kr_iws_client_t *client) {

  int32_t ret;

  ret = -1;

  if (client->type == INTERWEB_UNKNOWN) {
    ret = krad_interweb_client_handle_request(client);
    if (client->type == INTERWEB_UNKNOWN) {
      return ret;
    }
  }

  switch (client->type) {
    case KR_IWS_WS:
      ret = krad_interweb_ws_client_handle(client);
      break;
    case KR_IWS_FILE:
      ret = krad_interweb_file_client_handle(client);
      break;
    case KR_IWS_STREAM_IN:
      ret = krad_interweb_stream_in_client_handle(client);
      break;
    default:
      break;
  }

  return ret;
}

static kr_iws_client_t *kr_iws_accept_client(kr_iws_t *server, int sd) {

  kr_iws_client_t *client;
  int outsize;
  int i;
  struct sockaddr_un sin;
  socklen_t sin_len;

  client = NULL;

  outsize = MAX(server->api_js_len, server->html_len);
  outsize = MAX(outsize + server->deviface_js_len,
   outsize + server->iface_js_len);
  outsize += 1024;
  outsize += outsize % 1024;

  while (client == NULL) {
    for(i = 0; i < KR_IWS_MAX_CLIENTS; i++) {
      if (server->clients[i].sd == 0) {
        client = &server->clients[i];
        break;
      }
    }
    if (client == NULL) {
      //printk ("Krad Interweb Server: Overloaded with clients!\n");
      return NULL;
    }
  }
  sin_len = sizeof(sin);
  client->sd = accept(sd, (struct sockaddr *)&sin, &sin_len);
  if (client->sd > -1) {
    krad_system_set_socket_nonblocking(client->sd);
    client->in = kr_io2_create();
    client->out = kr_io2_create_size(outsize);
    kr_io2_set_fd(client->in, client->sd);
    kr_io2_set_fd(client->out, client->sd);
    client->server = server;
    printk ("Krad Interweb Server: Client accepted!");  
    return client;
  } else {
    printke ("Krad Interweb Server: Client NOT accepted!");
    client->sd = 0;
  }

  return NULL;
}

static void krad_interweb_disconnect_client(kr_interweb_server_t *server,
 kr_iws_client_t *client) {

  close(client->sd);
  client->sd = 0;
  client->type = 0;
  client->drop_after_sync = 0;
  client->hdr_le = 0;
  client->hdr_pos = 0;
  client->hdrs_recvd = 0;
  client->verb = 0;
  memset(&client->ws, 0, sizeof(interwebs_t));
  memset(client->get, 0, sizeof(client->get));
  kr_io2_destroy(&client->in);
  kr_io2_destroy(&client->out);
  if (client->ws.krclient != NULL) {
    kr_client_destroy (&client->ws.krclient);
  }
  printk ("Krad Interweb Server: Client Disconnected");
}

static void krad_interweb_server_update_pollfds(kr_interweb_server_t *server) {

  int r;
  int c;
  int s;

  s = 0;
  server->sockets[s].fd = krad_controller_get_client_fd(&server->krad_control);
  server->sockets[s].events = POLLIN;
  s++;
  for (r = 0; r < MAX_REMOTES; r++) {
    if (server->tcp_sd[r] != 0) {
      server->sockets[s].fd = server->tcp_sd[r];
      server->sockets[s].events = POLLIN;
      s++;
      server->socket_type[s] = KR_REMOTE_LISTEN;
    }
  }
  for (c = 0; c < KR_IWS_MAX_KRCLIENTS; c++) {
    if ((server->clients[c].sd > 0) && (server->clients[c].ws.krclient != NULL)) {
      server->sockets[s].fd = kr_client_get_fd(server->clients[c].ws.krclient);
      server->sockets[s].events = POLLIN;
      //if (kr_io2_want_out (server->clients[c].out)) {
      //  server->sockets[s].events |= POLLOUT;
      //}
      server->sockets_clients[s] = &server->clients[c];
      server->socket_type[s] = KR_APP;
      s++;
    }
  }
  for (c = 0; c < KR_IWS_MAX_CLIENTS; c++) {
    if (server->clients[c].sd > 0) {
      server->sockets[s].fd = server->clients[c].sd;
      server->sockets[s].events |= POLLIN;
      if (kr_io2_want_out (server->clients[c].out)) {
        server->sockets[s].events |= POLLOUT;
      }
      server->sockets_clients[s] = &server->clients[c];
      server->socket_type[s] = server->clients[c].type;
      s++;
    }
  }
  server->socket_count = s;
  //printk ("Krad Interweb Server: sockets rejiggerd!\n");  
}

static void *krad_interweb_server_loop(void *arg) {

  kr_interweb_server_t *server = (kr_interweb_server_t *)arg;
  kr_iws_client_t *client;
  int32_t oret;
  int32_t ret;
  int32_t s;
  int32_t r;
  int32_t read_ret;

  krad_system_set_thread_name ("kr_interweb");
  server->shutdown = KRAD_INTERWEB_RUNNING;

  while (!server->shutdown) {
    s = 0;
    krad_interweb_server_update_pollfds(server);
    ret = poll(server->sockets, server->socket_count, -1);
    if ((ret < 1) || (server->shutdown) || (server->sockets[s].revents)) {
      break;
    }
    s++;
    for (r = 0; r < MAX_REMOTES; r++) {
      if (server->tcp_sd[r] != 0) {
        if ((server->tcp_sd[r] != 0)
            && (server->sockets[s].revents & POLLIN)) {
          kr_iws_accept_client(server, server->tcp_sd[r]);
          ret--;
        }
        s++;
      }
    }
    for (; ret > 0; s++) {
      if (server->sockets[s].revents) {
        ret--;
        client = server->sockets_clients[s];
        if (server->socket_type[s] == KR_APP) {
          if (server->sockets[s].revents & POLLIN) {
            krad_delivery_handler(client);
          }
          continue;
        }
        if (server->sockets[s].revents & POLLIN) {
          read_ret = kr_io2_read(client->in);
          if (read_ret > 0) {
            if (krad_interweb_client_handle(client) < 0) {
              krad_interweb_disconnect_client(server, client);
              continue;
            }
            if (kr_io2_want_out (client->out)) {
              server->sockets[s].events |= POLLOUT;
            }
          } else {
            if (read_ret == 0) {
              //printk ("Krad Interweb Server: Client EOF\n");
            }
            if (read_ret == -1) {
              printke("Krad Interweb Server: Client Socket Error");
            }
            krad_interweb_disconnect_client(server, client);
            continue;
          }
        }
        if (server->sockets[s].revents & POLLOUT) {
          oret = kr_io2_output(client->out);
          if (oret != 0) {
            printke ("panic dropping the client");
            krad_interweb_disconnect_client(server, client);
            continue;
          }
          if (!(kr_io2_want_out (client->out))) {
            if (client->drop_after_sync == 1) {
              krad_interweb_disconnect_client(server, client);
              continue;
            }
            server->sockets[s].events = POLLIN;
          }
        } else {
          if (server->sockets[s].revents & POLLHUP) {
            //printk ("Krad Interweb Server %d : POLLHUP\n", s);
            krad_interweb_disconnect_client(server, client);
            continue;
          }
        }
        if (server->sockets[s].revents & POLLERR) {
          printke("Krad Interweb Server: POLLERR");
          krad_interweb_disconnect_client(server, client);
          continue;
        }
      }
    }
  }
  server->shutdown = KRAD_INTERWEB_SHUTINGDOWN;
  krad_controller_client_close(&server->krad_control);
  return NULL;
}

void krad_interweb_server_disable (kr_interweb_t *server) {
  printk ("Krad Interweb Server: Disable Started");
  if (!krad_controller_shutdown(&server->krad_control, &server->server_thread,
      30)) {
    krad_controller_destroy(&server->krad_control, &server->server_thread);
  }
  krad_interweb_server_listen_off(server, "", 0);
  printk ("Krad Interweb Server: Disable Complete");
}

void krad_interweb_server_destroy(kr_interweb_t *server) {

  int i;

  printk("Krad Interweb Server: Destroy Started");
  if (server->shutdown != KRAD_INTERWEB_SHUTINGDOWN) {
    krad_interweb_server_disable(server);
  }
  for (i = 0; i < KR_IWS_MAX_CLIENTS; i++) {
    if (server->clients[i].sd > 0) {
      krad_interweb_disconnect_client(server, &server->clients[i]);
    }
  }
  free(server->clients);
  free(server);
  printk ("Krad Interweb Server: Destroy Completed");
}

void krad_interweb_server_run (kr_interweb_server_t *server) {
  pthread_create(&server->server_thread, NULL, krad_interweb_server_loop,
   (void *)server);
}

kr_interweb_server_t *krad_interweb_server_create (char *sysname, int32_t port,
 char *headcode, char *htmlheader, char *htmlfooter) {

  kr_interweb_server_t *server;

  server = calloc(1, sizeof (kr_interweb_server_t));
  if (krad_control_init(&server->krad_control)) {
    free(server);
    return NULL;
  }
  strcpy(server->sysname, sysname);
  server->uberport = port;
  server->headcode_source = headcode;
  server->htmlheader_source = htmlheader;
  server->htmlfooter_source = htmlfooter;
  server->shutdown = KRAD_INTERWEB_STARTING;
  server->clients = calloc(KR_IWS_MAX_CLIENTS, sizeof(kr_iws_client_t));
  kr_interweb_server_setup_html (server);
  krad_interweb_server_listen_on(server, NULL, port);
  krad_interweb_server_run(server);

  return server;
}
