#include "krad_app_server.h"

static kr_app_server *krad_app_server_init (char *appname, char *sysname);
static void *krad_app_server_run_thread (void *arg);
static int kr_app_servercp_socket_create_and_bind (char *interface, int port);
static void krad_app_disconnect_client (kr_app_server *krad_app_server, kr_app_server_client *client);
static void krad_app_server_update_pollfds (kr_app_server *krad_app_server);
static kr_app_server_client *krad_app_server_accept_client (kr_app_server *krad_app_server, int sd);

static krad_app_broadcaster_t *krad_app_server_broadcaster_create ( kr_app_server *app_server );
static int krad_app_server_broadcaster_destroy ( krad_app_broadcaster_t **broadcaster );

static kr_app_server *krad_app_server_init (char *appname, char *sysname) {

  kr_app_server *krad_app_server;
  socklen_t socket_sz;
  krad_app_server = calloc(1, sizeof(kr_app_server));

  if (krad_control_init(&krad_app_server->krad_control)) {
    free(krad_app_server);
    return NULL;
  }

  krad_app_server->shutdown = KRAD_APP_STARTING;

  krad_app_server->clients = calloc (KRAD_APP_SERVER_MAX_CLIENTS, sizeof (kr_app_server_client));

  uname (&krad_app_server->unixname);
  if (strncmp(krad_app_server->unixname.sysname, "Linux", 5) == 0) {
    krad_app_server->on_linux = 1;
  }

  krad_app_server->sd = socket (AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);

  if (krad_app_server->sd == -1) {
    printke ("Krad APP Server: Socket failed.\n");
    krad_app_server_destroy (krad_app_server);
    return NULL;
  }

  krad_app_server->saddr.sun_family = AF_UNIX;

  if (krad_app_server->on_linux) {
    snprintf (krad_app_server->saddr.sun_path, sizeof (krad_app_server->saddr.sun_path), "@%s_%s_api", appname, sysname);
    socket_sz = sizeof(krad_app_server->saddr) - sizeof(krad_app_server->saddr.sun_path) + strlen(krad_app_server->saddr.sun_path);
    krad_app_server->saddr.sun_path[0] = '\0';
    if (connect (krad_app_server->sd, (struct sockaddr *) &krad_app_server->saddr, socket_sz) != -1) {
      /* active socket already exists! */
      printke ("Krad APP Server: Krad APP path in use. (linux abstract)\n");
      krad_app_server_destroy (krad_app_server);
      return NULL;
    }
  } else {
    snprintf (krad_app_server->saddr.sun_path, sizeof (krad_app_server->saddr.sun_path), "%s/%s_%s_api", "/tmp", appname, sysname);
    socket_sz = sizeof(krad_app_server->saddr) - sizeof(krad_app_server->saddr.sun_path) + strlen(krad_app_server->saddr.sun_path);
    if (access (krad_app_server->saddr.sun_path, F_OK) == 0) {
      if (connect (krad_app_server->sd, (struct sockaddr *) &krad_app_server->saddr, socket_sz) != -1) {
        /* active socket already exists! */
        printke ("Krad APP Server: Krad APP path in use.\n");
        krad_app_server_destroy (krad_app_server);
        return NULL;
      }
      /* remove stale socket */
      unlink (krad_app_server->saddr.sun_path);
    }
  }

  if (bind (krad_app_server->sd, (struct sockaddr *) &krad_app_server->saddr, socket_sz) == -1) {
    printke ("Krad APP Server: Can't bind.\n");
    krad_app_server_destroy (krad_app_server);
    return NULL;
  }

  listen (krad_app_server->sd, SOMAXCONN);

  return krad_app_server;
}

