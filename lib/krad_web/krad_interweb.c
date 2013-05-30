#include "krad_interweb.h"

#include "krad_radio.html.h"
#include "krad_radio.js.h"

static int kr_interweb_server_socket_setup (char *interface, int port) {

  char port_string[6];
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s;
  int sfd = 0;
  char *interface_actual;
  
  interface_actual = interface;
  
  printk ("Krad Interweb Server: interface: %s port %d", interface, port);

  snprintf (port_string, 6, "%d", port);

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
  hints.ai_flags = AI_PASSIVE;     /* All interfaces */

  if ((strlen(interface) == 7) && (strncmp(interface, "0.0.0.0", 7) == 0)) {
    hints.ai_family = AF_INET;
    interface_actual = NULL;
  }
  if ((strlen(interface) == 4) && (strncmp(interface, "[::]", 4) == 0)) {
    hints.ai_family = AF_INET6;
    interface_actual = NULL;
  }

  s = getaddrinfo (interface_actual, port_string, &hints, &result);
  if (s != 0) {
    printke ("getaddrinfo: %s\n", gai_strerror (s));
    return -1;
  }

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    
    sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    
    if (sfd == -1) {
      continue;
    }

    s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
    
    if (s == 0) {
      /* We managed to bind successfully! */
      break;
    }

    close (sfd);
  }

  if (rp == NULL) {
    printke ("Could not bind %d\n", port);
    return -1;
  }

  freeaddrinfo (result);

  return sfd;
}

#ifdef KR_LINUX
int krad_interweb_server_listen_on_adapter (krad_iws_t *server,
                                            char *adapter,
                                            int32_t port) {

  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];
  int ret;
  int ifs;

  ret = 0;
  ifs = 0;

  if (getifaddrs (&ifaddr) == -1) {
    return -1;
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL) {
      continue;
    }

    family = ifa->ifa_addr->sa_family;

    if (family == AF_INET || family == AF_INET6) {
      s = getnameinfo(ifa->ifa_addr,
             (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                   sizeof(struct sockaddr_in6),
             host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        printke ("getnameinfo() failed: %s\n", gai_strerror(s));
        return -1;
      }
      ret = krad_interweb_server_listen_on (server, host, port);
      if (ret == 1) {
        ifs++;
      }
    }
  }

  freeifaddrs (ifaddr);

  return ifs;
}
#endif

