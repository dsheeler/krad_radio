#include "krad_io2.h"
#include "krad_file.h"
#include "krad_stream.h"

typedef struct kr_ogg_io_St kr_ogg_io_t;

struct kr_ogg_io_St {
  int wrote_header;
  kr_ogg_t *ogg;
  int fd;
  kr_io2_t *io;
  kr_file_t *file;
  krad_stream_t *stream;
};

int kr_ogg_io_eos_track (kr_ogg_io_t *ogg_io, int track);
int kr_ogg_io_push_header (kr_ogg_io_t *ogg_io);
int kr_ogg_io_push (kr_ogg_io_t *ogg_io, int track, int64_t gpos,
 uint8_t *buffer, size_t sz);
int kr_ogg_io_destroy (kr_ogg_io_t **ogg_io);
kr_ogg_io_t *kr_ogg_io_create_file (char *filename);
kr_ogg_io_t *kr_ogg_io_create_stream (char *host, int port, char *mount,
 char *content_type, char *password);
