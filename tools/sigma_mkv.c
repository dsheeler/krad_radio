#include <stdio.h>
#include <unistd.h>
 
#include <krad_mkv_demux.h>

#include <krad_vorbis.h>
#include <krad_coder_common.h>

#include "krad_debug.c"

#define VIDEO_TRACK 1
#define AUDIO_TRACK 2
 
static void remuxcode (kr_mkv_t *mkv, char *file) {

  int bytes_read;
  uint32_t track;
  uint64_t timecode;
  uint8_t *buffer;
  uint32_t out_track;
  kr_mkv_t *new_mkv;
  int keyframe;
  uint8_t flags;
  int packets;
  
  kr_codec_hdr_t header;
  
  kr_medium_t *medium;
  kr_codeme_t *codeme;
  krad_vorbis_t *vorbis_dec;
  krad_vorbis_t *vorbis_enc;

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


  header.header_count = mkv->tracks[AUDIO_TRACK].headers;
  header.header[0] = mkv->tracks[AUDIO_TRACK].header[0];
  header.header_size[0] = mkv->tracks[AUDIO_TRACK].header_len[0];
  header.header[1] = mkv->tracks[AUDIO_TRACK].header[1];
  header.header_size[1] = mkv->tracks[AUDIO_TRACK].header_len[1];
  header.header[2] = mkv->tracks[AUDIO_TRACK].header[2];
  header.header_size[2] = mkv->tracks[AUDIO_TRACK].header_len[2];

  vorbis_dec = krad_vorbis_decoder_create (&header);

  vorbis_enc = krad_vorbis_encoder_create (2, 48000, 0.4);

  kr_mkv_add_audio_track (new_mkv, VORBIS,
                            48000, 2,
                            vorbis_enc->krad_codec_header.header_combined,
                            vorbis_enc->krad_codec_header.header_combined_size);

  printf ("\n");

  while ((bytes_read = kr_mkv_read_packet (mkv, &track, &timecode, &flags, buffer)) > 0) {
    
    printf ("\rRead packet %d track %d %d bytes\t\t", packets++, track, bytes_read);
    fflush (stdout);

    if (flags == 0x80) {
      keyframe = 1;
    } else {
      keyframe = 0;
    }

    if (track == 1) {
      kr_mkv_add_video (new_mkv, out_track, buffer, bytes_read, keyframe);
    }
    
    if (track == 2) {

      medium = kr_medium_kludge_create ();
      codeme = kr_codeme_kludge_create ();
      
      codeme->sz = bytes_read;
      memcpy (codeme->data, buffer, codeme->sz);

      kr_vorbis_decode (vorbis_dec, codeme, medium);

      kr_codeme_kludge_destroy (&codeme);
      codeme = kr_codeme_kludge_create ();

      kr_vorbis_encode (vorbis_enc, codeme, medium);

      kr_mkv_add_audio (new_mkv, 2, codeme->data, codeme->sz, codeme->count);

      kr_codeme_kludge_destroy (&codeme);
      kr_medium_kludge_destroy (&medium);
    }    
  }

  printf ("\nDone.\n");
  krad_vorbis_decoder_destroy (vorbis_dec);  
  krad_vorbis_encoder_destroy (vorbis_enc);
  kr_mkv_destroy (&new_mkv);
  free (buffer);
}

static void remux (kr_mkv_t *mkv, char *file) {

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

    if (track == 1) {
      kr_mkv_add_video (new_mkv, out_track, buffer, bytes_read, keyframe);
    }
  }

  printf ("\nDone.\n");

  kr_mkv_destroy (&new_mkv);
  free (buffer);
}

static void splice (char *file1, char *file2, char *fileout) {

  kr_mkv_t *in[2];
  kr_mkv_t *out;
  int i;
  int bytes_read;
  uint32_t track;
  uint64_t timecode;
  uint8_t *buffer;
  uint32_t out_track;
  int keyframe;
  uint8_t flags;
  int packets;
  
  packets = 0;

  buffer = malloc (10000000);
  
  for (i = 0; i < 2; i++) {
  
    if (i == 0) {
      in[i] = kr_mkv_open_file (file1);
    } else {
      in[i] = kr_mkv_open_file (file2);
    }

    
    printf ("File %d: FPS %d/%d RES %dx%d\n",
            i, 
            in[i]->tracks[VIDEO_TRACK].fps_numerator,
            in[i]->tracks[VIDEO_TRACK].fps_denominator,
            in[i]->tracks[VIDEO_TRACK].width,
            in[i]->tracks[VIDEO_TRACK].height);

    if (in[0] == NULL) {
      fprintf (stderr, "Could not open input file: %s\n", file1);
      exit (1);
    }
  }
 
 
  if ((in[0]->tracks[VIDEO_TRACK].width != in[0]->tracks[VIDEO_TRACK].width) ||
      (in[1]->tracks[VIDEO_TRACK].height != in[1]->tracks[VIDEO_TRACK].height)) {

    fprintf (stderr, "Resolutions are not equal\n");
    exit (1);
  }
 
 
  out = kr_mkv_create_file (fileout);

  if (out == NULL) {
    fprintf (stderr, "Could not open output file: %s\n", fileout);
    exit (1);
  }

  printf ("Created file: %s\n", fileout);

  out_track = kr_mkv_add_video_track (out, VP8,
                                     30,
                                     1,
                                     in[0]->tracks[VIDEO_TRACK].width,
                                     in[0]->tracks[VIDEO_TRACK].height);

  printf ("Added new track: %d\n", out_track);
  
  for (i = 0; i < 2; i++) {
    while ((bytes_read = kr_mkv_read_packet (in[i], &track, &timecode, &flags, buffer)) > 0) {
      
      printf ("\rRead file %d packet %d track %d %d bytes\t\t",
              i, packets++, track, bytes_read);
      fflush (stdout);

      if (flags == 0x80) {
        keyframe = 1;
      } else {
        keyframe = 0;
      }

      if (track == 1) {
        kr_mkv_add_video (out, out_track, buffer, bytes_read, keyframe);
      }
    }
  }
  
  printf ("\nDone.\n");  
  
  free (buffer);
  kr_mkv_destroy (&in[0]);
  kr_mkv_destroy (&in[1]);  
  kr_mkv_destroy (&out);

}

