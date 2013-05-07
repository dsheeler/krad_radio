#include "krad_stream.h"

static void kr_base64_encode (char *dest, char *src, int maxlen);
static void kr_stream_read_http_headers (krad_stream_t *stream);
static void kr_stream_send_request_to_stream (krad_stream_t *stream);
static void kr_stream_send_request_for_stream (krad_stream_t *stream);
static krad_stream_t *kr_stream_connect (char *host, int port);

static void kr_base64_encode (char *dest, char *src, int maxlen) {

  char b64t[64] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
                    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
                    '8', '9', '+', '/' };
  int len;
  int base64_len;
  int slice;
  char *out;
  char *result;
  char buffer[1024];

  len = strlen (src);
  base64_len = len * 4 / 3 + 4;
  out = buffer;
  result = out;
  
  if ((dest == NULL) || (base64_len > 1024) || (base64_len > maxlen)) {
    return;
  }

  while (len > 0) {
   slice = (len > 3) ? 3 : len;
    *out++ = b64t[(*src & 0xFC) >> 2];
    *out++ = b64t[((*src & 0x03) << 4) | ((*(src + 1) & 0xF0) >> 4)];
    switch (slice) {
      case 3:
        *out++ = b64t[((*(src + 1) & 0x0F) << 2) | ((*(src + 2) & 0xC0) >> 6)];
        *out++ = b64t[(*(src + 2)) & 0x3F];
        break;

      case 2:
        *out++ = b64t[((*(src + 1) & 0x0F) << 2)];
        *out++ = '=';
        break;

      case 1:
        *out++ = '=';
        *out++ = '=';
        break;
    }
    src += slice;
    len -= slice;
  }
  *out = 0;
  strncpy (dest, result, maxlen);
}

ssize_t kr_stream_send (krad_stream_t *stream, void *buffer, size_t len) {
  ssize_t ret;
  ret = send (stream->sd, buffer, len, 0);
  if (ret > 0) {
    stream->position += ret;
  } else {
    if ((ret == -1) && (errno != EAGAIN)) {
      stream->err_no = errno;
      strerror_r (stream->err_no, stream->error_str, sizeof(stream->error_str));
      stream->position = 0;
      stream->half_ready = 0;
      stream->ready = 0;
      stream->connected = 0;
    }
  }
  return ret;
}

ssize_t kr_stream_recv (krad_stream_t *stream, void *buffer, size_t len) {
  ssize_t ret;
  ret = recv (stream->sd, buffer, len, 0);
  if (ret > 0) {
    stream->position += ret;
  }
  return ret;
}

int kr_stream_disconnect (krad_stream_t *stream) {
  if (stream == NULL) {
    return -2;
  }
  if (stream->sd != 0) {
    close (stream->sd);
    stream->sd = 0;
    return 0;
  }
  return -1;
}

int kr_stream_free (krad_stream_t **stream) {

  if ((stream == NULL) || (*stream == NULL)) {
    return -1;
  }
  
  if ((*stream)->host != NULL) {
    free ((*stream)->host);
    (*stream)->host = NULL;
  }
  if ((*stream)->mount != NULL) {
    free ((*stream)->mount);
    (*stream)->mount = NULL;
  }
  if ((*stream)->password != NULL) {
    free ((*stream)->password);
    (*stream)->password = NULL;
  }

  free (*stream);
  *stream = NULL;
  return 0;
}

int kr_stream_destroy (krad_stream_t **stream) {
  if ((stream == NULL) || (*stream == NULL)) {
    return -1;
  }
  kr_stream_disconnect (*stream);
  return kr_stream_free (stream);
}

int kr_stream_reconnect (krad_stream_t *stream) {
  //kr_stream_disconnect (stream);
  //FIXME RECCONNECTING
  return -1;
}