static int kr_app_servercp_socket_create_and_bind (char *interface, int port) {

  char port_string[6];
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s;
  int sfd = 0;
  char *interface_actual;

  interface_actual = interface;

  printk ("Krad APP Server: interface: %s port %d", interface, port);

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

int krad_app_server_recvfd (kr_app_server_client *client) {

  int n;
  int fd;
  int ret;
  struct pollfd sockets[1];
  char buf[1];
  struct iovec iov;
  struct msghdr msg;
  struct cmsghdr *cmsg;
  char cms[CMSG_SPACE(sizeof(int))];

  sockets[0].fd = client->sd;
  sockets[0].events = POLLIN;

  ret = poll (sockets, 1, 100);

  if (ret > 0) {

    iov.iov_base = buf;
    iov.iov_len = 1;

    memset(&msg, 0, sizeof msg);
    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    msg.msg_control = (caddr_t)cms;
    msg.msg_controllen = sizeof cms;

    if((n=recvmsg(client->sd, &msg, 0)) < 0)
        return -1;
    if(n == 0){
        printke("krad_app_server_recvfd: unexpected EOF");
        return 0;
    }
    cmsg = CMSG_FIRSTHDR(&msg);
    memmove(&fd, CMSG_DATA(cmsg), sizeof(int));
    return fd;
  } else {
    return 0;
  }
}

int krad_app_server_disable_remote (kr_app_server *app_server,
                                    char *interface,
                                    int port) {

  //FIXME needs to loop thru clients and disconnect remote ones .. optionally?

  // Note: This removes exactly one at a time in the case of multiple matches..

  int r;
  int d;

  d = -1;

  if ((interface == NULL) || (strlen(interface) == 0)) {
    interface = "All";
  }

  for (r = 0; r < MAX_REMOTES; r++) {
    if ((app_server->tcp_sd[r] != 0) && ((port == 0) ||
        (app_server->tcp_port[r] == port))) {
      close (app_server->tcp_sd[r]);
      app_server->tcp_port[r] = 0;
      app_server->tcp_sd[r] = 0;
      free (app_server->tcp_interface[r]);
      d = r;
      break;
    }
  }

  if (d > -1) {
    printk ("Disable remote on interface %s port %d", interface, port);
    //app_server_update_pollfds (app_server);
  }

  return d;
}

#ifdef KR_LINUX
int krad_app_server_enable_remote_on_adapter (kr_app_server *app_server,
                                              char *adapter,
                                              uint16_t port) {

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
      ret = krad_app_server_enable_remote (app_server, host, port);
      if (ret == 1) {
        ifs++;
      }
    }
  }

  freeifaddrs (ifaddr);

  return ifs;
}
#endif

int krad_app_server_enable_remote (kr_app_server *krad_app_server,
                                   char *interface,
                                   uint16_t port) {

  int r;
  int sd;

  sd = 0;

  //FIXME handle when an adapter such as eth0 is specified or when interface is null
  // bind to all ips on that port

  if ((interface == NULL) || (strlen(interface) == 0)) {
    interface = "[::]";
  } else {
    #ifdef KR_LINUX
    if (krad_system_is_adapter (interface)) {
      //printk ("Krad APP Server: its an adapter, we should probably bind to all ips of this adapter");
      return krad_app_server_enable_remote_on_adapter (krad_app_server, interface, port);
    }
    #else
      return 0;
    #endif
  }

  for (r = 0; r < MAX_REMOTES; r++) {
    if ((krad_app_server->tcp_sd[r] != 0) && (krad_app_server->tcp_port[r] == port)) {
      if ((strlen(interface) == strlen(krad_app_server->tcp_interface[r])) &&
          (strncmp(interface, krad_app_server->tcp_interface[r], strlen(interface)))) {
        return 0;
      }
    }
  }

  for (r = 0; r < MAX_REMOTES; r++) {
    if ((krad_app_server->tcp_sd[r] == 0) && (krad_app_server->tcp_port[r] == 0)) {

      sd = kr_app_servercp_socket_create_and_bind (interface, port);

      if (sd < 0) {
        return 0;
      }

      krad_app_server->tcp_port[r] = port;
      krad_app_server->tcp_sd[r] = sd;

      if (krad_app_server->tcp_sd[r] != 0) {
        listen (krad_app_server->tcp_sd[r], SOMAXCONN);
        //krad_app_server_update_pollfds (krad_app_server);
        krad_app_server->tcp_interface[r] = strdup (interface);
        printk ("Enable remote on interface %s port %d", interface, port);
        return 1;
      } else {
        krad_app_server->tcp_port[r] = 0;
        return 0;
      }
    }
  }

  return 0;
}

