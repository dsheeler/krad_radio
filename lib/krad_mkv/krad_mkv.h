#include <time.h>
#include <math.h>

#include "krad_mkv_ids.h"
#include "krad_ebml2.h"
#include "krad_codec_header.h"
#include "krad_radio_version.h"

#ifndef KRAD_MKV_H
#define KRAD_MKV_H

#define KRAD_MKV_VERSION "Krad MKV Version 6"

typedef struct kr_mkv_St kr_mkv_t;
typedef struct kr_mkv_track_St kr_mkv_track_t;

struct kr_mkv_track_St {
  uint64_t total_video_frames;
  uint64_t total_audio_frames;
 	int audio_frames_since_cluster;
  int fps_numerator;
  int fps_denominator;
  int width;
  int height;
  int sample_rate;
  krad_codec_t codec;
};

struct kr_mkv_St {

  kr_mkv_track_t *tracks;

  int track_count;
  int current_track;
  
  unsigned char *segment;
  unsigned char *tracks_info;

  unsigned char *cluster;
  uint64_t cluster_timecode;
    
  float segment_duration;
  uint64_t segment_size;
  uint64_t segment_timecode;

  kr_ebml2_t ebml;
  kr_ebml2_t *e;
};

int kr_mkv_destroy (kr_mkv_t **mkv);
kr_mkv_t *kr_mkv_create ();

#endif