static krad_stream_t *kr_stream_connect (char *host, int port) {

  krad_stream_t *stream;
  int ret;
  int flags;
  char port_string[6];
  struct in6_addr serveraddr;
  struct addrinfo hints;
  struct addrinfo *res;

  res = NULL;  

  if ((host == NULL) || ((port < 0) || (port > 65535))) {
    return NULL;
  }

  stream = calloc (1, sizeof(krad_stream_t));

  memset (&hints, 0, sizeof(hints));
  hints.ai_flags = AI_NUMERICSERV;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  ret = inet_pton (AF_INET, host, &serveraddr);
  if (ret == 1) {
    hints.ai_family = AF_INET;
    hints.ai_flags |= AI_NUMERICHOST;
  } else {
    ret = inet_pton (AF_INET6, host, &serveraddr);
    if (ret == 1) {
      hints.ai_family = AF_INET6;
      hints.ai_flags |= AI_NUMERICHOST;
    }
  }
  snprintf (port_string, 6, "%d", port);
  ret = getaddrinfo (host, port_string, &hints, &res);
  if (ret != 0) {
    kr_stream_destroy (&stream);
    return NULL;
  }

  stream->sd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
  if (stream->sd < 0) {
    kr_stream_destroy (&stream);
  } else {
    flags = fcntl (stream->sd, F_GETFL, 0);
    if (flags == -1) {
      failfast ("Krad System: error on syscall fcntl F_GETFL");
    } else {
      flags |= O_NONBLOCK;
      ret = fcntl (stream->sd, F_SETFL, flags);
      if (ret == -1) {
        failfast ("Krad System: error on syscall fcntl F_SETFL");
      } else {
        ret = connect (stream->sd, res->ai_addr, res->ai_addrlen);
        if ((ret < 0) && (errno != EINPROGRESS)) {
          kr_stream_destroy (&stream);
        }
      }
    }
  }

  if (res != NULL) {
    freeaddrinfo (res);
  }

  return stream;
}

static void kr_stream_read_http_headers (krad_stream_t *stream) {

  ssize_t ret;
  int32_t i;
  char buf[1024];

  while (1) {
    if (stream->drain > 0) {
      ret = recv (stream->sd, buf, stream->drain, 0);
      //printf ("drain %zd of %d\n", ret, stream->drain);
      if (ret < 1) {
        if ((ret == 0) || ((ret == -1) && (errno != EAGAIN))) {
          stream->position = 0;
          stream->half_ready = 0;
          stream->ready = 0;
          stream->connected = 0;
        }
      } else {
        stream->drain -= ret;
        if (stream->hle == 2) {
          stream->position = 0;
          stream->half_ready = 0;
          stream->ready = 1;
          stream->connected = 1;
          return;
        }
      }
    } else {
      ret = recv (stream->sd, buf, sizeof(buf), MSG_PEEK);
      if (ret < 1) {
        if ((ret == 0) || ((ret == -1) && (errno != EAGAIN))) {
          stream->position = 0;
          stream->half_ready = 0;
          stream->ready = 0;
          stream->connected = 0;
        }
        return;
      } else {
        for (i = 0; i < ret; i++) {
          if ((buf[i] == '\n') || (buf[i] == '\r')) {
            if (stream->hle_pos != ((stream->position + i) - 1)) {
              stream->hle = 0;
              stream->hler = 0;
            }
            stream->hle_pos = stream->position + i;
            if (buf[i] == '\n') {
              stream->hle += 1;
            }
            if (buf[i] == '\r') {
              stream->hler += 1;
            }
            if (stream->hle == 2) {
              //printf ("%.*s--hle %d hler %d-\n",
              //        (i + 1) - (stream->hler + stream->hle),
              //        buf, stream->hle, stream->hler);
              stream->drain = i + 1;
              break;
            }
          }
        }
        if (stream->drain == 0) {
          stream->drain = ret;
          //printf ("drain to %d\n", stream->drain);
        }
      }
    }
  }
}

