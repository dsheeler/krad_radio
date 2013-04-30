#include <stdio.h>
#include <unistd.h>

#include <krad_muxponder.h>
#include <krad_transmitter.h>
#include <krad_ticker.h>
#include <krad_mkv_demux.h>

#include <krad_theora.h>
#include <krad_vorbis.h>
#include <krad_flac.h>

#include "krad_debug.c"

#define VIDEO_TRACK 1
#define AUDIO_TRACK 2

void krad_muxponder_test (int port, char *filename1, char *filename2) {

  kr_muxponder_t *muxponder;
  krad_transmitter_t *transmitter;
  kr_track_info_t track_info;
  kr_muxer_output_params_t output_params;

  kr_mkv_t *in[2];
  krad_ticker_t *ticker;

  int i;
  int ret;
  int first_file_output;
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
  char stream_name[256];
  char file_name[256];
  char *content_type;
  
  start_tc = 0;
  last_tc = 0;
  packets = 0;

  memset (&track_info, 0, sizeof(kr_track_info_t));
  memset (&output_params, 0, sizeof(kr_muxer_output_params_t));

  content_type = "video/webm";
  snprintf (stream_name, sizeof(stream_name), "stream_%"PRIu64".webm",
            krad_unixtime());
  snprintf (file_name, sizeof(file_name), "%s/file_%"PRIu64".webm",
            getenv ("HOME"), krad_unixtime());

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

  ret = krad_transmitter_listen_on (transmitter, port);

  if (ret != 0) {
    fprintf (stderr, "transmitter could not listen on port %d\n", port);
    exit (1);
  }

  muxponder = kr_muxponder_create (transmitter);

  track_info.header.codec = VP8;
  track_info.params.v.width = in[0]->tracks[VIDEO_TRACK].width;
  track_info.params.v.height = in[0]->tracks[VIDEO_TRACK].height;

  out_track = kr_muxponder_create_track (muxponder, &track_info);

  printf ("Added new track: %d\n", out_track);

  char *host = "europa.kradradio.com";
  int stream_port = 80;
  char *password = "firefox";    

  output_params.container = MKV;
  output_params.transport = STREAM;
  output_params.transport_params.stream_output_params.host = host;
  output_params.transport_params.stream_output_params.port = stream_port;
  output_params.transport_params.stream_output_params.mount = stream_name;
  output_params.transport_params.stream_output_params.password = password;  

  kr_muxponder_create_output (muxponder, &output_params);

  output_params.container = MKV;
  output_params.transport = TRANSMISSION;
  output_params.transport_params.transmission_output_params.mount = stream_name;
  output_params.transport_params.transmission_output_params.content_type = content_type;

  kr_muxponder_create_output (muxponder, &output_params);

  output_params.container = MKV;
  output_params.transport = LOCAL_FILE;
  output_params.transport_params.file_output_params.filename = file_name;

  first_file_output = kr_muxponder_create_output (muxponder, &output_params);

  for (i = 0; i < 2; i++) {
  
    if (i == 1) {
    
      snprintf (file_name, sizeof(file_name), "%s/file_%"PRIu64".webm",
                getenv ("HOME"), krad_unixtime());
      output_params.container = MKV;
      output_params.transport = LOCAL_FILE;
      output_params.transport_params.file_output_params.filename = file_name;

      kr_muxponder_create_output (muxponder, &output_params);
    
      kr_muxponder_destroy_output (muxponder, first_file_output);
    
    }

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
        kr_muxponder_add_data (muxponder, out_track, timecode, buffer, bytes_read, keyframe);
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
  kr_muxponder_destroy (&muxponder);  
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
        krad_muxponder_test (atoi(argv[1]), argv[2], argv[3]);
      }
    }
  } else {
    printf ("Need port\n");
  }

  krad_debug_shutdown ();

  return 0;
}
