#include "krad_mkv.h"

static char *kr_codec_to_mkv_codec (krad_codec_t codec);
static void kr_mkv_cluster (kr_mkv_t *mkv, int64_t timecode);
static int kr_mkv_new_tracknumber (kr_mkv_t *mkv);
static int kr_mkv_generate_track_uid (int track_number);

static char *kr_codec_to_mkv_codec (krad_codec_t codec) {
  switch (codec) {
    case VORBIS:
      return "A_VORBIS";
    case FLAC:
      return "A_FLAC";
    case OPUS:
      return "A_OPUS";
    case VP8:
      return "V_VP8";
    case KVHS:
      return "V_KVHS";  
    case THEORA:
      return "V_THEORA";
    case MJPEG:
      return "V_MJPEG";
    case H264:
      return "V_MPEG4/ISO/AVC";  
    default:
      return "No Codec";
  }
}

static int kr_mkv_new_tracknumber (kr_mkv_t *mkv) {

  int tracknumber;

  if (mkv->current_track == KR_MKV_MAX_TRACKS) {
    return -1;
  }

  tracknumber = mkv->current_track;

  mkv->track_count++;
  mkv->current_track++;

  return tracknumber;
}

static int kr_mkv_generate_track_uid (int track_number) {

  uint64_t t;
  uint64_t r;
  uint64_t rval;
  
  t = time (NULL) * track_number;
  r = rand ();
  r = r << 32;
  r += rand ();
  rval = t ^ r;

  return rval;
}

void kr_mkv_start_segment (kr_mkv_t *mkv, char *title) {

  unsigned char *segment_info;
  
  if (title == NULL) {
    title = "A Krad Radio Creation";
  }

  //FIXME
  //make sure title is nullterm and not insane len

  kr_ebml2_start_element (mkv->e, MKV_SEGMENT, &mkv->segment);
  kr_ebml2_start_element (mkv->e, MKV_SEGMENT_INFO, &segment_info);
  kr_ebml2_pack_string (mkv->e, MKV_SEGMENT_TITLE, title);
  kr_ebml2_pack_int32 (mkv->e, MKV_TIMECODESCALE, 1000000);
  kr_ebml2_pack_string (mkv->e, MKV_MUXINGAPP, KRAD_MKV_VERSION);
  kr_ebml2_pack_string (mkv->e, MKV_WRITINGAPP, KRAD_VERSION_STRING);
  kr_ebml2_finish_element (mkv->e, segment_info);

  kr_ebml2_start_element (mkv->e, MKV_SEGMENT_TRACKS, &mkv->tracks_info);
}

int kr_mkv_add_video_track_with_private_data (kr_mkv_t *mkv,
                                              krad_codec_t codec,
                                              int fps_numerator,
                                              int fps_denominator,
                                              int width, int height,
                                              unsigned char *priv_data,
                                              int priv_data_size) {
  int t;
  unsigned char *track_info;
  unsigned char *video_info;

  t = kr_mkv_new_tracknumber (mkv);
  if (t == -1) {
    return t;
  }

  mkv->tracks[t].fps_numerator = fps_numerator;
  mkv->tracks[t].fps_denominator = fps_denominator;
  mkv->tracks[t].width = width;
  mkv->tracks[t].height = height;

  kr_ebml2_start_element (mkv->e, MKV_TRACK, &track_info);
  kr_ebml2_pack_int8 (mkv->e, MKV_TRACKNUMBER, t);
  kr_ebml2_pack_int64 (mkv->e, MKV_TRACK_UID, kr_mkv_generate_track_uid (t));
  kr_ebml2_pack_string (mkv->e, MKV_CODECID, kr_codec_to_mkv_codec (codec));
  kr_ebml2_pack_int8 (mkv->e, MKV_TRACKTYPE, 1);

  if (priv_data_size > 0) {
    kr_ebml2_pack_data (mkv->e, MKV_CODECDATA, priv_data, priv_data_size);
  }

  kr_ebml2_start_element (mkv->e, MKV_VIDEOSETTINGS, &video_info);
  kr_ebml2_pack_int16 (mkv->e, MKV_VIDEOWIDTH, mkv->tracks[t].width);
  kr_ebml2_pack_int16 (mkv->e, MKV_VIDEOHEIGHT, mkv->tracks[t].height);
  kr_ebml2_finish_element (mkv->e, video_info);
  kr_ebml2_finish_element (mkv->e, track_info);  

  return t;
}

