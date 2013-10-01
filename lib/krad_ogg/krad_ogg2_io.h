#include "krad_io2.h"
#include "krad_file.h"
#include "krad_stream.h"

typedef struct kr_ogg_io kr_ogg_io;

struct kr_ogg_io {
  int wrote_header;
  kr_ogg *ogg;
  int fd;
  kr_io2_t *io;
  kr_file *file;
  kr_stream *stream;
};

int kr_ogg_io_eos_track(kr_ogg_io *ogg_io, int track);
int kr_ogg_io_push_header(kr_ogg_io *ogg_io);
int kr_ogg_io_push(kr_ogg_io *ogg_io, int track, int64_t gpos,
 uint8_t *buffer, size_t sz);
int kr_ogg_io_destroy(kr_ogg_io **ogg_io);
kr_ogg_io *kr_ogg_io_create_file(char *filename);
kr_ogg_io *kr_ogg_io_create_stream(char *host, int port, char *mount,
 char *content_type, char *password);
