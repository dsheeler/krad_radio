#include "krad_stream.h"

static void kr_base64_encode (char *dest, char *src, int maxlen);
static void kr_stream_read_http_headers (krad_stream_t **stream);
static void kr_stream_send_request_to_stream (krad_stream_t **stream);
static void kr_stream_send_request_for_stream (krad_stream_t **stream);
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

int kr_stream_send (krad_stream_t *stream, void *buffer, size_t len) {

  int bytes;
  int ret;
  
  ret = 0;
  bytes = 0;

  while (bytes != len) {
    ret += send (stream->sd, buffer + bytes, len - bytes, 0);
    if (ret <= 0) {
      break;
    } else {
      bytes += ret;
    }
  }
  return bytes;
}

int kr_stream_recv (krad_stream_t *stream, void *buffer, size_t len) {
  return recv (stream->sd, buffer, len, 0);
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
    if (connect (stream->sd, res->ai_addr, res->ai_addrlen) < 0) {
      kr_stream_destroy (&stream);
    }
  }

  if (res != NULL) {
    freeaddrinfo (res);
  }

  return stream;
}

static void kr_stream_read_http_headers (krad_stream_t **stream) {

  int ret;
  int end;
  char buf[8];

  end = 0;

  while (end != 4) {
    ret = kr_stream_recv (*stream, buf, 1);
    if (ret != 1) {
      kr_stream_destroy (stream);
      return;
    }    
    if ((buf[0] == '\n') || (buf[0] == '\r')) {
      end++;
    } else {
      end = 0;
    }
  }
}

static void kr_stream_send_request_to_stream (krad_stream_t **stream) {

  int ret;
  int len;
  char http_req[512];
  char auth[256];
  char auth_base64[256];

  len = 0;

  snprintf (auth, sizeof (auth), "source:%s", (*stream)->password);
  kr_base64_encode (auth_base64, auth, sizeof(auth_base64));

  len = snprintf (http_req, sizeof(http_req) - len,
                  "SOURCE /%s ICE/1.0\r\n",
                  (*stream)->mount);

  len += snprintf (http_req + len, sizeof(http_req) - len,
                   "content-type: %s\r\n",
                   (*stream)->content_type);

  len += snprintf (http_req + len, sizeof(http_req) - len,
                   "Authorization: Basic %s\r\n\r\n",
                   auth_base64);

  ret = kr_stream_send (*stream, http_req, len);
  if (ret != len) {
    kr_stream_destroy (stream);
  }
}

static void kr_stream_send_request_for_stream (krad_stream_t **stream) {

  int ret;
  int len;
  char http_req[512];

  len = snprintf (http_req, sizeof (http_req),
                  "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n",
                  (*stream)->mount, (*stream)->host);

  ret = kr_stream_send (*stream, http_req, len);
  if (ret != len) {
    kr_stream_destroy (stream);
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

  if (mount[0] == '/') {
    if (strlen(mount) > 1) {
      mount = mount + 1;
    } else {
      return NULL;
    }
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

  /* destroys if write fail */
  kr_stream_send_request_to_stream (&stream);

  return stream;
}

krad_stream_t *kr_stream_open (char *host, int port, char *mount) {

  krad_stream_t *stream;

  if ((host == NULL) || (mount == NULL) || ((port < 0) || (port > 65535))) {
    return NULL;
  }

  if (mount[0] == '/') {
    if (strlen(mount) > 1) {
      mount = mount + 1;
    } else {
      return NULL;
    }
  }

  stream = kr_stream_connect (host, port);
  if (stream == NULL) {
    return NULL;
  }

  stream->host = strdup (host);
  stream->port = port;
  stream->mount = strdup (mount);

  /* destroys if write fail */
  kr_stream_send_request_for_stream (&stream);

  /* destroys if read fail */
  kr_stream_read_http_headers (&stream);

  return stream;
}
