#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>

/*
* Krad Pipe
*
* To Compile: gcc -Wall -o krad_pipe krad_pipe.c
*/

#define BUF_SIZE 4096
#define DEBUG 1
#define USE "krad_pipe [from_host] [from_port] [from_mount] [to_host] [to_port] [to_mount] [to_password]"
#define VERSION 3

typedef struct krad_stream_St krad_stream_t;

struct krad_stream_St {
  int sd;
};

void krad_pipe (char *host, int port, char *mount,
                char *host_out, int port_out, char *mount_out, char *password);

static void krad_base64_encode (char *dest, char *src, int maxlen);
static int krad_stream_write (krad_stream_t *krad_stream, void *buffer, size_t length);
static int krad_stream_read (krad_stream_t *krad_stream, void *buffer, size_t length);
static void krad_stream_destroy (krad_stream_t *krad_stream);
static krad_stream_t *krad_stream_open (char *host, int port, char *mount, char *password);

static void krad_base64_encode (char *dest, char *src, int maxlen) {

  static char base64table[64] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
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
    *out++ = base64table[(*src & 0xFC) >> 2];
    *out++ = base64table[((*src & 0x03) << 4) | ((*(src + 1) & 0xF0) >> 4)];
    switch(chunk) {
      case 3:
        *out++ = base64table[((*(src + 1) & 0x0F) << 2) | ((*(src + 2) & 0xC0) >> 6)];
        *out++ = base64table[(*(src + 2)) & 0x3F];
        break;

      case 2:
        *out++ = base64table[((*(src + 1) & 0x0F) << 2)];
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

static int krad_stream_write (krad_stream_t *krad_stream, void *buffer, size_t length) {

  int bytes;
  int ret;
  
  ret = 0;
  bytes = 0;

  while (bytes != length) {
    ret += send (krad_stream->sd, buffer + bytes, length - bytes, 0);
    if (ret <= 0) {
      break;
    } else {
      bytes += ret;
    }
  }
  return bytes;
}

static int krad_stream_read (krad_stream_t *krad_stream, void *buffer, size_t length) {
  return recv (krad_stream->sd, buffer, length, 0);
}

static void krad_stream_destroy (krad_stream_t *krad_stream) {
  if (krad_stream->sd != 0) {
    close (krad_stream->sd);
    krad_stream->sd = 0;
  }
  free (krad_stream);
}

static krad_stream_t *krad_stream_open (char *host, int port, char *mount, char *password) {

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
  
  if (krad_stream == NULL) {
    return NULL;
  }

  if ((host == NULL) || (mount == NULL) || ((port < 0) || (port > 65535))) {
    krad_stream_destroy (krad_stream);
    return NULL;
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
    krad_stream_destroy (krad_stream);
    return NULL;
  }

  krad_stream->sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (krad_stream->sd < 0) {
    krad_stream_destroy (krad_stream);
    krad_stream = NULL;
  } else {
    if (connect (krad_stream->sd, res->ai_addr, res->ai_addrlen) < 0) {
      krad_stream_destroy (krad_stream);
      krad_stream = NULL;
    } else {
      if (password == NULL) {
        snprintf (http_string, sizeof (http_string),
                  "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", mount, host);
        krad_stream_write (krad_stream, http_string, strlen(http_string));
        int end_http_headers = 0;
        char buf[8];
        while (end_http_headers != 4) {
          krad_stream_read (krad_stream, buf, 1);
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
        krad_base64_encode ( auth_base64, auth, sizeof (auth_base64));
        http_string_pos = snprintf ( http_string,
                                     sizeof (http_string) - http_string_pos, 
                                     "SOURCE %s ICE/1.0\r\n", mount);
        http_string_pos += snprintf ( http_string + http_string_pos,
                                      sizeof (http_string) - http_string_pos,
                                      "content-type: %s\r\n", content_type);
        http_string_pos += snprintf ( http_string + http_string_pos,
                                      sizeof (http_string) - http_string_pos,
                                      "Authorization: Basic %s\r\n", auth_base64);
        http_string_pos += snprintf ( http_string + http_string_pos,
                                      sizeof (http_string) - http_string_pos, "\r\n");
        krad_stream_write (krad_stream, http_string, http_string_pos);
      }
    }
  }
  
  if (res != NULL) {
    freeaddrinfo (res);
    res = NULL;
  }

  return krad_stream;
}

void krad_pipe (char *host, int port, char *mount,
                char *host_out, int port_out, char *mount_out, char *password) {

  krad_stream_t *stream_in;
  krad_stream_t *stream_out;
  int verbose;
  int bytes;  
  uint64_t total_bytes;
  unsigned char buffer[BUF_SIZE];

  verbose = DEBUG;
  bytes = 0;

  stream_in = krad_stream_open (host, port, mount, NULL);

  if (stream_in == NULL) {
    fprintf (stderr, "Connection to %s:%d%s failed.\n", host, port, mount);
    return;
  } else {
    stream_out = krad_stream_open (host_out, port_out, mount_out, password);
    if (stream_out == NULL) {
      fprintf (stderr, "Connection to %s:%d%s failed.\n", host_out, port_out, mount_out);
      krad_stream_destroy (stream_in);
      return;
    }
  }

  while (1) {
    bytes = krad_stream_read (stream_in, buffer, BUF_SIZE);
    if (bytes < 1) {
      break;
    }
    if (krad_stream_write (stream_out, buffer, bytes) != bytes) {
      break;
    }
    total_bytes += bytes;
    if (verbose) {
      printf ("Krad Piped: %"PRIu64"K\r", total_bytes / 1000);
      fflush (stdout);
    }
  }

  krad_stream_destroy (stream_in);
  krad_stream_destroy (stream_out);
}

int main (int argc, char *argv[]) {

  if (argc != 8) {
    fprintf (stderr, "Krad Pipe Version %d\n", VERSION);  
    fprintf (stderr, USE);
    fprintf (stderr, "\nWrong number of args, got %d, need 7\n", argc - 1);
    return 1;
  }

  krad_pipe (argv[1], atoi(argv[2]), argv[3],
             argv[4], atoi(argv[5]), argv[6], argv[7]);
  return 1;
}
