#include "krad_stream.h"

static void kr_base64_encode (char *dest, char *src, int maxlen);
static int kr_stream_read (krad_stream_t *stream, void *buffer, size_t len);

static void kr_base64_encode (char *dest, char *src, int maxlen) {

  char b64t[64] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
                    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
                    '8', '9', '+', '/' };
  int len;
  int base64_len;
  int chunk;
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
   chunk = (len > 3) ? 3 : len;
    *out++ = b64t[(*src & 0xFC) >> 2];
    *out++ = b64t[((*src & 0x03) << 4) | ((*(src + 1) & 0xF0) >> 4)];
    switch(chunk) {
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
    src += chunk;
    len -= chunk;
  }
  *out = 0;
  strncpy (dest, result, maxlen);
}

int kr_stream_write (krad_stream_t *stream, void *buffer, size_t len) {

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

static int kr_stream_read (krad_stream_t *stream, void *buffer, size_t len) {
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

krad_stream_t *kr_stream_create (char *host, int port,
                                 char *mount, char *password) {

  krad_stream_t *krad_stream;
  int http_string_pos;
  char *content_type;
  char auth[256];
  char auth_base64[256];
  char http_string[512];
  int ret;
  char port_string[6];
  struct in6_addr serveraddr;
  struct addrinfo hints;
  struct addrinfo *res;

  krad_stream = NULL;
  res = NULL;  
  http_string_pos = 0;

  krad_stream = calloc (1, sizeof(krad_stream_t));

  if ((host == NULL) || (mount == NULL) || ((port < 0) || (port > 65535))) {
    kr_stream_destroy (&krad_stream);
    return NULL;
  }
  
  if (mount[0] == '/') {
    if (strlen(mount) > 1) {
      mount = mount + 1;
    } else {
      kr_stream_destroy (&krad_stream);
      return NULL;
    }
  }

  memset (&hints, 0x00, sizeof (hints));
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
    kr_stream_destroy (&krad_stream);
    return NULL;
  }

  krad_stream->sd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
  if (krad_stream->sd < 0) {
    kr_stream_destroy (&krad_stream);
  } else {
    if (connect (krad_stream->sd, res->ai_addr, res->ai_addrlen) < 0) {
      kr_stream_destroy (&krad_stream);
    } else {
      if (password == NULL) {
        snprintf (http_string, sizeof (http_string),
                  "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", mount, host);
        kr_stream_write (krad_stream, http_string, strlen (http_string));
        int end_http_headers = 0;
        char buf[8];
        while (end_http_headers != 4) {
          kr_stream_read (krad_stream, buf, 1);
          if ((buf[0] == '\n') || (buf[0] == '\r')) {
            end_http_headers++;
          } else {
            end_http_headers = 0;
          }
        }
      } else {
        if ((strstr(mount, ".opus")) ||
            (strstr(mount, ".Opus")) ||
            (strstr(mount, ".OPUS"))) {
          content_type = "audio/opus";
         } else {
          if ((strstr(mount, ".ogg")) ||
            (strstr(mount, ".OGG")) ||
            (strstr(mount, ".Ogg")) ||
            (strstr(mount, ".oga")) ||
            (strstr(mount, ".ogv")) ||
            (strstr(mount, ".Oga")) ||    
            (strstr(mount, ".OGV"))) {
              content_type = "application/ogg";
          } else {
            content_type = "video/webm";
          }
        }
        snprintf (auth, sizeof (auth), "source:%s", password );
        kr_base64_encode ( auth_base64, auth, sizeof (auth_base64));
        http_string_pos = snprintf ( http_string,
                                     sizeof (http_string) - http_string_pos, 
                                     "SOURCE /%s ICE/1.0\r\n", mount);
        http_string_pos += snprintf ( http_string + http_string_pos,
                                      sizeof (http_string) - http_string_pos,
                                      "content-type: %s\r\n", content_type);
        http_string_pos += snprintf ( http_string + http_string_pos,
                                      sizeof (http_string) - http_string_pos,
                                      "Authorization: Basic %s\r\n\r\n",
                                      auth_base64);
        kr_stream_write (krad_stream, http_string, http_string_pos);
      }
    }
  }
  
  if (res != NULL) {
    freeaddrinfo (res);
    res = NULL;
  }

  if (krad_stream != NULL) {
    krad_stream->host = strdup (host);
    krad_stream->port = port;
    krad_stream->mount = strdup (mount);
    krad_stream->password = strdup (password);
  }

  return krad_stream;
}

krad_stream_t *kr_stream_open (char *host, int port, char *mount) {
  //FIXME
  return NULL;
}

