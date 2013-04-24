#include <stdio.h>
#include <unistd.h>
 
#include <krad_mkv_demux.h>
 
#define VIDEO_TRACK 1
#define AUDIO_TRACK 2
 
void remux (kr_mkv_t *mkv, char *file) {

  int bytes_read;
  uint32_t track;
  uint64_t timecode;
  uint8_t *buffer;
  uint32_t out_track;
  kr_mkv_t *new_mkv;
  int keyframe;
  uint8_t flags;
  int packets;
  
  packets = 0;

  buffer = malloc (10000000);
 
  new_mkv = kr_mkv_create_file (file);

  if (new_mkv == NULL) {
    fprintf (stderr, "Could not open %s\n", file);
    exit (1);
  }

  printf ("Created file: %s\n", file);

  out_track = kr_mkv_add_video_track (new_mkv, VP8,
                                     30,
                                     1,
                                     mkv->tracks[VIDEO_TRACK].width,
                                     mkv->tracks[VIDEO_TRACK].height);

  printf ("Added new track: %d\n", out_track);

  printf ("\n");

  while ((bytes_read = kr_mkv_read_packet (mkv, &track, &timecode, &flags, buffer)) > 0) {
    
    printf ("\rRead packet %d track %d %d bytes\t\t", packets++, track, bytes_read);
    fflush (stdout);

    if (flags == 0x80) {
      keyframe = 1;
    } else {
      keyframe = 0;
    }

    //if (track == 0) {
      kr_mkv_add_video (new_mkv, out_track, buffer, bytes_read, keyframe);
    //}
  }

  printf ("\nDone.\n");

  //kr_mkv_destroy (&new_mkv);
  free (buffer);
}
 
int main (int argc, char *argv[])  {
  
  kr_mkv_t *mkv;

  mkv = kr_mkv_open_file (argv[1]);
  
  printf ("FPS %d/%d RES: %dx%d\n", 
          mkv->tracks[VIDEO_TRACK].fps_numerator,
          mkv->tracks[VIDEO_TRACK].fps_denominator,
          mkv->tracks[VIDEO_TRACK].width,
          mkv->tracks[VIDEO_TRACK].height);

  if (mkv == NULL) {
    fprintf (stderr, "Could not open %s\n", argv[1]);
    return 1;
  }
  
  remux (mkv, argv[2]);
  kr_mkv_destroy (&mkv);


  return 0;
}
