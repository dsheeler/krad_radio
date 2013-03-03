#include "krad_io.h"

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

int krad_io_write (krad_io_t *krad_io, void *buffer, size_t length) {

//  if ((length + krad_io->write_buffer_pos) >= KRADio_WRITE_BUFFER_SIZE) {
//    krad_io_write_sync(krad_io);
//  }

  memcpy(krad_io->write_buffer + krad_io->write_buffer_pos, buffer, length);
  krad_io->write_buffer_pos += length;

  return length;
}

int krad_io_write_sync (krad_io_t *krad_io) {

  uint64_t length;
  
  length = krad_io->write_buffer_pos;
  krad_io->write_buffer_pos = 0;

  return krad_io->write(krad_io, krad_io->write_buffer, length);
}

int krad_io_read (krad_io_t *krad_io, void *buffer, size_t length) {
  return krad_io->read(krad_io, buffer, length);
}

int krad_io_seek (krad_io_t *krad_io, int64_t offset, int whence) {

  if (krad_io->mode == KRAD_IO_WRITEONLY) {
    if (whence == SEEK_CUR) {
      krad_io->write_buffer_pos += offset;
    }
    if (whence == SEEK_SET) {
      krad_io->write_buffer_pos = offset;
    }
    return krad_io->write_buffer_pos;
  }

  return krad_io->seek(krad_io, offset, whence);
}

int64_t krad_io_tell (krad_io_t *krad_io) {

  if (krad_io->mode == KRAD_IO_WRITEONLY) {
    return krad_io->write_buffer_pos;
  }

  return krad_io->tell(krad_io);
}

