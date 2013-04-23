#include <stdio.h>
#include <unistd.h>

#include <krad_mkv_demux.h>

#include <krad_theora.h>
#include <krad_vorbis.h>
#include <krad_flac.h>

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

int main (int argc, char *argv[]) {

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