int kr_mkv_add_video_track (kr_mkv_t *mkv, krad_codec_t codec,
                            int fps_numerator, int fps_denominator,
                            int width, int height) {

  return kr_mkv_add_video_track_with_private_data (mkv, codec,
                                                   fps_numerator,
                                                   fps_denominator,
                                                   width, height, NULL, 0);
}

int kr_mkv_add_audio_track (kr_mkv_t *mkv, krad_codec_t codec,
                            int sample_rate, int channels,
                            unsigned char *priv_data,
                            int priv_data_size) {

  int t;
  unsigned char *track_info;
  unsigned char *audio_info;

  t = kr_mkv_new_tracknumber (mkv);
  if (t == -1) {
    return t;
  }

  mkv->tracks[t].sample_rate = sample_rate;
  mkv->tracks[t].codec = codec;

  kr_ebml2_start_element (mkv->e, MKV_TRACK, &track_info);
  kr_ebml2_pack_int8 (mkv->e, MKV_TRACKNUMBER, t);
  kr_ebml2_pack_int64 (mkv->e, MKV_TRACK_UID, kr_mkv_generate_track_uid (t));
  kr_ebml2_pack_string (mkv->e, MKV_CODECID, kr_codec_to_mkv_codec (codec));
  kr_ebml2_pack_int8 (mkv->e, MKV_TRACKTYPE, 2);

  kr_ebml2_start_element (mkv->e, MKV_AUDIOSETTINGS, &audio_info);
  kr_ebml2_pack_int8 (mkv->e, MKV_AUDIOCHANNELS, channels);

  kr_ebml2_pack_float (mkv->e, MKV_AUDIOSAMPLERATE, sample_rate);
  kr_ebml2_pack_int8 (mkv->e, MKV_AUDIOBITDEPTH, 16);

  kr_ebml2_finish_element (mkv->e, audio_info);

  if (priv_data_size > 0) {
    kr_ebml2_pack_data (mkv->e, MKV_CODECDATA, priv_data, priv_data_size);
  }

  kr_ebml2_finish_element (mkv->e, track_info);

  return t;
}

int kr_mkv_add_subtitle_track (kr_mkv_t *mkv, char *codec_id) {

  int t;
  unsigned char *track_info;

  t = kr_mkv_new_tracknumber (mkv);

  kr_ebml2_start_element (mkv->e, MKV_TRACK, &track_info);
  kr_ebml2_pack_int8 (mkv->e, MKV_TRACKNUMBER, t);
  kr_ebml2_pack_int64 (mkv->e, MKV_TRACK_UID, kr_mkv_generate_track_uid (t));
  kr_ebml2_pack_string (mkv->e, MKV_CODECID, codec_id);
  kr_ebml2_pack_int8 (mkv->e, MKV_TRACKTYPE, 0x11);
  kr_ebml2_finish_element (mkv->e, track_info);

  return t;
}