static void kraise_splice (char *file1, char *file2, char *fileout) {

  kr_mkv_t *in[2];
  kr_mkv_t *out;
  int i;
  int bytes_read;
  uint32_t track;
  uint64_t timecode;
  uint8_t *buffer[2];
  uint32_t out_track;
  int keyframe;
  uint8_t flags;
  int packets;
  int have_buffer[2];
  int have_buffer_bytes[2];  
  int inframes;
  
  have_buffer_bytes[0] = 0;
  have_buffer_bytes[1] = 0;
  have_buffer[1] = 0;
  inframes = 0;
  have_buffer[0] = 0;
  have_buffer[1] = 0;
  packets = 0;

  buffer[0] = malloc (10000000);
  buffer[1] = malloc (10000000);
  
  for (i = 0; i < 2; i++) {
  
    if (i == 0) {
      in[i] = kr_mkv_open_file (file1);
    } else {
      in[i] = kr_mkv_open_file (file2);
    }

    
    printf ("File %d: FPS %d/%d RES %dx%d\n",
            i, 
            in[i]->tracks[VIDEO_TRACK].fps_numerator,
            in[i]->tracks[VIDEO_TRACK].fps_denominator,
            in[i]->tracks[VIDEO_TRACK].width,
            in[i]->tracks[VIDEO_TRACK].height);

    if (in[0] == NULL) {
      fprintf (stderr, "Could not open input file: %s\n", file1);
      exit (1);
    }
  }
 
 
  if ((in[0]->tracks[VIDEO_TRACK].width != in[0]->tracks[VIDEO_TRACK].width) ||
      (in[1]->tracks[VIDEO_TRACK].height != in[1]->tracks[VIDEO_TRACK].height)) {

    fprintf (stderr, "Resolutions are not equal\n");
    exit (1);
  }
 
 
  out = kr_mkv_create_file (fileout);

  if (out == NULL) {
    fprintf (stderr, "Could not open output file: %s\n", fileout);
    exit (1);
  }

  printf ("Created file: %s\n", fileout);

  out_track = kr_mkv_add_video_track (out, VP8,
                                     30,
                                     1,
                                     in[0]->tracks[VIDEO_TRACK].width,
                                     in[0]->tracks[VIDEO_TRACK].height);

  printf ("Added new track: %d\n", out_track);




  while ((bytes_read = kr_mkv_read_packet (in[0], &track, &timecode, &flags, buffer[0])) > 0) {
    
    have_buffer_bytes[0] = bytes_read;
    
    printf ("Read file %d packet %d track %d %d bytes\n",
            1, packets++, track, bytes_read);
    fflush (stdout);

    if (flags == 0x80) {
      keyframe = 1;
    } else {
      keyframe = 0;
    }

    if (keyframe == 1) {
      inframes = 0;
      while ((have_buffer[1] == 1) || 
             (((bytes_read = kr_mkv_read_packet (in[1], &track, &timecode, &flags, buffer[1])) > 0))) {
        
        if (have_buffer[1] == 1) {
          keyframe = 1;
          have_buffer[1] = 0;
        } else {
          printf ("Read file %d packet %d track %d %d bytes\n",
                  2, packets++, track, bytes_read);
          fflush (stdout);

          if (flags == 0x80) {
            keyframe = 1;
          } else {
            keyframe = 0;
          }
          
          have_buffer_bytes[1] = bytes_read;
          
          if ((keyframe == 1) && (inframes > 0)) {
            have_buffer[1] = 1;
            break;
          }
        }
        if (track == 1) {
          kr_mkv_add_video (out, out_track, buffer[1], have_buffer_bytes[1], keyframe);
        }
        inframes++;
      }
    }

    if (track == 1) {
      kr_mkv_add_video (out, out_track, buffer[0], have_buffer_bytes[0], keyframe);
    }
  }

  printf ("\nDone.\n");
  
  free (buffer[0]);
  free (buffer[1]);
  kr_mkv_destroy (&in[0]);
  kr_mkv_destroy (&in[1]);  
  kr_mkv_destroy (&out);

}

static void other_thing (char *file1, char *file2) {

  kr_mkv_t *mkv;

  mkv = kr_mkv_open_file (file1);
  
  printf ("FPS %d/%d RES: %dx%d\n", 
          mkv->tracks[VIDEO_TRACK].fps_numerator,
          mkv->tracks[VIDEO_TRACK].fps_denominator,
          mkv->tracks[VIDEO_TRACK].width,
          mkv->tracks[VIDEO_TRACK].height);

  if (mkv == NULL) {
    fprintf (stderr, "Could not open %s\n", file1);
    exit (1);
  }
  
//  remux (mkv, file2);

  remuxcode (mkv, file2);

  kr_mkv_destroy (&mkv);
}
 
int main (int argc, char *argv[])  {

  krad_debug_init ("kr_sigmamkv");

  if (argc > 3) {
    if (argc == 5) {
      kraise_splice (argv[1], argv[2], argv[3]);
    }
    if (argc == 4) {
      splice (argv[1], argv[2], argv[3]);
    }
  } else {
    if (argc == 3) {
      other_thing (argv[1], argv[2]);
    }
  }

  return 0;
}