static kr_app_server_client *krad_app_server_accept_client (kr_app_server *krad_app_server, int sd) {

  kr_app_server_client *client = NULL;

  int i;
  struct sockaddr_un sin;
  socklen_t sin_len;

  while (client == NULL) {
    for(i = 0; i < KRAD_APP_SERVER_MAX_CLIENTS; i++) {
      if (krad_app_server->clients[i].sd == 0) {
        client = &krad_app_server->clients[i];
        break;
      }
    }
    if (client == NULL) {
      //printk ("Krad APP Server: Overloaded with clients!\n");
      return NULL;
    }
  }

  sin_len = sizeof (sin);
  client->sd = accept (sd, (struct sockaddr *)&sin, &sin_len);

  if (client->sd > 0) {
    //krad_app_server_update_pollfds (krad_app_server);
    krad_system_set_socket_nonblocking(client->sd);
    client->in = kr_io2_create();
    client->out = kr_io2_create();
    kr_io2_set_fd(client->in, client->sd);
    kr_io2_set_fd(client->out, client->sd);
    client->ptr = krad_app_server->client_create(krad_app_server->pointer);
    krad_app_server->num_clients++;
    //printk ("Krad APP Server: Client accepted!");
    return client;
  } else {
    printke ("Krad APP Server: Client NOT accepted!");
  }

  return NULL;
}

static void krad_app_disconnect_client(kr_app_server *krad_app_server, kr_app_server_client *client) {

  close(client->sd);
  client->sd = 0;
  client->broadcasts = 0;
  krad_app_server->client_destroy(client->ptr);
  client->ptr = NULL;
  kr_io2_destroy(&client->in);
  kr_io2_destroy(&client->out);
  krad_app_server->num_clients--;
  //krad_app_server_update_pollfds (krad_app_server);
  //printk ("Krad APP Server: Client Disconnected");
}

uint32_t krad_app_server_num_clients(kr_app_server *app_server) {
  return app_server->num_clients;
}

static void krad_app_server_update_pollfds (kr_app_server *krad_app_server) {

  int r;
  int b;
  int c;
  int s;

  s = 0;

  krad_app_server->sockets[s].fd = krad_controller_get_client_fd (&krad_app_server->krad_control);
  krad_app_server->sockets[s].events = POLLIN;

  s++;

  krad_app_server->sockets[s].fd = krad_app_server->sd;
  krad_app_server->sockets[s].events = POLLIN;

  s++;

  for (r = 0; r < MAX_REMOTES; r++) {
    if (krad_app_server->tcp_sd[r] != 0) {
      krad_app_server->sockets[s].fd = krad_app_server->tcp_sd[r];
      krad_app_server->sockets[s].events = POLLIN;
      s++;
    }
  }

  for (b = 0; b < MAX_BROADCASTERS + MAX_REMOTES + 2; b++) {
    krad_app_server->sockets_broadcasters[b] = NULL;
  }

  for (b = 0; b < MAX_BROADCASTERS; b++) {
    if (krad_app_server->broadcasters[b] != NULL) {
      krad_app_server->sockets[s].fd = krad_app_server->broadcasters[b]->sockets[1];
      krad_app_server->sockets[s].events |= POLLIN;
      krad_app_server->sockets_broadcasters[s] = krad_app_server->broadcasters[b];
      s++;
    }
  }

  for (c = 0; c < KRAD_APP_SERVER_MAX_CLIENTS; c++) {
    if (krad_app_server->clients[c].sd > 0) {
      krad_app_server->sockets[s].fd = krad_app_server->clients[c].sd;
      krad_app_server->sockets[s].events |= POLLIN;
      if (kr_io2_want_out (krad_app_server->clients[c].out)) {
        krad_app_server->sockets[s].events |= POLLOUT;
      }
      krad_app_server->sockets_clients[s] = &krad_app_server->clients[c];
      s++;
    }
  }

  krad_app_server->socket_count = s;

  //printk ("Krad APP Server: sockets rejiggerd!\n");
}

