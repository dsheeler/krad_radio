#include <stdio.h>
#include <unistd.h>

#include <krad_mkv_demux.h>

#include "krad_debug.c"

void mkv_frames_print (kr_mkv_t *mkv) {

  int ret;
  uint32_t track;
  uint64_t timecode;
  uint8_t *buffer;
  
  buffer = malloc (10000000);

  do {
    ret = kr_mkv_read_packet (mkv, &track, &timecode, NULL, buffer);
    if (ret > 0) {
      printf ("Got packet for track %d, size %d, timecode %"PRIu64"\n",
              track, ret, timecode);
    }
  } while (ret > 0);

  free (buffer);
}

void mkv_info_print (kr_mkv_t *mkv) {

  int t;
  int h;
  uint32_t headers;
  uint32_t header_size;
  uint32_t tracks;

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
}

int main (int argc, char *argv[]) {

  int32_t ret;
  kr_mkv_t *mkv;

  krad_debug_init ("mkvinfo");
 
  mkv = kr_mkv_open_file (argv[1]);
 
  if (mkv == NULL) {
    printf ("Error opening %s\n", argv[1]);
    return 1;
  }
  
  mkv_info_print (mkv);
  
  if (argc > 2) {
    mkv_frames_print (mkv);
  }

  if (mkv != NULL) {
    ret = kr_mkv_destroy (&mkv);
    if (ret < 0) {
      printf ("Error closing %s\n", argv[1]);
    }
  }
  
  krad_debug_shutdown ();

  return 0;
}
