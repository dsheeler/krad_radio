#include <stdio.h>
#include <unistd.h>

#include <krad_transmitter.h>
#include <krad_ticker.h>

#include <krad_mkv_demux.h>

#include <krad_theora.h>
#include <krad_vorbis.h>
#include <krad_flac.h>

#include "krad_debug.c"

void krad_transmitter_mkv_test (int port, char *filename) {

  krad_transmitter_t *transmitter;
  krad_transmission_t *tx;
  kr_mkv_t *mkv;
  krad_ticker_t *ticker;

  int size;
  uint32_t track;
  uint64_t start_tc;
  uint64_t timecode;
  uint64_t last_tc;
  uint64_t sleeptime;
  uint8_t *buffer;
  int keyframe;
  uint8_t flags;
  int elements;
  
  start_tc = 0;
  last_tc = 0;
  elements = 0;

  char *stream_name = "stream.webm";
  char *content_type = "video/webm";

  buffer = malloc (10000000);

  mkv = kr_mkv_open_file (filename);

  if (mkv == NULL) {
    fprintf (stderr, "Could not open %s\n", filename);
    exit (1);
  }

  ticker = krad_ticker_throttle_create ();
  transmitter = krad_transmitter_create ();
  krad_transmitter_listen_on (transmitter, port);

  tx = krad_transmitter_transmission_create (transmitter,
                                             stream_name,
                                             content_type);

  printf ("Stream header is %zu bytes\n", mkv->stream_hdr_len);

  krad_transmitter_transmission_set_header (tx,
                                            mkv->stream_hdr,
                                            mkv->stream_hdr_len);

  while ((size = kr_mkv_read_streamable_raw_element (mkv,
                                                     &track,
                                                     &timecode,
                                                     &flags,
                                                     buffer)) > 0) {

    if (start_tc == 0) {
      start_tc = timecode;
      krad_ticker_start (ticker);
    }

    if ((track == 1) && (flags == 0x80)) {
      keyframe = 1;
    } else {
      keyframe = 0;
    }
    
    printf ("\rElement %10d track %d sync %d timecode %14"PRIu64"s",
            elements++, track, keyframe, timecode / 1000);
    fflush (stdout);
    
    krad_transmitter_transmission_add_data_opt (tx, buffer, size, keyframe);

    if ((last_tc) && (last_tc < timecode)) {
      sleeptime = (timecode - start_tc) - (last_tc - start_tc);
      krad_ticker_throttle (ticker, sleeptime);
    }
    if (timecode > last_tc) {
      last_tc = timecode;
    }
  }

  krad_ticker_destroy (ticker);
  krad_transmitter_transmission_destroy (tx);
  krad_transmitter_destroy (transmitter);
  kr_mkv_destroy (&mkv);
  free (buffer);
}

int main (int argc, char *argv[]) {

  krad_debug_init ("kr_txmtr");

  if (argc > 1) {
    if (argc == 2) {
      printf ("Need filename\n");
    } else {
      krad_transmitter_mkv_test (atoi(argv[1]), argv[2]);
    }
  } else {
    printf ("Need port\n");
  }

  krad_debug_shutdown ();

  return 0;
}