int strmatch (char *string1, char *string2) {
  
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

int krad_interweb_server_listen_off (krad_iws_t *server,
                                     char *interface,
                                     int32_t port) {

  //FIXME needs to loop thru clients and disconnect remote ones .. optionally?

  int r;
  int d;
  int all_if;
  
  all_if = 0;
  d = 0;
  
  if (strlen(interface) == 0) {
    all_if = 1;
  }

  for (r = 0; r < MAX_REMOTES; r++) {
    if ((server->tcp_sd[r] != 0) &&
        ((port == 0) || (server->tcp_port[r] == port)) &&
         ((all_if == 1) || (strmatch(server->tcp_interface[r], interface)))) {
          
      close (server->tcp_sd[r]);
      server->tcp_sd[r] = 0;
      d++;
      printk ("Disabled interweb on %s port %d",
              server->tcp_interface[r], server->tcp_port[r]);

      server->tcp_port[r] = 0;
      free (server->tcp_interface[r]);
    }
  }

  return d;
}

int krad_interweb_server_listen_on (kr_interweb_server_t *server,
                                    char *interface,
                                    int32_t port) {

  int r;
  int sd;
  
  sd = 0;
  
  if ((interface == NULL) || (strlen(interface) == 0)) {
    interface = "[::]";
  } else {
    #ifdef KR_LINUX
    if (krad_system_is_adapter (interface)) {
      //printk ("Krad Interweb Server: its an adapter,
      //we should probably bind to all ips of this adapter");
      return krad_interweb_server_listen_on_adapter (server, interface, port);
    }
    #else
      return 0;
    #endif
  }

  for (r = 0; r < MAX_REMOTES; r++) {
    if ((server->tcp_sd[r] != 0) && (server->tcp_port[r] == port)) {
      if ((strlen(interface) == strlen(server->tcp_interface[r])) &&
          (strncmp(interface, server->tcp_interface[r], strlen(interface)))) {
        return 0;
      }
    }
  }  
  
  for (r = 0; r < MAX_REMOTES; r++) {
    if ((server->tcp_sd[r] == 0) && (server->tcp_port[r] == 0)) {
    
      sd = kr_interweb_server_socket_setup (interface, port);
      
      if (sd < 0) {
        return 0;
      }
      
      server->tcp_port[r] = port;
      server->tcp_sd[r] = sd;

      if (server->tcp_sd[r] != 0) {
        listen (server->tcp_sd[r], SOMAXCONN);
        //krad_interweb_server_update_pollfds (krad_interweb_server);
        server->tcp_interface[r] = strdup (interface);
        printk ("Enable remote on interface %s port %d", interface, port);
        return 1;
      } else {
        server->tcp_port[r] = 0;
        return 0;
      }
    }
  }

  return 0;
}


static kr_iws_client_t *kr_iws_accept_client (kr_iws_t *server, int sd) {

  kr_iws_client_t *client = NULL;
  
  int i;
  struct sockaddr_un sin;
  socklen_t sin_len;
    
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

  sin_len = sizeof (sin);
  client->sd = accept (sd, (struct sockaddr *)&sin, &sin_len);

  if (client->sd > 0) {
    //krad_interweb_server_update_pollfds (krad_interweb_server);
    krad_system_set_socket_nonblocking (client->sd);
    client->in = kr_io2_create ();
    client->out = kr_io2_create_size (128000);
    kr_io2_set_fd (client->in, client->sd);
    kr_io2_set_fd (client->out, client->sd);
    client->server = server;
    client->noob = 1;
    //client->ptr = server->client_create (server->pointer);
    printk ("Krad Interweb Server: Client accepted!");  
    return client;
  } else {
    printke ("Krad Interweb Server: Client NOT accepted!");  
  }

  return NULL;
}

static void krad_interweb_abandon_client (kr_interweb_server_t *server, kr_iws_client_t *client) {
  client->sd = 0;
  client->noob = 0;  
  kr_io2_destroy (&client->in);
  kr_io2_destroy (&client->out);  
}

static void krad_interweb_disconnect_client (kr_interweb_server_t *server, kr_iws_client_t *client) {

  close (client->sd);
  krad_interweb_abandon_client (server, client);
  printk ("Krad Interweb Server: Client Disconnected");
}

static void krad_interweb_server_update_pollfds (kr_interweb_server_t *server) {

  int r;
  int c;
  int s;

  s = 0;

  server->sockets[s].fd = krad_controller_get_client_fd (&server->krad_control);
  server->sockets[s].events = POLLIN;

  s++;
  
  for (r = 0; r < MAX_REMOTES; r++) {
    if (server->tcp_sd[r] != 0) {
      server->sockets[s].fd = server->tcp_sd[r];
      server->sockets[s].events = POLLIN;
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
      s++;
    }
  }
  
  server->socket_count = s;

  //printk ("Krad Interweb Server: sockets rejiggerd!\n");  
}

char *kr_interweb_server_load_file_or_string (char *input) {

  int fd;
  char *string;
  unsigned int length;
  struct stat file_stat;
  int bytes_read;
  int ret;
  
  fd = 0;
  bytes_read = 0;
  string = NULL;

  if (input == NULL) {
    return NULL;
  }

  if ((strlen(input)) && (input[0] == '/')) {

    fd = open (input, O_RDONLY);
    if (fd < 1) {
      printke("could not open");    
      return NULL;
    }
    fstat (fd, &file_stat);
    length = file_stat.st_size;
    if (length > 1000000) {
      printke("too big");
      close (fd);
      return NULL;
    }

    string = calloc (1, length);        

    while (bytes_read < length) {
    
      ret = read (fd, string + bytes_read, length - bytes_read);

      if (ret < 0) {
        printke("read fail");
        close (fd);
        free (string);
        return NULL;
      }
      bytes_read += ret;
    }
    close (fd);
    return string;
  } else {
    return strdup (input);
  }
}

void kr_interweb_server_setup_html (kr_interweb_t *server) {

  char string[64];
  char *html_template;
  int html_template_len;
  int total_len;
  int len;
  int pos;
  int template_pos;
  
  template_pos = 0;
  pos = 0;
  len = 0;
  total_len = 0;
  
  memset (string, 0, sizeof(string));
  snprintf (string, 7, "%d", server->ws_port);
  total_len += strlen(string);
  server->js = (char *)lib_krad_web_res_krad_radio_js;
  server->js_len = lib_krad_web_res_krad_radio_js_len;
  server->js[server->js_len] = '\0';
  
  html_template = (char *)lib_krad_web_res_krad_radio_html;
  html_template_len = lib_krad_web_res_krad_radio_html_len - 1;
  total_len += html_template_len - 4;

  server->headcode = kr_interweb_server_load_file_or_string (server->headcode_source);
  server->htmlheader = kr_interweb_server_load_file_or_string (server->htmlheader_source);
  server->htmlfooter = kr_interweb_server_load_file_or_string (server->htmlfooter_source);
  
  if (server->headcode != NULL) {
    total_len += strlen(server->headcode);
  }
  if (server->htmlheader != NULL) {
    total_len += strlen(server->htmlheader);    
  }
  if (server->htmlfooter != NULL) {
    total_len += strlen(server->htmlfooter);    
  }

  server->html_len = total_len + 1;
  server->html = calloc (1, server->html_len);
  
  len = strcspn (html_template, "~");
  strncpy (server->html, html_template, len);
  strcpy (server->html + len, string);
  pos = len + strlen(string);
  template_pos = len + 1;
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (server->headcode != NULL) {
    len = strlen(server->headcode);
    strncpy (server->html + pos, server->headcode, len);
    pos += len;
  }
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (server->htmlheader != NULL) {
    len = strlen(server->htmlheader);
    strncpy (server->html + pos, server->htmlheader, len);
    pos += len;
  }
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (server->htmlfooter != NULL) {
    len = strlen(server->htmlfooter);
    strncpy (server->html + pos, server->htmlfooter, len);
    pos += len;
  }
  
  len = html_template_len - template_pos;
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len;
  pos += len;  
  
  if (template_pos != html_template_len) {
    failfast("html template miscalculation: %d %d", template_pos, html_template_len);
  }  
  
  if (pos != total_len) {
    printke("html miscalculation: %d %d", pos, total_len);
  }

  server->html[total_len] = '\0';

  if (server->headcode != NULL) {
    free (server->headcode);
    server->headcode = NULL;    
  }
  if (server->htmlheader != NULL) {
    free (server->htmlheader);
    server->htmlheader = NULL;    
  }
  if (server->htmlfooter != NULL) {
    free (server->htmlfooter);
    server->htmlfooter = NULL;    
  }    
}

static void krad_interweb_pack_headers (kr_iws_client_t *client, char *content_type);
static void krad_interweb_pack_buffer (kr_iws_client_t *client, char *buffer, size_t length);

static void krad_interweb_pack_buffer (kr_iws_client_t *client, char *buffer, size_t length) {

  kr_io2_pack (client->out, buffer, length);
}

static void krad_interweb_pack_headers (kr_iws_client_t *client, char *content_type) {

  int32_t pos;
  char *buffer;
  
  pos = 0;

  buffer = (char *)client->out->buf;
  

  pos += sprintf (buffer + pos, "HTTP/1.0 200 OK\r\n");
  pos += sprintf (buffer + pos, "Status: 200 OK\r\n");
  pos += sprintf (buffer + pos, "Connection: close\r\n");
  pos += sprintf (buffer + pos, "Server: Krad-Radio\r\n");
  pos += sprintf (buffer + pos, "Content-Type: %s; charset=utf-8\r\n", content_type);
  pos += sprintf (buffer + pos, "\r\n");


  kr_io2_advance (client->out, pos);
}

static void krad_interweb_pack_404 (kr_iws_client_t *client) {

  int32_t pos;
  char *buffer;
  
  pos = 0;

  buffer = (char *)client->out->buf;
  pos += sprintf (buffer + pos, "HTTP/1.1 404 Not Found\r\n");
  pos += sprintf (buffer + pos, "Status: 404 Not Found\r\n");
  pos += sprintf (buffer + pos, "Connection: close\r\n");
  pos += sprintf (buffer + pos, "Server: Krad-Radio\r\n");  
  pos += sprintf (buffer + pos, "Content-Type: text/html; charset=utf-8\r\n");
  pos += sprintf (buffer + pos, "\r\n");
  pos += sprintf (buffer + pos, "404 Not Found");
  
  kr_io2_advance (client->out, pos);  
}

void krad_interweb_client_handle (kr_iws_client_t *client) {

  int32_t ret;
  int32_t getlen;
  int32_t pos;
  char *buffer;
  char get[256];
  
  getlen = -1;
  ret = 0;
  pos = 0;
  
  //ret = read (client->sd, buffer, BUFSIZE * 2);
  
  //if (ret < 1) {
  //printk ("failed to read browser request");
  //  krad_http_destroy_client (client);
  //}
  
  //printf ("ret is %d\n", ret);
  
  buffer = (char *)client->in->rd_buf;  
  ret = client->in->len;
           // printk ("Krad Interweb reerer Server %s: Got %d bytes\n", buffer, ret);
  while (pos < ret) {
    if (strncmp(buffer, "GET /", 5) == 0) {
      getlen = strcspn (buffer + pos + 5, "\r ?");
      memcpy (get, buffer + pos + 5, getlen);
      get[getlen] = '\0';
      break;
    } 
    pos += strcspn (buffer + pos, "\r") + 2;
  }

  if ((getlen > -1) && (getlen < 32)) {
    if (strncmp("krad_radio.js", get, 13) == 0) {
      krad_interweb_pack_headers (client, "text/javascript");
      krad_interweb_pack_buffer (client, client->server->js, client->server->js_len);
    } else {
      if ((getlen == 0) || ((getlen == 15) && (strncmp("krad_radio.html", get, 15) == 0))) {
        krad_interweb_pack_headers (client, "text/html");
        krad_interweb_pack_buffer (client, client->server->html, client->server->html_len);
      } else {
        krad_interweb_pack_404 (client);
      }
    }
  }
  
  kr_io2_pulled (client->in, ret);  
  client->drop_after_flush = 1;
}

int interweb_ws_gen_accept_header (char *resp, char *key) {

  static char *ws_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  char string[128];
  char hash[21];

  hash[20] = '\0';

  if ((resp == NULL) || (key == NULL)) {
    return -1;
  }

  snprintf(string, sizeof(string), "%s%s", key, ws_guid);
  string[127] = '\0';

  kr_sha1(string, strlen(string), hash);

  kr_base64(resp, hash, 20, 64);

  return 0;
}

int krad_interweb_ws_peek (kr_iws_client_t *client) {

  int ret;
  char buf[256];
  ret = 0;
  
  memset (buf, 0, sizeof(buf));
  
  ret = recv (client->sd, buf, sizeof(buf) - 1, MSG_PEEK);

  if ((ret > 0) && (strstr(buf, "Upgrade: websocket") != NULL)) {
    printk ("Krad Interweb websocket peek is YEAAY after %d bytes", ret);
    return 1;
  }
  printk ("Krad Interweb websocket peek is NO after %d bytes", ret);
  return 0;
}

static void *krad_interweb_server_loop (void *arg) {

  kr_interweb_server_t *server = (kr_interweb_server_t *)arg;
  kr_iws_client_t *client;
  int32_t ret, s, r, read_ret, hret;

  krad_system_set_thread_name ("kr_interweb");
  server->shutdown = KRAD_INTERWEB_RUNNING;

  hret = 0;

  while (!server->shutdown) {

    s = 0;
    krad_interweb_server_update_pollfds (server);
    ret = poll (server->sockets, server->socket_count, -1);

    if ((ret < 1) ||
        (server->shutdown) ||
        (server->sockets[s].revents)) {
      break;
    }

    s++;

    for (r = 0; r < MAX_REMOTES; r++) {
      if (server->tcp_sd[r] != 0) {
        if ((server->tcp_sd[r] != 0) && (server->sockets[s].revents & POLLIN)) {
          kr_iws_accept_client (server, server->tcp_sd[r]);
          ret--;
        }
        s++;
      }
    }

    for (; ret > 0; s++) {
      if (server->sockets[s].revents) {
        ret--;
        client = server->sockets_clients[s];
        if (server->sockets[s].revents & POLLIN) {
        
          if (client->noob == 1) {
            if (krad_interweb_ws_peek (client)) {
              //libwebsocket_shove_fd (server->ws, client->sd);
              krad_interweb_abandon_client (server, client);
              continue;
            }
            client->noob = 0;
          }
               
          read_ret = kr_io2_read (client->in);
          if (read_ret > 0) {
            printk ("Krad Interweb Server %d: Got %d bytes\n", s, read_ret);
            //server->current_client = client;
            //hret = server->client_handler (client->in, client->out, client->ptr);
            //FIXME do important interwebing here
              //krad_interweb_pack_headers (client, "text/html");
              //char *stringy = "ooohada booga dao!\n";
              //kr_io2_pack (client->out, stringy, strlen(stringy));
              krad_interweb_client_handle (client);
  
              hret = 0;
            if (hret != 0) {
              krad_interweb_disconnect_client (server, client);
              continue;
            } else {
              if (kr_io2_want_out (client->out)) {
                server->sockets[s].events |= POLLOUT;
              }
            }
          } else {
            if (read_ret == 0) {
              //printk ("Krad Interweb Server: Client EOF\n");
              krad_interweb_disconnect_client (server, client);
              continue;
            }
            if (read_ret == -1) {
              printke ("Krad Interweb Server: Client Socket Error");
              krad_interweb_disconnect_client (server, client);
              continue;
            }
          }
        }
        if (server->sockets[s].revents & POLLOUT) {
          kr_io2_output (client->out);
          if (!(kr_io2_want_out (client->out))) {
            if (client->drop_after_flush == 1) {
              client->drop_after_flush = 0;
              krad_interweb_disconnect_client (server, client);
              continue;
            }
            server->sockets[s].events = POLLIN;
          }
        } else {
          if (server->sockets[s].revents & POLLHUP) {
            //printk ("Krad Interweb Server %d : POLLHUP\n", s);
            krad_interweb_disconnect_client (server, client);
            continue;
          }
        }
        if (server->sockets[s].revents & POLLERR) {
          printke ("Krad Interweb Server: POLLERR\n");
          krad_interweb_disconnect_client (server, client);
          continue;
        }
      }
    }
  }

  server->shutdown = KRAD_INTERWEB_SHUTINGDOWN;

  krad_controller_client_close (&server->krad_control);

  return NULL;
}

void krad_interweb_server_disable (kr_interweb_t *server) {

  printk ("Krad Interweb Server: Disable Started");

  if (!krad_controller_shutdown (&server->krad_control, &server->server_thread, 30)) {
    krad_controller_destroy (&server->krad_control, &server->server_thread);
  }

  krad_interweb_server_listen_off (server, "", 0);
  
  if (server->ws != NULL) {
    krad_websocket_server_destroy (server->ws);
  }  
  
  printk ("Krad Interweb Server: Disable Complete");
}

void krad_interweb_server_destroy (kr_interweb_t *server) {

  int i;

  printk ("Krad Interweb Server: Destroy Started");

  if (server->shutdown != KRAD_INTERWEB_SHUTINGDOWN) {
    krad_interweb_server_disable (server);
  }
  
  for (i = 0; i < KR_IWS_MAX_CLIENTS; i++) {
    if (server->clients[i].sd > 0) {
      krad_interweb_disconnect_client (server, &server->clients[i]);
    }
  }
  
  free (server->clients);
  free (server);
  
  printk ("Krad Interweb Server: Destroy Completed");
}

void krad_interweb_server_run (kr_interweb_server_t *server) {
  pthread_create (&server->server_thread,
                  NULL,
                  krad_interweb_server_loop,
                  (void *)server);
}

kr_interweb_server_t * krad_interweb_server_create (char *sysname, int32_t port, int32_t websocket_port,
                                      char *headcode, char *htmlheader, char *htmlfooter) {

  kr_interweb_server_t *server;

  server = calloc (1, sizeof (kr_interweb_server_t));
  
  if (krad_control_init (&server->krad_control)) {
    return NULL;
  }

  strcpy (server->sysname, sysname);

  server->ws_port = websocket_port;
  server->headcode_source = headcode;
  server->htmlheader_source = htmlheader;
  server->htmlfooter_source = htmlfooter;
  
  server->shutdown = KRAD_INTERWEB_STARTING;
  
  server->clients = calloc (KR_IWS_MAX_CLIENTS, sizeof (kr_iws_client_t));
  
  kr_interweb_server_setup_html (server);
  
  krad_interweb_server_listen_on (server, NULL, port);

  server->ws = krad_websocket_server_create (server->sysname, server->ws_port);

  krad_interweb_server_run (server);
  

  char resp[128];

  interweb_ws_gen_accept_header (resp, "dGhlIHNhbXBsZSBub25jZQ==");

  printk ("the resp was %s", resp);

  return server;
}


/*

            if (strncmp(client->in_buffer, "SOURCE /", 8) == 0) {
              ret = strcspn (client->in_buffer + 8, "\n\r ");
              memcpy (client->mount, client->in_buffer + 8, ret);
              client->mount[ret] = '\0';
            
              printk ("Got a mount! its %s", client->mount);
            
              client->got_mount = 1;
            } else {
              printk ("client no good! %s", client->in_buffer);
              krad_receiver_destroy_client (client);              
            }
          
          } else {
            printk ("client no good! .. %s", client->in_buffer);
            krad_receiver_destroy_client (client);
          }
        } else {
          printk ("client buffer: %s", client->in_buffer);
          
          
          if (client->got_content_type == 0) {
            if (((string = strstr(client->in_buffer, "Content-Type:")) != NULL) ||
                ((string = strstr(client->in_buffer, "content-type:")) != NULL) ||
              ((string = strstr(client->in_buffer, "Content-type:")) != NULL)) {
              ret = strcspn(string + 14, "\n\r ");
              memcpy(client->content_type, string + 14, ret);
              client->content_type[ret] = '\0';
              client->got_content_type = 1;
              printk ("Got a content_type! its %s", client->content_type);              
            }
*/