static krad_app_broadcaster_t *krad_app_server_broadcaster_create ( kr_app_server *app_server ) {

  krad_app_broadcaster_t *broadcaster;

  if (app_server == NULL) {
    return NULL;
  }

  broadcaster = calloc (1, sizeof(krad_app_broadcaster_t));
  broadcaster->app = app_server;

  if (socketpair(AF_UNIX, SOCK_STREAM, 0, broadcaster->sockets)) {
    printke ("Krad APP Server: can't socketpair errno: %d", errno);
    free (broadcaster);
    return NULL;
  }

  krad_system_set_socket_nonblocking (broadcaster->sockets[0]);
  krad_system_set_socket_nonblocking (broadcaster->sockets[1]);

  broadcaster->msg_ring = krad_ringbuffer_create ( 200 * sizeof(krad_broadcast_msg_t *) );

  return broadcaster;
}

static int krad_app_server_broadcaster_destroy ( krad_app_broadcaster_t **broadcaster ) {
  if (*broadcaster != NULL) {

    close ((*broadcaster)->sockets[0]);
    close ((*broadcaster)->sockets[1]);

    if ((*broadcaster)->msg_ring != NULL) {
      krad_ringbuffer_free ( (*broadcaster)->msg_ring );
      (*broadcaster)->msg_ring = NULL;
    }
    free (*broadcaster);
    *broadcaster = NULL;
    return 1;
  }
  return -1;
}

int krad_app_server_broadcaster_unregister ( krad_app_broadcaster_t **broadcaster ) {
  return 1;
}

krad_app_broadcaster_t *krad_app_server_broadcaster_register ( kr_app_server *app_server ) {

  int b;

  if (app_server == NULL) {
    return NULL;
  }
  if (app_server->broadcasters_count == MAX_BROADCASTERS) {
    return NULL;
  }

  for (b = 0; b < MAX_BROADCASTERS; b++) {
    if (app_server->broadcasters[b] == NULL) {
      app_server->broadcasters[b] = krad_app_server_broadcaster_create ( app_server );
      return app_server->broadcasters[b];
    }
  }

  return NULL;
}

int krad_broadcast_msg_destroy (krad_broadcast_msg_t **broadcast_msg) {

  if (*broadcast_msg != NULL) {
    if ((*broadcast_msg)->buffer != NULL) {
      free ( (*broadcast_msg)->buffer );
    }
    free (*broadcast_msg);
    *broadcast_msg = NULL;
    return 1;
  }
  return -1;
}

krad_broadcast_msg_t *krad_broadcast_msg_create (krad_app_broadcaster_t *broadcaster, unsigned char *buffer, uint32_t size) {

  krad_broadcast_msg_t *broadcast_msg;

  if (size < 1) {
    return NULL;
  }

  broadcast_msg = calloc (1, sizeof(krad_broadcast_msg_t));

  broadcast_msg->size = size;
  broadcast_msg->buffer = malloc (broadcast_msg->size);
  memcpy (broadcast_msg->buffer, buffer, broadcast_msg->size);

  return broadcast_msg;
}

int krad_app_server_broadcaster_broadcast ( krad_app_broadcaster_t *broadcaster, krad_broadcast_msg_t **broadcast_msg ) {

  int ret;

  if (krad_ringbuffer_write_space (broadcaster->msg_ring) < sizeof(krad_broadcast_msg_t *)) {
    printke ("ahh fraking out of bradcast space");
    return -1;
  }

  ret = krad_ringbuffer_write ( broadcaster->msg_ring, (char *)broadcast_msg, sizeof(krad_broadcast_msg_t *) );
  if (ret != sizeof(krad_broadcast_msg_t *)) {
    printke ("Krad APP Server: invalid broadcast msg write len %d... broadcast ringbuffer full", ret);
    return -1;
  }

  ret = write (broadcaster->sockets[0], "0", 1);
  if (ret == 1) {
    return 1;
  } else {
    printke ("Krad APP Server: some error broadcasting: %d", ret);
  }

  return 0;
}

int krad_app_server_current_client_is_subscriber (kr_app_server *app) {

  if (app->current_client->broadcasts > 0) {
    return 1;
  }

  return 0;
}

