#include <stdio.h>
#include <unistd.h>

#include <krad_transmitter.h>

#include <krad_mkv_demux.h>

#include <krad_theora.h>
#include <krad_vorbis.h>
#include <krad_flac.h>

#include "krad_debug.c"

void krad_transmitter_test (int port) {

	krad_transmitter_t *krad_transmitter;
	krad_transmission_t *krad_transmission;

	int count;
	int test_count;
	char data[256];
	int data_len;
	int sync_point;

	test_count = 60000;
	count = 0;
	sync_point = 0;

	char *stream_name = "stream.txt";
	char *content_type = "text/plain";

	char *stream_header = "This is the header of the text stream.\n";

	krad_transmitter = krad_transmitter_create ();
	
	krad_transmitter_listen_on (krad_transmitter, port);
	
	krad_transmission = krad_transmitter_transmission_create (krad_transmitter, stream_name, content_type);
	krad_transmitter_transmission_set_header (krad_transmission, (uint8_t *)stream_header, strlen(stream_header));
	
	data_len = sprintf (data, "The first sentence ever in the stream, key of course.\n");
	krad_transmitter_transmission_add_data_sync (krad_transmission, (uint8_t *)data, data_len);
	
	for (count = 1; count < test_count; count++) {

		if ((count % 5) == 0) {
			sync_point = 1;
			data_len = sprintf (data, "\nSentence number %d, a very key sentence.\n", count);
		} else {
			sync_point = 0;
			data_len = sprintf (data, "Sentence number %d.\n", count);
		}

		krad_transmitter_transmission_add_data_opt (krad_transmission, (uint8_t *)data, data_len, sync_point);

		usleep (100000);
	}
	
	krad_transmitter_transmission_destroy (krad_transmission);
	
	krad_transmitter_destroy (krad_transmitter);
}

void krad_transmitter_mkv_test (int port, char *filename) {

	krad_transmitter_t *transmitter;
	krad_transmission_t *transmission;
  kr_mkv_t *mkv;

  int bytes_read;
  uint32_t track;
  uint64_t timecode;
  uint8_t *buffer;
  int keyframe;
  uint8_t flags;
  int elements;
  
  elements = 0;

	char *stream_name = "stream.webm";
	char *content_type = "video/webm";

  buffer = malloc (10000000);

  mkv = kr_mkv_open_file (filename);

  if (mkv == NULL) {
    fprintf (stderr, "Could not open %s\n", filename);
    exit (1);
  }

	transmitter = krad_transmitter_create ();
	krad_transmitter_listen_on (transmitter, port);
	
	transmission = krad_transmitter_transmission_create (transmitter,
	                                                     stream_name,
	                                                     content_type);

  printf ("Stream header is %zu bytes\n", mkv->stream_hdr_len);

	krad_transmitter_transmission_set_header (transmission,
	                                          mkv->stream_hdr,
	                                          mkv->stream_hdr_len);

  while ((bytes_read = kr_mkv_read_streamable_raw_element (mkv, &track, &timecode, &flags, buffer)) > 0) {

    if (flags == 0x80) {
      keyframe = 1;
    } else {
      keyframe = 0;
    }
		
    printf ("\rRead streamable_raw_element %d track %d sync %d timecode %"PRIu64" %d bytes\n", elements++, track, keyframe, timecode, bytes_read);
    fflush (stdout);
		
    krad_transmitter_transmission_add_data_opt (transmission, buffer, bytes_read, keyframe);

		usleep (100000);

  }

	krad_transmitter_transmission_destroy (transmission);
	krad_transmitter_destroy (transmitter);
  kr_mkv_destroy (&mkv);
  free (buffer);
}

int main (int argc, char *argv[]) {

  krad_debug_init ("kr_txmtr");

	if (argc > 1) {
	  if (argc == 2) {
		  krad_transmitter_test (atoi(argv[1]));
	  } else {
		  krad_transmitter_mkv_test (atoi(argv[1]), argv[2]);
	  }
	} else {
		printf ("Need port\n");
	}

  krad_debug_shutdown ();

  return 0;
}
