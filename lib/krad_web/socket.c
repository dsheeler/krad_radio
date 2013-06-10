int set_socket_nodelay(int sd) {

  int ret;
  const int val = 1;

  ret = setsockopt(sd, IPPROTO_TCP, TCP_NODELAY, (const void *)&val,
   (socklen_t)sizeof(int));

  if (ret != 0) {
    printke("Could not set socket to TCP_NODELAY");
  }
  return ret;
}

static int kr_interweb_server_socket_setup(char *interface, int port) {

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
int krad_interweb_server_listen_on_adapter (krad_iws_t *server, char *adapter,
 int32_t port) {

  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];
  int ret;
  int ifs;

  ret = 0;
  ifs = 0;

  if (getifaddrs(&ifaddr) == -1) {
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
        printke("getnameinfo() failed: %s\n", gai_strerror(s));
        return -1;
      }
      ret = krad_interweb_server_listen_on(server, host, port);
      if (ret == 1) {
        ifs++;
      }
    }
  }
  freeifaddrs(ifaddr);
  return ifs;
}
#endif

int krad_interweb_server_listen_off (krad_iws_t *server, char *interface,
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
          
      close(server->tcp_sd[r]);
      server->tcp_sd[r] = 0;
      d++;
      printk("Disabled interweb on %s port %d",
             server->tcp_interface[r], server->tcp_port[r]);
      server->tcp_port[r] = 0;
      free(server->tcp_interface[r]);
    }
  }

  return d;
}

int krad_interweb_server_listen_on(kr_interweb_server_t *server,
 char *interface, int32_t port) {

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
