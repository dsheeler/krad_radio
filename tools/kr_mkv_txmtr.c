#include <stdio.h>
#include <unistd.h>

#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>

#include <krad_theora.h>
#include <krad_vorbis.h>
#include <krad_flac.h>

#include "krad_debug.c"

#define VIDEO_TRACK 1
#define AUDIO_TRACK 2

void krad_transmitter_mkv_test2 (int port, char *filename1, char *filename2) {

  krad_transmitter_t *transmitter;
  kr_mkv_t *mkv_tx;
  kr_mkv_t *in[2];
  krad_ticker_t *ticker;

  int i;
  uint32_t track;
  int bytes_read;  
  uint32_t out_track;
  uint64_t start_tc;
  uint64_t timecode;
  uint64_t last_tc;
  uint64_t sleeptime;
  uint8_t *buffer;
  int keyframe;
  uint8_t flags;
  int packets;
  
  start_tc = 0;
  last_tc = 0;
  packets = 0;

  char *stream_name = "stream.webm";
  char *content_type = "video/webm";

  buffer = malloc (10000000);

  
  for (i = 0; i < 2; i++) {
  
    if (i == 0) {
      in[i] = kr_mkv_open_file (filename1);
    } else {
      in[i] = kr_mkv_open_file (filename2);
    }

    
    printf ("File %d: FPS %d/%d RES %dx%d\n",
            i, 
            in[i]->tracks[VIDEO_TRACK].fps_numerator,
            in[i]->tracks[VIDEO_TRACK].fps_denominator,
            in[i]->tracks[VIDEO_TRACK].width,
            in[i]->tracks[VIDEO_TRACK].height);

    if ((i == 0) && (in[i] == NULL)) {
      fprintf (stderr, "Could not open input file1: %s\n", filename1);
      exit (1);
    }
    if ((i == 1) && (in[i] == NULL)) {
      fprintf (stderr, "Could not open input file2: %s\n", filename2);
      exit (1);
    }
  }
 
 
  if ((in[0]->tracks[VIDEO_TRACK].width != in[0]->tracks[VIDEO_TRACK].width) ||
      (in[1]->tracks[VIDEO_TRACK].height != in[1]->tracks[VIDEO_TRACK].height)) {

    fprintf (stderr, "Resolutions are not equal\n");
    exit (1);
  }

  ticker = krad_ticker_throttle_create ();
  transmitter = krad_transmitter_create ();
  krad_transmitter_listen_on (transmitter, port);

  mkv_tx = kr_mkv_create_transmission (transmitter, stream_name, content_type);

  out_track = kr_mkv_add_video_track (mkv_tx, VP8,
                                      30,
                                      1,
                                      in[0]->tracks[VIDEO_TRACK].width,
                                      in[0]->tracks[VIDEO_TRACK].height);

  printf ("Added new track: %d\n", out_track);

  for (i = 0; i < 2; i++) {
    while ((bytes_read = kr_mkv_read_packet (in[i], &track, &timecode, &flags, buffer)) > 0) {

      if (start_tc == 0) {
        start_tc = timecode;
        krad_ticker_start (ticker);
      }

      if (flags == 0x80) {
        keyframe = 1;
      } else {
        keyframe = 0;
      }

      printf ("\rFile %d Packet %10d track %d sync %d timecode %14"PRIu64"s",
              i, packets++, track, keyframe, timecode / 1000);
      fflush (stdout);

      if (track == 1) {
        kr_mkv_add_video (mkv_tx, out_track, buffer, bytes_read, keyframe);
      }

      if ((last_tc) && (last_tc < timecode)) {
        sleeptime = (timecode - start_tc) - (last_tc - start_tc);
        krad_ticker_throttle (ticker, sleeptime);
      }
      if (timecode > last_tc) {
        last_tc = timecode;
      }
    }
  }  

  krad_ticker_destroy (ticker);
  kr_mkv_destroy (&in[0]);
  kr_mkv_destroy (&in[1]);
  kr_mkv_destroy (&mkv_tx);
  krad_transmitter_destroy (transmitter);
  free (buffer);
}

int main (int argc, char *argv[]) {

  krad_debug_init ("kr_mkv_txmtr");

  if (argc > 1) {
    if (argc == 2) {
      printf ("Need filename\n");
    } else {
      if (argc == 3) {
        printf ("Need filename2\n");
      } else {
        krad_transmitter_mkv_test2 (atoi(argv[1]), argv[2], argv[3]);
      }
    }
  } else {
    printf ("Need port\n");
  }

  krad_debug_shutdown ();

  return 0;
}
