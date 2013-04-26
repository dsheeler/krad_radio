#include <time.h>
#include <math.h>

#include "krad_mkv_ids.h"
#include "krad_ebml2.h"
#include "krad_io2.h"
#include "krad_file.h"
#include "krad_stream.h"
#include "krad_codec_header.h"
#include "krad_radio_version.h"

#ifndef KRAD_MKV_H
#define KRAD_MKV_H

#define KRAD_MKV_VERSION "Krad MKV Version 6"
#define KR_MKV_MAX_TRACKS 10

typedef struct kr_mkv_St kr_mkv_t;
typedef struct kr_mkv_track_St kr_mkv_track_t;

struct kr_mkv_track_St {
  uint32_t fps_numerator;
  uint32_t fps_denominator;
  uint32_t width;
  uint32_t height;
  
  uint32_t channels;
  uint32_t bit_depth;
  uint32_t sample_rate;

  krad_codec_t codec;

  uint8_t *codec_data;
  size_t codec_data_size;

  uint32_t headers;
  uint8_t *header[3];
  size_t header_len[3];

  uint64_t total_video_frames;
  uint64_t total_audio_frames;
  uint32_t audio_frames_since_cluster;
  uint32_t changed;
};

struct kr_mkv_St {

  kr_mkv_track_t *tracks;

  int video_tracks;
  int seen_keyframe;

  int track_count;
  int current_track;

  unsigned char *segment;
  unsigned char *tracks_info;

  unsigned char *cluster;

  uint64_t current_timecode;
  uint64_t cluster_timecode;

  float segment_duration;
  uint64_t segment_size;
  uint64_t segment_timecode;

  kr_ebml2_t ebml;
  kr_ebml2_t *e;
  
  int fd;
  kr_io2_t *io;
};

int kr_mkv_destroy (kr_mkv_t **mkv);
kr_mkv_t *kr_mkv_create ();
kr_mkv_t *kr_mkv_create_bufsize (size_t size);

kr_mkv_t *kr_mkv_create_file (char *filename);
kr_mkv_t *kr_mkv_stream (char *host, int port, char *mount, char *password);

void kr_mkv_start_segment (kr_mkv_t *mkv, char *title);

int kr_mkv_add_video_track_with_private_data (kr_mkv_t *mkv,
                                              krad_codec_t codec,
                                              int fps_numerator,
                                              int fps_denominator,
                                              int width, int height,
                                              unsigned char *priv_data,
                                              int priv_data_size);

int kr_mkv_add_video_track (kr_mkv_t *mkv, krad_codec_t codec,
                            int fps_numerator, int fps_denominator,
                            int width, int height);

int kr_mkv_add_audio_track (kr_mkv_t *mkv, krad_codec_t codec,
                            uint32_t sample_rate, uint8_t channels,
                            unsigned char *priv_data,
                            int priv_data_size);

int kr_mkv_add_subtitle_track (kr_mkv_t *mkv, char *codec_id);

void kr_mkv_add_video (kr_mkv_t *mkv, int track_num, unsigned char *buffer,
                       int buffer_len, int keyframe);

void kr_mkv_add_audio (kr_mkv_t *mkv, int track_num, unsigned char *buffer,
                       int buffer_len, int frames);

#endif
