int kr_ogg_io_destroy(kr_ogg_io **ogg_io) {
  if ((ogg_io != NULL) && (*ogg_io != NULL)) {
    if ((*ogg_io)->wrote_header == 1) {
      kr_ogg_io_eos_track((*ogg_io), 0);
    }
    kr_io2_destroy (&(*ogg_io)->io);
    if ((*ogg_io)->file != NULL) {
      kr_file_close (&(*ogg_io)->file);
    }
    if ((*ogg_io)->stream != NULL) {
      kr_stream_destroy (&(*ogg_io)->stream);
    }
/*
    if ((*mkv)->transmission != NULL) {
      kr_transmission_destroy ((*mkv)->transmission);
      (*mkv)->transmission = NULL;
    }
*/
    kr_ogg_destroy (&(*ogg_io)->ogg);
    free (*ogg_io);
    *ogg_io = NULL;
    return 0;
  }
  return -1;
}

kr_ogg_io *kr_ogg_io_create_file(char *filename) {

  kr_ogg_io *ogg_io;
  kr_file *file;

  file = kr_file_create (filename);

  if (file == NULL) {
    return NULL;
  }

  ogg_io = calloc(1, sizeof(kr_ogg_io));

  ogg_io->ogg = kr_ogg_create ();
  ogg_io->io = kr_io2_create ();
  ogg_io->file = file;
  ogg_io->fd = file->fd;
  kr_io2_set_fd (ogg_io->io, ogg_io->fd);

  return ogg_io;
}

kr_ogg_io *kr_ogg_io_create_stream (char *host, int port, char *mount,
 char *content_type, char *password) {

  kr_ogg_io *ogg_io;
  kr_stream *stream;

  stream = kr_stream_create (host, port, mount, content_type, password);

  if (stream == NULL) {
    return NULL;
  }

  ogg_io = calloc(1, sizeof(kr_ogg_io));

  while (stream->ready != 1) {
    kr_stream_handle_headers(stream);
  }

  ogg_io->ogg = kr_ogg_create ();
  ogg_io->io = kr_io2_create ();
  ogg_io->stream = stream;
  ogg_io->fd = stream->sd;
  kr_io2_set_fd (ogg_io->io, ogg_io->fd);

  return ogg_io;
}

static int kr_ogg_io_push_internal (kr_ogg_io *ogg_io) {

  ssize_t ret;
  ssize_t sent;
  ssize_t bytes;
  uint8_t *buffer;

  if (ogg_io->stream != NULL) {

    sent = 0;
    buffer = ogg_io->io->buffer;
    bytes = ogg_io->io->len;

    while (sent != bytes) {
      kr_stream_i_am_a_blocking_subscripter (ogg_io->stream);
      ret = kr_stream_send (ogg_io->stream, buffer + sent, bytes - sent);
      if (ret > 0) {
        sent += ret;
      }
      if (sent != bytes) {
        //printf ("\nSent to few bytes: ret %zd - sent %zd - total %zd\n",
        //        ret, sent, bytes);
        //fflush (stdout);
      }
      if (ogg_io->stream->connected == 0) {
        printke ("Failed!: %s\n", ogg_io->stream->error_str);
        return -1;
      }
    }
    if (ogg_io->stream->connected == 1) {
      kr_io2_restart (ogg_io->io);
    }
  } else {
    kr_io2_sync (ogg_io->io);
  }
  return 0;
}

int kr_ogg_io_push (kr_ogg_io *ogg_io, int track, int64_t gpos,
 uint8_t *buffer, size_t sz) {

  if (ogg_io == NULL) return -2;

  int32_t bytes;
  int32_t ret;

  if (ogg_io->wrote_header == 0) {
    bytes = kr_ogg_generate_header (ogg_io->ogg);
    if (bytes < 1) return -1;
    memcpy(ogg_io->io->buf, ogg_io->ogg->hdr, ogg_io->ogg->hdr_sz);
    kr_io2_advance (ogg_io->io, ogg_io->ogg->hdr_sz);
    ret = kr_ogg_io_push_internal(ogg_io);
    if (ret != 0) return -1;
    ogg_io->wrote_header = 1;
    if (sz == 0) {
      return 0;
    }
  }

  bytes = kr_ogg_add_data (ogg_io->ogg, track, gpos, buffer, sz, ogg_io->io->buf);
  kr_io2_advance (ogg_io->io, bytes);
  ret = kr_ogg_io_push_internal(ogg_io);
  if (ret != 0) return -1;
  return 0;
}

int kr_ogg_io_push_header (kr_ogg_io *ogg_io) {
  if (ogg_io == NULL) return -2;
  return kr_ogg_io_push (ogg_io, 0, 0, NULL, 0);
}

int kr_ogg_io_eos_track (kr_ogg_io *ogg_io, int track) {
  if (ogg_io == NULL) return -2;
  return kr_ogg_io_push (ogg_io, track, 0, NULL, 0);
}
