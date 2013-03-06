#include "krad_ipc_client.h"

static int krad_ipc_client_init (krad_ipc_client_t *client);

void krad_ipc_disconnect (krad_ipc_client_t *client) {
  if (client != NULL) {
    if (client->sd != 0) {
      close (client->sd);
    }
    if (client->io != NULL) {
      kr_io2_destroy (&client->io);
    }
    if (client->ebml2 != NULL) {
      kr_ebml2_destroy (&client->ebml2);
    }
    if (client->krad_ebml != NULL) {
      krad_ebml_destroy (client->krad_ebml);
    }
    free(client);
  }
}

krad_ipc_client_t *krad_ipc_connect (char *sysname) {
  
  krad_ipc_client_t *client = calloc (1, sizeof (krad_ipc_client_t));
  
  if (client == NULL) {
    failfast ("Krad IPC Client mem alloc fail");
    return NULL;
  }
  
  krad_system_init ();
  
  uname (&client->unixname);

  if (krad_valid_host_and_port (sysname)) {
    krad_get_host_and_port (sysname, client->host, &client->tcp_port);
  } else {
    strncpy (client->sysname, sysname, sizeof (client->sysname));
    if (strncmp(client->unixname.sysname, "Linux", 5) == 0) {
      client->on_linux = 1;
      client->ipc_path_pos = sprintf(client->ipc_path, "@krad_radio_%s_ipc", sysname);
    } else {
      client->ipc_path_pos = sprintf(client->ipc_path, "%s/krad_radio_%s_ipc", "/tmp", sysname);
    }
  
    if (!client->on_linux) {
      if(stat(client->ipc_path, &client->info) != 0) {
        krad_ipc_disconnect(client);
        failfast ("Krad IPC Client: IPC PATH Failure\n");
        return NULL;
      }
    }
  }
  
  if (krad_ipc_client_init (client) == 0) {
    printke ("Krad IPC Client: Failed to init!");
    krad_ipc_disconnect (client);
    return NULL;
  }

  return client;
}

static int krad_ipc_client_init (krad_ipc_client_t *client) {

  int rc;
  char port_string[6];
  struct sockaddr_un unix_saddr;
  struct in6_addr serveraddr;
  struct addrinfo hints;
  struct addrinfo *res;

  res = NULL;

  //FIXME make connect nonblocking  
  

  if (client->tcp_port != 0) {

    //FIXME hrm we don't know the sysname of a remote connect! crazy ?
    //printf ("Krad IPC Client: Connecting to remote %s:%d", client->host, client->tcp_port);

    memset(&hints, 0x00, sizeof(hints));
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    rc = inet_pton (AF_INET, client->host, &serveraddr);
    if (rc == 1) {
      hints.ai_family = AF_INET;
      hints.ai_flags |= AI_NUMERICHOST;
    } else {
      rc = inet_pton (AF_INET6, client->host, &serveraddr);
      if (rc == 1) {
        hints.ai_family = AF_INET6;
        hints.ai_flags |= AI_NUMERICHOST;
      }
    }

    snprintf (port_string, 6, "%d", client->tcp_port);

    rc = getaddrinfo (client->host, port_string, &hints, &res);
    if (rc != 0) {
       printf ("Krad IPC Client: Host not found --> %s\n", gai_strerror(rc));
       return 0;
    }
    
    client->sd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
    if (client->sd < 0) {
      printf ("Krad IPC Client: Socket Error");
      if (res != NULL) {
        freeaddrinfo (res);
        res = NULL;
      }
      return 0;
    }

    rc = connect (client->sd, res->ai_addr, res->ai_addrlen);
    if (rc < 0) {
      printf ("Krad IPC Client: Remote Connect Error\n");
      if (res != NULL) {
        freeaddrinfo (res);
        res = NULL;
      }
      return 0;
    }

    if (res != NULL) {
      freeaddrinfo (res);
      res = NULL;
    }

  } else {

    client->sd = socket (AF_UNIX, SOCK_STREAM, 0);
    if (client->sd == -1) {
      failfast ("Krad IPC Client: socket fail");
      return 0;
    }

    memset(&unix_saddr, 0x00, sizeof(unix_saddr));
    unix_saddr.sun_family = AF_UNIX;
    snprintf (unix_saddr.sun_path, sizeof(unix_saddr.sun_path), "%s", client->ipc_path);
    if (client->on_linux) {
      unix_saddr.sun_path[0] = '\0';
    }

    if (connect (client->sd, (struct sockaddr *) &unix_saddr, sizeof (unix_saddr)) == -1) {
      close (client->sd);
      client->sd = 0;
      printke ("Krad IPC Client: Can't connect to socket %s", client->ipc_path);
      return 0;
    }
  }

  client->krad_ebml = krad_ebml_open_active_socket (client->sd, KRAD_EBML_IO_READWRITE);

  client->io = kr_io2_create ();
  client->ebml2 = kr_ebml2_create ();

  kr_io2_set_fd ( client->io, client->sd );
  kr_ebml2_set_buffer ( client->ebml2, client->io->buf, client->io->space );

  kr_ebml2_pack_header (client->ebml2, KRAD_IPC_CLIENT_DOCTYPE, KRAD_IPC_DOCTYPE_VERSION, KRAD_IPC_DOCTYPE_READ_VERSION);
  kr_io2_advance (client->io, client->ebml2->pos);

  if (kr_io2_flush (client->io)) {
    close (client->sd);
    client->sd = 0;
    printke ("Krad IPC Client: io2_flush fail");
    return 0;
  }
  kr_ebml2_set_buffer ( client->ebml2, client->io->buf, client->io->space );

  krad_ebml_read_ebml_header (client->krad_ebml, client->krad_ebml->header);
  krad_ebml_check_ebml_header (client->krad_ebml->header);

  if (krad_ebml_check_doctype_header (client->krad_ebml->header, KRAD_IPC_SERVER_DOCTYPE, KRAD_IPC_DOCTYPE_VERSION, KRAD_IPC_DOCTYPE_READ_VERSION)) {
    //printf("Matched %s Version: %d Read Version: %d\n", KRAD_IPC_SERVER_DOCTYPE, KRAD_IPC_DOCTYPE_VERSION, KRAD_IPC_DOCTYPE_READ_VERSION);
  } else {
    printke ("Did Not Match %s Version: %d Read Version: %d", KRAD_IPC_SERVER_DOCTYPE, KRAD_IPC_DOCTYPE_VERSION, KRAD_IPC_DOCTYPE_READ_VERSION);
  }

  return client->sd;
}

int krad_ipc_client_send_fd (krad_ipc_client_t *client, int fd) {
  char buf[1];
  struct iovec iov;
  struct msghdr msg;
  struct cmsghdr *cmsg;
  int n;
  char cms[CMSG_SPACE(sizeof(int))];

  buf[0] = 0;
  iov.iov_base = buf;
  iov.iov_len = 1;

  memset(&msg, 0, sizeof msg);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = (caddr_t)cms;
  msg.msg_controllen = CMSG_LEN(sizeof(int));

  cmsg = CMSG_FIRSTHDR(&msg);
  cmsg->cmsg_len = CMSG_LEN(sizeof(int));
  cmsg->cmsg_level = SOL_SOCKET;
  cmsg->cmsg_type = SCM_RIGHTS;
  memmove(CMSG_DATA(cmsg), &fd, sizeof(int));

  if((n=sendmsg(client->sd, &msg, 0)) != iov.iov_len)
        return 0;
  return 1;
}