void kr_mkv_add_video (kr_mkv_t *mkv, int track_num, unsigned char *buffer,
                       int buffer_len, int keyframe) {

  kr_mkv_track_t *track;
  uint32_t block_length;
  unsigned char track_number;
  unsigned short block_timecode;
  unsigned char flags;
  int64_t timecode;

  flags = 0;
  block_timecode = 0;
  block_length = buffer_len + 4;
  block_length |= 0x10000000;

  track_number = track_num;
  track_number |= 0x80;

  if (keyframe) {
    flags |= 0x80;
  }

  track = &mkv->tracks[track_num];

  timecode = round (1000000000 *
                    track->total_video_frames /
                    track->fps_numerator *
                    track->fps_denominator / 1000000);

  track->total_video_frames++;

  if (keyframe) {
    kr_mkv_cluster (mkv, timecode);
  }

  /* Must be after clustering esp. in case of keyframe */
  block_timecode = timecode - mkv->cluster_timecode;  

  if (timecode > mkv->segment_timecode) {
    mkv->segment_timecode = timecode;
    mkv->segment_duration = mkv->segment_timecode;
  }

  kr_ebml2_pack_element (mkv->e, MKV_SIMPLEBLOCK);
  kr_ebml2_revpack4 (mkv->e, &block_length);
  kr_ebml2_pack (mkv->e, &track_number, 1);
  kr_ebml2_revpack2 (mkv->e, &block_timecode);
  kr_ebml2_pack (mkv->e, &flags, 1);
  kr_ebml2_pack (mkv->e, buffer, buffer_len);
}

void kr_mkv_add_audio (kr_mkv_t *mkv, int track_num, unsigned char *buffer,
                       int buffer_len, int frames) {

  kr_mkv_track_t *track;
  int64_t timecode;
  unsigned long block_length;
  unsigned char track_number;
  short block_timecode;
  unsigned char flags;

  block_timecode = 0;
  flags = 0;
  flags |= 0x80;

  block_length = buffer_len + 4;
  block_length |= 0x10000000;

  track_number = track_num;
  track_number |= 0x80;

  track = &mkv->tracks[track_num];

  if (track->codec != VORBIS) {
    timecode = round ((1000000000 *
                       track->total_audio_frames /
                       track->sample_rate /
                       1000000));
  }

  track->total_audio_frames += frames;
  track->audio_frames_since_cluster += frames;

  if (track->codec == VORBIS) {
      timecode = round ((1000000000 *
                         track->total_audio_frames /
                         track->sample_rate /
                         1000000));
  }

  if ((timecode == 0) && (mkv->track_count == 1)) {
    kr_mkv_cluster (mkv, timecode);
  }

  if ((track->audio_frames_since_cluster >= track->sample_rate) &&
      (mkv->track_count == 1)) {
    kr_mkv_cluster (mkv, timecode);
    track->audio_frames_since_cluster = 0;
  }

  block_timecode = timecode - mkv->cluster_timecode;

  if (timecode > mkv->segment_timecode) {
    mkv->segment_timecode = timecode;
    mkv->segment_duration = mkv->segment_timecode;
  }

  kr_ebml2_pack_element (mkv->e, MKV_SIMPLEBLOCK);
  kr_ebml2_revpack4 (mkv->e, &block_length);
  kr_ebml2_pack (mkv->e, &track_number, 1);
  kr_ebml2_revpack2 (mkv->e, &block_timecode);
  kr_ebml2_pack (mkv->e, &flags, 1);
  kr_ebml2_pack (mkv->e, buffer, buffer_len);
}

static void kr_mkv_cluster (kr_mkv_t *mkv, int64_t timecode) {

  if (mkv->tracks_info != NULL) {
    kr_ebml2_finish_element (mkv->e, mkv->tracks_info);
    mkv->tracks_info = NULL;
  }

  if (mkv->cluster != NULL) {
    kr_ebml2_finish_element (mkv->e, mkv->cluster);
    mkv->cluster = NULL;
  }

  mkv->cluster_timecode = timecode;

  kr_ebml2_start_element (mkv->e, MKV_CLUSTER, &mkv->cluster);
  kr_ebml2_pack_int64 (mkv->e, MKV_CLUSTER_TIMECODE, mkv->cluster_timecode);
}

kr_mkv_t *kr_mkv_create () {
  kr_mkv_t *mkv;
  mkv = calloc (1, sizeof(kr_mkv_t));
  mkv->tracks = calloc (KR_MKV_MAX_TRACKS, sizeof(kr_mkv_track_t));
  mkv->e = &mkv->ebml;
  return mkv;
}

int kr_mkv_destroy (kr_mkv_t **mkv) {
  if ((mkv != NULL) && (*mkv != NULL)) {
    free ((*mkv)->tracks);
    free (*mkv);
    return 0;
  }
  return -1;
}
