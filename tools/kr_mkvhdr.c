#include <stdio.h>
#include <unistd.h>

#include <krad_ogg2.h>

#include <krad_mkv_demux.h>

#include <krad_theora.h>
#include <krad_vorbis.h>
#include <krad_flac.h>
#include <krad_opus.h>

#include "krad_debug.c"

void mkv_header_test (kr_mkv_t *mkv) {

  int ret;
  int t;
  int h;
  int headers;
  int header_size;
  int tracks;
  krad_codec_t codec;
  krad_codec_header_t header;

  tracks = kr_mkv_track_count (mkv);

  printf ("Found %d tracks\n", tracks);

  for (t = 1; t < tracks + 1; t++) {
    headers = kr_mkv_track_header_count (mkv, t);
    printf ("Track %d has %d headers\n", t, headers);
    for (h = 0; h < headers; h++) {
      header_size = kr_mkv_track_header_size (mkv, t, h);
      printf ("Track %d header %d is %d bytes\n",
              t, h, header_size);  
    }
  }

  for (t = 1; t < tracks + 1; t++) {
    headers = kr_mkv_track_header_count (mkv, t);
    codec = kr_mkv_track_codec (mkv, t);

    if (codec == FLAC) {
      printf ("Testing Track %d header with FLAC decoder\n", t);
      header.header_count = headers;
      header.codec = codec;

      header.header_size[0] = kr_mkv_track_header_size (mkv, t, 0);
      header.header[0] = malloc (header.header_size[0]);
      ret = kr_mkv_read_track_header (mkv, header.header[0], t, 0);
      if (ret != header.header_size[0]) {
        printf ("Header read fail\n");  
        exit (1);
      }
      krad_flac_decoder_test (header.header[0], header.header_size[0]);
      header.header_size[0] = 0;
      free (header.header[0]);
    }

    if (codec == VORBIS) {
      printf ("Testing Track %d headers with Vorbis decoder\n", t);
      header.header_count = headers;
      header.codec = codec;
      for (h = 0; h < headers; h++) {
        header.header_size[h] = kr_mkv_track_header_size (mkv, t, h);
        header.header[h] = malloc (header.header_size[h]);
        ret = kr_mkv_read_track_header (mkv, header.header[h], t, h);
        if (ret != header.header_size[h]) {
          printf ("Header read fail\n");  
          exit (1);
        }
      }
      krad_vorbis_test_headers (&header);
      for (h = 0; h < headers; h++) {
        header.header_size[h] = 0;
        free (header.header[h]);
      }
    }

    if (codec == THEORA) {
      printf ("Testing Track %d headers with Theora decoder\n", t);
      header.header_count = headers;
      header.codec = codec;
      for (h = 0; h < headers; h++) {
        header.header_size[h] = kr_mkv_track_header_size (mkv, t, h);
        header.header[h] = malloc (header.header_size[h]);
        ret = kr_mkv_read_track_header (mkv, header.header[h], t, h);
        if (ret != header.header_size[h]) {
          printf ("Header read fail\n");  
          exit (1);
        }
      }
      krad_theora_test_headers (&header);
      for (h = 0; h < headers; h++) {
        header.header_size[h] = 0;
        free (header.header[h]);
      }
    }
  }
}

void ogg_test () {

  krad_vorbis_t *vorbis;
  krad_theora_encoder_t *theora;
  krad_opus_t *opus;
  kr_ogg_t *ogg;
  int track;
  int i;
  int pages;
  size_t page_size;
  size_t total_size;
  char *data = "";
  uint8_t page[5000];
  int64_t granule_position;

  granule_position = 0;
  total_size = 0;
  pages = 1;

  ogg = kr_ogg_create ();

  opus = krad_opus_encoder_create (2, 48000, 128000, OPUS_APPLICATION_AUDIO);
  theora = krad_theora_encoder_create (640, 480, 30, 1, 420, 32);
  vorbis = krad_vorbis_encoder_create (2, 48000, 0.5);

  track = kr_ogg_add_track (ogg, &theora->krad_codec_header);
  track = kr_ogg_add_track (ogg, &vorbis->krad_codec_header);

  track = kr_ogg_add_track (ogg, &opus->krad_codec_header);

  kr_ogg_generate_header (ogg);

  write (STDOUT_FILENO, ogg->hdr, ogg->hdr_sz);

  for (i = 0; i < pages; i++) {
    granule_position += 0;
    page_size = kr_ogg_add_data (ogg, track, granule_position,
                                (uint8_t *)data, strlen(data), page);
    write (STDOUT_FILENO, page, page_size);
    total_size += page_size;

    fprintf (stderr, "Page size was %zu\n", page_size);
  }

  fprintf (stderr, "Total size was %zu\n", total_size);

  kr_ogg_destroy (&ogg);
  krad_opus_encoder_destroy (opus);  
  krad_vorbis_encoder_destroy (vorbis);
  krad_theora_encoder_destroy (theora);
}

int main (int argc, char *argv[]) {

  ogg_test ();

  return 0;

  int32_t ret;
  kr_mkv_t *mkv;

  krad_debug_init ("kr_mkvhdrtest");
 
  mkv = kr_mkv_open_file (argv[1]);
 
  if (mkv == NULL) {
    printf ("Error opening %s\n", argv[1]);
    return 1;
  }
  
  mkv_header_test (mkv);

  if (mkv != NULL) {
    ret = kr_mkv_destroy (&mkv);
    if (ret < 0) {
      printf ("Error closing %s\n", argv[1]);
    }
  }
  
  krad_debug_shutdown ();

  return 0;
}