int krad_io_file_open (krad_io_t *krad_io) {

  int fd;
  
  fd = 0;
  
  if (krad_io->mode == KRAD_IO_READONLY) {
    fd = open ( krad_io->uri, O_RDONLY );
  }
  
  if (krad_io->mode == KRAD_IO_WRITEONLY) {
    fd = open ( krad_io->uri, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
  }
  
  //printf("fd is %d\n", fd);
  
  krad_io->ptr = fd;

  return fd;
}

int krad_io_file_close (krad_io_t *krad_io) {
  return close(krad_io->ptr);
}

int krad_io_file_write (krad_io_t *krad_io, void *buffer, size_t length) {
  return write(krad_io->ptr, buffer, length);
}

int krad_io_file_read (krad_io_t *krad_io, void *buffer, size_t length) {

  //printf("len is %zu\n", length);

  return read(krad_io->ptr, buffer, length);
}

int64_t krad_io_file_seek (krad_io_t *krad_io, int64_t offset, int whence) {
  return lseek(krad_io->ptr, offset, whence);
}

int64_t krad_io_file_tell (krad_io_t *krad_io) {
  return lseek(krad_io->ptr, 0, SEEK_CUR);
}

int krad_io_stream_close (krad_io_t *krad_io) {
  return close(krad_io->sd);
}

static int krad_io_stream_write (krad_io_t *krad_io, void *buffer, size_t length) {

  int bytes;
  int ret;
  
  ret = 0;
  bytes = 0;

  while (bytes != length) {
    ret += send (krad_io->sd, buffer + bytes, length - bytes, 0);
    if (ret <= 0) {
      break;
    } else {
      bytes += ret;
    }
  }
  return bytes;
}

int krad_io_stream_read (krad_io_t *krad_io, void *buffer, size_t length) {

  int bytes;
  int total_bytes;

  total_bytes = 0;
  bytes = 0;

  while (total_bytes != length) {

    bytes = recv (krad_io->sd, buffer + total_bytes, length - total_bytes, 0);

    if (bytes <= 0) {
      if (bytes == 0) {
        printkd ("Krad IO Stream Recv: Got EOF\n");
        return total_bytes;
      }
      if (bytes < 0) {
        printkd ("Krad IO Stream Recv: Got Disconnected\n");
        return total_bytes;
      }
    }
    total_bytes += bytes;
  }

  return total_bytes;
}

int krad_io_stream_open (krad_io_t *krad_io) {

  int ret;
  char http_string[512];
  int http_string_pos;
  char *content_type;
  char auth[256];
  char auth_base64[256];
  char port_string[6];
  struct in6_addr serveraddr;
  struct addrinfo hints;
  struct addrinfo *res;

  http_string_pos = 0;

  printkd ("Krad io: Connecting to %s:%d", krad_io->host, krad_io->port);

  memset (&hints, 0x00, sizeof (hints));
  hints.ai_flags = AI_NUMERICSERV;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  ret = inet_pton (AF_INET, krad_io->host, &serveraddr);
  if (ret == 1) {
    hints.ai_family = AF_INET;
    hints.ai_flags |= AI_NUMERICHOST;
  } else {
    ret = inet_pton (AF_INET6, krad_io->host, &serveraddr);
    if (ret == 1) {
      hints.ai_family = AF_INET6;
      hints.ai_flags |= AI_NUMERICHOST;
    }
  }
  snprintf (port_string, 6, "%d", krad_io->port);
  ret = getaddrinfo (krad_io->host, port_string, &hints, &res);
  if (ret != 0) {
    return -1;
  }

  krad_io->sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (krad_io->sd > 0) {
    if (connect (krad_io->sd, res->ai_addr, res->ai_addrlen) < 0) {
      printkd ("Krad io Source: Connect Error");
    } else {
      if (krad_io->mode == KRAD_IO_READONLY) {
        sprintf (http_string, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", krad_io->mount, krad_io->host);
        snprintf (http_string, sizeof (http_string),
                  "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n",
                  krad_io->mount, krad_io->host);
        printkd ("%s\n", http_string);
        krad_io_stream_write(krad_io, http_string, strlen(http_string));
   
        int end_http_headers = 0;
        char buf[8];
        while (end_http_headers != 4) {
    
          krad_io_stream_read(krad_io, buf, 1);
    
          printkd ("%c", buf[0]);
    
          if ((buf[0] == '\n') || (buf[0] == '\r')) {
            end_http_headers++;
          } else {
            end_http_headers = 0;
          }
    
        }
      } 
      
      if (krad_io->mode == KRAD_IO_WRITEONLY) {
        if ((strstr(krad_io->mount, ".opus")) ||
            (strstr(krad_io->mount, ".Opus")) ||
            (strstr(krad_io->mount, ".OPUS"))) {
          content_type = "audio/opus";
         } else {
          if ((strstr(krad_io->mount, ".ogg")) ||
            (strstr(krad_io->mount, ".OGG")) ||
            (strstr(krad_io->mount, ".Ogg")) ||
            (strstr(krad_io->mount, ".oga")) ||
            (strstr(krad_io->mount, ".ogv")) ||
            (strstr(krad_io->mount, ".Oga")) ||    
            (strstr(krad_io->mount, ".OGV"))) {
              content_type = "application/ogg";
          } else {
            content_type = "video/webm";
          }
        }
        snprintf (auth, sizeof (auth), "source:%s", krad_io->password );
        krad_base64_encode ( auth_base64, auth, sizeof (auth_base64));
        http_string_pos = snprintf ( http_string,
                                     sizeof (http_string) - http_string_pos, 
                                     "SOURCE %s ICE/1.0\r\n", krad_io->mount);
        http_string_pos += snprintf ( http_string + http_string_pos,
                                      sizeof (http_string) - http_string_pos,
                                      "content-type: %s\r\n", content_type);
        http_string_pos += snprintf ( http_string + http_string_pos,
                                      sizeof (http_string) - http_string_pos,
                                      "Authorization: Basic %s\r\n", auth_base64);
        http_string_pos += snprintf ( http_string + http_string_pos,
                                      sizeof (http_string) - http_string_pos, "\r\n");
        krad_io_write (krad_io, http_string, http_string_pos);
      }
    }
  }
  
  if (res != NULL) {
    freeaddrinfo (res);
    res = NULL;
  }
  
  return krad_io->sd;
}

void krad_io_destroy (krad_io_t *krad_io) {

  if (krad_io->mode == KRAD_IO_WRITEONLY) {
    krad_io_write_sync (krad_io);
  }

  if (krad_io->mode != -1) {
    krad_io->close(krad_io);
  }
    
  if (krad_io->write_buffer != NULL) {
    free (krad_io->write_buffer);
  }
  
  free (krad_io);
}

krad_io_t *krad_io_create () {
  krad_io_t *krad_io = calloc(1, sizeof(krad_io_t));
  return krad_io;
}

krad_io_t *krad_io_open_stream (char *host, int port, char *mount, char *password) {

  krad_io_t *krad_io;
  
  krad_io = krad_io_create();

  if (password == NULL) {
    krad_io->mode = KRAD_IO_READONLY;
  } else {
    krad_io->mode = KRAD_IO_WRITEONLY;
    krad_io->write_buffer = malloc(KRAD_IO_WRITE_BUFFER_SIZE);
  }
  //krad_io->seekable = 1;
  krad_io->read = krad_io_stream_read;
  krad_io->write = krad_io_stream_write;
  krad_io->open = krad_io_stream_open;
  krad_io->close = krad_io_stream_close;
  krad_io->uri = host;
  krad_io->host = host;
  krad_io->port = port;
  krad_io->mount = mount;
  krad_io->password = password;
  
  //krad_io->stream = 1;
  
  if (strcmp("ListenSD", krad_io->host) == 0) {
    krad_io->sd = krad_io->port;
  } else {
    krad_io->open (krad_io);
  }
    
  return krad_io;
}

krad_io_t *krad_io_open_file (char *filename, krad_io_mode_t mode) {

  krad_io_t *krad_io;
  
  krad_io = krad_io_create();

  krad_io->seek = krad_io_file_seek;
  krad_io->tell = krad_io_file_tell;
  krad_io->mode = mode;
  krad_io->seekable = 1;
  krad_io->read = krad_io_file_read;
  krad_io->write = krad_io_file_write;
  krad_io->open = krad_io_file_open;
  krad_io->close = krad_io_file_close;
  krad_io->uri = filename;
  krad_io->open(krad_io);

  if (krad_io->mode == KRAD_IO_WRITEONLY) {
    krad_io->write_buffer = malloc(KRAD_IO_WRITE_BUFFER_SIZE);
  }
  
  return krad_io;
} 