static void kr_stream_send_request_to_stream (krad_stream_t *stream) {

  ssize_t ret;
  int len;
  char http_req[512];
  char auth[256];
  char auth_base64[256];

  len = 0;

  snprintf (auth, sizeof(auth), "source:%s", stream->password);
  kr_base64_encode (auth_base64, auth, sizeof(auth_base64));

  len = snprintf (http_req, sizeof(http_req) - len,
                  "SOURCE %s ICE/1.0\r\n",
                  stream->mount);

  len += snprintf (http_req + len, sizeof(http_req) - len,
                   "content-type: %s\r\n",
                   stream->content_type);

  len += snprintf (http_req + len, sizeof(http_req) - len,
                   "Authorization: Basic %s\r\n\r\n",
                   auth_base64);

  ret = kr_stream_send (stream,
                        http_req + stream->position,
                        len - stream->position);
  if (ret > 0) {
    //printf ("RTR!! %.*s--\n", stream->position, http_req);
    if (stream->position == len) {
      stream->position = 0;
      stream->ready = 1;
      stream->connected = 1;      
    }
  }
}

static void kr_stream_send_request_for_stream (krad_stream_t *stream) {

  ssize_t ret;
  int len;
  char http_req[512];

  len = snprintf (http_req, sizeof(http_req),
                  "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n",
                  stream->mount, stream->host);

  ret = kr_stream_send (stream,
                        http_req + stream->position,
                        len - stream->position);
  if (ret > 0) {
    if (stream->position == len) {
      stream->position = 0;
      stream->half_ready = 1;
    }
  }
}

krad_stream_t *kr_stream_create (char *host, int port,
                                 char *mount, char *content_type,
                                 char *password) {
  krad_stream_t *stream;

  if ((host == NULL) || (mount == NULL) || (password == NULL) ||
      (content_type == NULL) || ((port < 0) || (port > 65535))) {
    return NULL;
  }

  if (mount[0] != '/') {
    return NULL;
  }

  stream = kr_stream_connect (host, port);
  if (stream == NULL) {
    return NULL;
  }

  stream->host = strdup (host);
  stream->port = port;
  stream->mount = strdup (mount);
  stream->password = strdup (password);
  stream->content_type = strdup (content_type);

  stream->direction = 1;

  return stream;
}

krad_stream_t *kr_stream_open (char *host, int port, char *mount) {

  krad_stream_t *stream;

  if ((host == NULL) || (mount == NULL) || ((port < 0) || (port > 65535))) {
    return NULL;
  }

  if (mount[0] != '/') {
    return NULL;
  }

  stream = kr_stream_connect (host, port);
  if (stream == NULL) {
    return NULL;
  }

  stream->host = strdup (host);
  stream->port = port;
  stream->mount = strdup (mount);

  stream->direction = 0;

  return stream;
}

int32_t kr_stream_handle_headers (krad_stream_t *stream) {
  if (stream->ready == 0) {
    if (stream->direction == 1) {
      kr_stream_send_request_to_stream (stream);
    } else {
      if (stream->half_ready == 0) {
        kr_stream_send_request_for_stream (stream);
      }
      if (stream->half_ready == 1) {
        kr_stream_read_http_headers (stream);
      }
    }
  }
  return stream->ready;
}

void kr_stream_i_am_a_blocking_subscripter (krad_stream_t *stream) {

  int ret;
  struct pollfd sp[1];
  
  if (stream->direction == 1) {
    sp[0].events = POLLOUT;
  } else {
    sp[0].events = POLLIN;
  }
  sp[0].fd = stream->sd;
  
  ret = poll (sp, 1, -1);

  if (sp[0].revents & POLLERR) {
    fprintf (stderr, "Got poll err on %s\n", stream->mount);
  }
  if (sp[0].revents & POLLHUP) {
    fprintf (stderr, "Got poll POLLHUP on %s\n", stream->mount);
  }
  
  if (stream->direction == 1) {
    if (!(sp[0].revents & POLLOUT)) {
      fprintf (stderr, "Did NOT get POLLOUT on %s\n", stream->mount);
    }
  } else {
    if (!(sp[0].revents & POLLIN)) {
      fprintf (stderr, "Did NOT get POLLIN on %s\n", stream->mount);
    }
  }  

  if (ret != 1) {
    fprintf (stderr, "poll failure\n");
    exit (1);
  }
}
