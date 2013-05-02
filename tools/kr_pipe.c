#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "krad_file.h"
#include "krad_stream.h"

/* Stream to stream */
void krad_pipe (char *host, int port, char *mount,
                char *host_out, int port_out, char *mount_out, char *password);

/* stdin to stream */
void krad_pipe_to (char *host, int port, char *mount, char *password);

void krad_pipe (char *host, int port, char *mount,
                char *host_out, int port_out, char *mount_out, char *password) {

  krad_stream_t *stream_in;
  krad_stream_t *stream_out;
  int bytes;
  uint64_t total_bytes;
  uint8_t buffer[4096];

  bytes = 0;

  /* Temp */
  char *content_type = "video/webm";

  stream_in = kr_stream_open (host, port, mount);

  if (stream_in == NULL) {
    fprintf (stderr, "Connection to %s:%d%s failed.\n", host, port, mount);
    return;
  }

  stream_out = kr_stream_create (host_out, port_out, mount_out,
                                 content_type, password);
  if (stream_out == NULL) {
    fprintf (stderr, "Connection to %s:%d%s failed.\n", host_out, port_out, mount_out);
    kr_stream_destroy (&stream_in);
    return;
  }

  while (1) {
    bytes = kr_stream_recv (stream_in, buffer, sizeof(buffer));
    if (bytes < 1) {
      break;
    }
    if (kr_stream_send (stream_out, buffer, bytes) != bytes) {
      break;
    }
    total_bytes += bytes;
    if (1) {
      printf ("Krad Piped: %"PRIu64"K\r", total_bytes / 1000);
      fflush (stdout);
    }
  }

  kr_stream_destroy (&stream_in);
  kr_stream_destroy (&stream_out);
}

void krad_pipe_to (char *host, int port, char *mount, char *password) {

  kr_file_t *stream_in;
  krad_stream_t *stream_out;
  int bytes;
  uint64_t total_bytes;
  uint8_t buffer[1024];

  bytes = 0;

  /* Temp */
  char *content_type = "video/webm";

  stream_in = kr_file_open_stdin ();

  if (stream_in == NULL) {
    fprintf (stderr, "Opening stdin fail, sad story.\n");
    return;
  }

  stream_out = kr_stream_create (host, port, mount, content_type, password);
  if (stream_out == NULL) {
    fprintf (stderr, "Connection to %s:%d%s failed.\n", host, port, mount);
    kr_file_close (&stream_in);
    return;
  }

  while (1) {
    bytes = kr_file_read (stream_in, buffer, sizeof(buffer));
    if (bytes < 1) {
      break;
    }
    if (kr_stream_send (stream_out, buffer, bytes) != bytes) {
      break;
    }
    total_bytes += bytes;
    if (1) {
      printf ("Krad Piped: %"PRIu64"K\r", total_bytes / 1000);
      fflush (stdout);
    }
  }

  kr_file_close (&stream_in);
  kr_stream_destroy (&stream_out);
}

int main (int argc, char *argv[]) {

  if ((argc != 5) && (argc != 8)) {
    fprintf (stderr, "Krad Pipe\n");
    fprintf (stderr, "\nWrong number of args, got %d, need 4 or 7\n", argc - 1);
    return 1;
  }

  if (argc == 5) {
    krad_pipe_to (argv[1], atoi(argv[2]), argv[3], argv[4]);
  }

  if (argc == 8) {
    krad_pipe (argv[1], atoi(argv[2]), argv[3],
               argv[4], atoi(argv[5]), argv[6], argv[7]);
  }

  return 1;
}