void krad_app_server_broadcaster_register_broadcast ( krad_app_broadcaster_t *broadcaster, uint32_t broadcast_ebml_id ) {

  broadcaster->app->broadcasts[broadcaster->app->broadcasts_count] = broadcast_ebml_id;
  broadcaster->app->broadcasts_count++;

}

void krad_app_server_add_client_to_broadcast ( kr_app_server *krad_app_server, uint32_t broadcast_ebml_id ) {

  if (broadcast_ebml_id == EBML_ID_KRAD_RADIO_GLOBAL_BROADCAST) {
    printk("client subscribing to global broadcasts");
  }

  krad_app_server->current_client->broadcasts = 1;
}

int handle_broadcast (krad_app_broadcaster_t *broadcaster) {

  int item;
  int items;
  int b;
  int c;
  int ret;
  char buf[32];
  krad_broadcast_msg_t *broadcast_msg;

  b = 0;

  items = read (broadcaster->sockets[1], buf, 32);

  if (items < 1) {
    printke ("Error handling broadcast");
    return -1;
  }

  for (item = 0; item < items; item++) {
    ret = krad_ringbuffer_read ( broadcaster->msg_ring, (char *)&broadcast_msg, sizeof(krad_broadcast_msg_t *) );
    if (ret != sizeof(krad_broadcast_msg_t *)) {
      printke ("Krad APP Server: invalid broadcast msg read len %d", ret);
      return -1;
    }

    //if (broadcast_msg->skip_client != NULL) {
    //  printk ("Goint to skip a client!!\n");
    //}

    for (c = 0; c < KRAD_APP_SERVER_MAX_CLIENTS; c++) {
      if (broadcaster->app->clients[c].broadcasts == 1) {
        if (&broadcaster->app->clients[c] != broadcast_msg->skip_client) {
          //FIXME prevent overpacking overflow
          kr_io2_pack (broadcaster->app->clients[c].out, broadcast_msg->buffer, broadcast_msg->size);
          b++;
        }
      }
    }

    //printk ("Krad APP Server: Broadcasted to %d", b);

    krad_broadcast_msg_destroy (&broadcast_msg);
  }

  return items;
}

static void *krad_app_server_run_thread (void *arg) {

  kr_app_server *krad_app_server = (kr_app_server *)arg;
  kr_app_server_client *client;
  int ret, s, r, read_ret, hret;
  int32_t oret;

  krad_system_set_thread_name ("kr_station");
  krad_app_server->shutdown = KRAD_APP_RUNNING;

  while (!krad_app_server->shutdown) {

    s = 0;
    krad_app_server_update_pollfds (krad_app_server);
    ret = poll (krad_app_server->sockets, krad_app_server->socket_count, -1);

    if ((ret < 1) ||
        (krad_app_server->shutdown) ||
        (krad_app_server->sockets[s].revents)) {
      break;
    }

    s++;
    // Unix socket for local connections
    if (krad_app_server->sockets[s].revents & POLLIN) {
      krad_app_server_accept_client (krad_app_server, krad_app_server->sd);
      ret--;
    }

    s++;
    // TCP Sockets for remote connections
    for (r = 0; r < MAX_REMOTES; r++) {
      if (krad_app_server->tcp_sd[r] != 0) {
        if ((krad_app_server->tcp_sd[r] != 0) && (krad_app_server->sockets[s].revents & POLLIN)) {
          krad_app_server_accept_client (krad_app_server, krad_app_server->tcp_sd[r]);
          ret--;
        }
        s++;
      }
    }

    for (; ret > 0; s++) {
      if (krad_app_server->sockets[s].revents) {
        ret--;
        if (krad_app_server->sockets_broadcasters[s] != NULL) {
          handle_broadcast ( krad_app_server->sockets_broadcasters[s] );
        } else {
          client = krad_app_server->sockets_clients[s];
          if (krad_app_server->sockets[s].revents & POLLIN) {
            read_ret = kr_io2_read (client->in);
            if (read_ret > 0) {
              //printk ("Krad APP Server %d: Got %d bytes\n", s, read_ret);
              krad_app_server->current_client = client;
              hret = krad_app_server->client_handler(client->in, client->out,
               client->ptr);
              if (hret != 0) {
                krad_app_disconnect_client (krad_app_server, client);
                continue;
              } else {
                if (kr_io2_want_out (client->out)) {
                  krad_app_server->sockets[s].events |= POLLOUT;
                }
              }
            } else {
              if (read_ret == 0) {
                //printk ("Krad APP Server: Client EOF\n");
                krad_app_disconnect_client (krad_app_server, client);
                continue;
              }
              if (read_ret == -1) {
                printke ("Krad APP Server: Client Socket Error");
                krad_app_disconnect_client (krad_app_server, client);
                continue;
              }
            }
          }
          if (krad_app_server->sockets[s].revents & POLLOUT) {
            oret = kr_io2_output (client->out);
            if (oret != 0) {
              printke ("Krad APP Server: panic drop");
              krad_app_disconnect_client (krad_app_server, client);
              continue;
            }
            if (!(kr_io2_want_out (client->out))) {
              krad_app_server->sockets[s].events = POLLIN;
            }
          } else {
            if (krad_app_server->sockets[s].revents & POLLHUP) {
              //printk ("Krad APP Server %d : POLLHUP\n", s);
              krad_app_disconnect_client (krad_app_server, client);
              continue;
            }
          }
          if (krad_app_server->sockets[s].revents & POLLERR) {
            printke ("Krad APP Server: POLLERR");
            krad_app_disconnect_client (krad_app_server, client);
            continue;
          }
        }
      }
    }
  }

  krad_app_server->shutdown = KRAD_APP_SHUTINGDOWN;

  krad_controller_client_close (&krad_app_server->krad_control);

  return NULL;
}

void krad_app_server_disable (kr_app_server *krad_app_server) {

  printk ("Krad APP Server: Disable Started");

  if (!krad_controller_shutdown (&krad_app_server->krad_control, &krad_app_server->server_thread, 30)) {
    krad_controller_destroy (&krad_app_server->krad_control, &krad_app_server->server_thread);
  }

  krad_app_server_disable_remote (krad_app_server, "", 0);

  if (krad_app_server->sd != 0) {
    close (krad_app_server->sd);
    if (!krad_app_server->on_linux) {
      unlink (krad_app_server->saddr.sun_path);
    }
  }

  printk ("Krad APP Server: Disable Complete");
}

void krad_app_server_destroy (kr_app_server *app_server) {

  int i;

  printk ("Krad APP Server: Destroy Started");

  if (app_server->shutdown != KRAD_APP_SHUTINGDOWN) {
    krad_app_server_disable (app_server);
  }

  for (i = 0; i < KRAD_APP_SERVER_MAX_CLIENTS; i++) {
    if (app_server->clients[i].sd > 0) {
      krad_app_disconnect_client (app_server, &app_server->clients[i]);
    }
  }

  for (i = 0; i < MAX_BROADCASTERS; i++) {
    if (app_server->broadcasters[i] != NULL) {
      krad_app_server_broadcaster_destroy ( &app_server->broadcasters[i] );
    }
  }

  free (app_server->clients);
  free (app_server);

  printk ("Krad APP Server: Destroy Completed");
}

void krad_app_server_run (kr_app_server *krad_app_server) {
  krad_app_server->app_broadcaster = krad_app_server_broadcaster_register ( krad_app_server );
  pthread_create (&krad_app_server->server_thread, NULL, krad_app_server_run_thread, (void *)krad_app_server);
}

kr_app_server *
krad_app_server_create (char *appname, char *sysname,
                        void *client_create (void *),
                        void client_destroy (void *),
                        int client_handler (kr_io2_t *in,
                                            kr_io2_t *out,
                                            void *),
                        void *pointer) {

  kr_app_server *krad_app_server;

  krad_app_server = krad_app_server_init (appname, sysname);

  if (krad_app_server == NULL) {
    return NULL;
  }

  krad_app_server->client_create = client_create;
  krad_app_server->client_destroy = client_destroy;
  krad_app_server->client_handler = client_handler;

  krad_app_server->pointer = pointer;

  return krad_app_server;
}

