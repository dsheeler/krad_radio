#include "krad_mkv.h"
#include "krad_mkv_internal.h"

static void kr_mkv_start_segment (kr_mkv_t *mkv, char *title);
static char *kr_codec_to_mkv_codec (krad_codec_t codec);
static void kr_mkv_cluster (kr_mkv_t *mkv, int64_t timecode);
static int kr_mkv_new_tracknumber (kr_mkv_t *mkv);
static int kr_mkv_generate_track_uid (int track_number);

static int kr_mkv_sync (kr_mkv_t *mkv, int splicepoint);

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

static int kr_mkv_sync (kr_mkv_t *mkv, int splicepoint) {

  //FIXME temp
  
  if ((mkv->stream_hdr_len == 0) && (mkv->e->pos > 0)) {
    mkv->stream_hdr_len = mkv->e->pos;
    mkv->stream_hdr = malloc (mkv->stream_hdr_len);
    memcpy (mkv->stream_hdr, mkv->io->buf, mkv->stream_hdr_len);
    if (mkv->transmission != NULL) {
      krad_transmitter_transmission_set_header (mkv->transmission,
                                                mkv->stream_hdr,
                                                mkv->stream_hdr_len);
      kr_io2_advance (mkv->io, mkv->e->pos); 
      kr_io2_restart (mkv->io);  
      kr_ebml2_set_buffer (mkv->e, mkv->io->buf, mkv->io->space);
      return 0;
    }
    if (mkv->io_cb_ptr != NULL) {
      mkv->io_callback (mkv->stream_hdr, mkv->stream_hdr_len, splicepoint, mkv->io_cb_ptr);
      kr_io2_advance (mkv->io, mkv->e->pos); 
      kr_io2_restart (mkv->io);
      kr_ebml2_set_buffer (mkv->e, mkv->io->buf, mkv->io->space);
      return 0;
    }
  }
  
  kr_io2_advance (mkv->io, mkv->e->pos);
  if (mkv->io_cb_ptr != NULL) {
    mkv->io_callback (mkv->io->buffer, mkv->io->len, splicepoint, mkv->io_cb_ptr);
    kr_io2_restart (mkv->io);  
  } else {
    if (mkv->transmission != NULL) {
      krad_transmitter_transmission_add_data_opt (mkv->transmission,
                                                  mkv->io->buffer,
                                                  mkv->io->len,
                                                  splicepoint);
      kr_io2_restart (mkv->io);
    } else {
      if (mkv->stream != NULL) {
 
        ssize_t ret;
        ssize_t sent;
        ssize_t bytes;
        uint8_t *buffer;

        sent = 0;
        buffer = mkv->io->buffer;
        bytes = mkv->io->len;
          
        while (sent != bytes) {
          kr_stream_i_am_a_blocking_subscripter (mkv->stream);
          ret = kr_stream_send (mkv->stream, buffer + sent, bytes - sent);
          if (ret > 0) {
            sent += ret;
          }
          if (sent != bytes) {
            //printf ("\nSent to few bytes: ret %zd - sent %zd - total %zd\n",
            //        ret, sent, bytes);
            //fflush (stdout);
          }
          if (mkv->stream->connected == 0) {
            printke ("Failed!: %s\n", mkv->stream->error_str);
            break;
          }
        }
        if (mkv->stream->connected == 1) {
          kr_io2_restart (mkv->io);
        }
      } else {    
        kr_io2_sync (mkv->io);
      }
    }
  }
  kr_ebml2_set_buffer (mkv->e, mkv->io->buf, mkv->io->space);
  
  return 0;
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

  kr_mkv_sync (mkv, 1);

  mkv->cluster_timecode = timecode;

  kr_ebml2_start_element (mkv->e, MKV_CLUSTER, &mkv->cluster);
  kr_ebml2_pack_int64 (mkv->e, MKV_CLUSTER_TIMECODE, mkv->cluster_timecode);
}

static void kr_mkv_start_segment (kr_mkv_t *mkv, char *title) {

  uint8_t *segment_info;
  
  if (title == NULL) {
    title = "A Krad Radio Creation";
  }

  mkv->timecode_scale = 1000000;

  //FIXME
  //make sure title is nullterm and not insane len

  //FIXME
  // temp place for this
  kr_ebml2_pack_header ( mkv->e, "webm", 2, 2);

  kr_ebml2_start_element (mkv->e, MKV_SEGMENT, &mkv->segment);

  kr_ebml2_start_element (mkv->e, MKV_SEGMENT_INFO, &segment_info);
  kr_ebml2_pack_string (mkv->e, MKV_SEGMENT_TITLE, title);
  kr_ebml2_pack_uint64 (mkv->e, MKV_SEGMENT_TIMECODESCALE, mkv->timecode_scale);
  kr_ebml2_pack_string (mkv->e, MKV_SEGMENT_MUXINGAPP, KRAD_MKV_VERSION);
  kr_ebml2_pack_string (mkv->e, MKV_SEGMENT_WRITINGAPP, KRAD_VERSION_STRING);
  kr_ebml2_finish_element (mkv->e, segment_info);

  mkv->current_track = 1;

  kr_ebml2_start_element (mkv->e, MKV_TRACKS, &mkv->tracks_info);
}

int kr_mkv_add_video_track_with_private_data (kr_mkv_t *mkv,
                                              krad_codec_t codec,
                                              int fps_numerator,
                                              int fps_denominator,
                                              int width, int height,
                                              uint8_t *priv_data,
                                              int priv_data_size) {
  int t;
  uint8_t *track_info;
  uint8_t *video_info;

  t = kr_mkv_new_tracknumber (mkv);
  if (t == -1) {
    return t;
  }

  mkv->video_tracks++;
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

  kr_ebml2_start_element (mkv->e, MKV_VIDEO, &video_info);
  kr_ebml2_pack_int16 (mkv->e, MKV_WIDTH, mkv->tracks[t].width);
  kr_ebml2_pack_int16 (mkv->e, MKV_HEIGHT, mkv->tracks[t].height);
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
                            uint32_t sample_rate, uint8_t channels,
                            uint8_t *priv_data,
                            int priv_data_size) {

  int t;
  uint8_t *track_info;
  uint8_t *audio_info;

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

  kr_ebml2_start_element (mkv->e, MKV_AUDIO, &audio_info);
  kr_ebml2_pack_uint8 (mkv->e, MKV_CHANNELS, channels);
  kr_ebml2_pack_float (mkv->e, MKV_SAMPLERATE, sample_rate);
  kr_ebml2_pack_uint8 (mkv->e, MKV_BITDEPTH, 16);
  kr_ebml2_finish_element (mkv->e, audio_info);

  if (priv_data_size > 0) {
    kr_ebml2_pack_data (mkv->e, MKV_CODECDATA, priv_data, priv_data_size);
  }

  kr_ebml2_finish_element (mkv->e, track_info);

  return t;
}

int kr_mkv_add_subtitle_track (kr_mkv_t *mkv, char *codec_id) {

  int t;
  uint8_t *track_info;

  t = kr_mkv_new_tracknumber (mkv);

  kr_ebml2_start_element (mkv->e, MKV_TRACK, &track_info);
  kr_ebml2_pack_int8 (mkv->e, MKV_TRACKNUMBER, t);
  kr_ebml2_pack_int64 (mkv->e, MKV_TRACK_UID, kr_mkv_generate_track_uid (t));
  kr_ebml2_pack_string (mkv->e, MKV_CODECID, codec_id);
  kr_ebml2_pack_int8 (mkv->e, MKV_TRACKTYPE, 0x11);
  kr_ebml2_finish_element (mkv->e, track_info);

  return t;
}

void kr_mkv_add_video_tc (kr_mkv_t *mkv, int track_num, uint8_t *buffer,
                          int len, int keyframe, int64_t tc) {

  kr_mkv_track_t *track;
  uint32_t block_length;
  unsigned char track_number;
  unsigned short block_timecode;
  unsigned char flags;
  int64_t timecode;

  flags = 0;
  timecode = 0;
  block_timecode = 0;
  block_length = len + 4;
  block_length |= 0x10000000;
  track_number = track_num;
  track_number |= 0x80;

  if (keyframe) {
    flags |= 0x80;
  }

  track = &mkv->tracks[track_num];

  if (track->total_video_frames == 0) {
    if (keyframe == 0) {
      return;
    } else {
      mkv->seen_keyframe = 1;
    }
  }

  if (tc > -1) {
    timecode = tc;
  } else {
    timecode = round (1000000000 *
                      track->total_video_frames /
                      track->fps_numerator *
                      track->fps_denominator / mkv->timecode_scale);
  }

  if (keyframe) {
    if (mkv->audio_init_cluster == 1) {
      mkv->audio_init_cluster = 0;
    } else {
      kr_mkv_cluster (mkv, timecode);
    }
  }

  track->total_video_frames++;

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
  kr_ebml2_pack (mkv->e, buffer, len);
    
  kr_mkv_sync (mkv, 0);
}

void kr_mkv_add_video (kr_mkv_t *mkv, int track_num, uint8_t *buffer,
                       int len, int keyframe) {
  kr_mkv_add_video_tc (mkv, track_num, buffer, len, keyframe, -1);
}

void kr_mkv_add_audio (kr_mkv_t *mkv, int track_num, uint8_t *buffer,
                       int buffer_len, int frames) {

  kr_mkv_track_t *track;
  int64_t timecode;
  unsigned long block_length;
  unsigned char track_number;
  short block_timecode;
  unsigned char flags;

  timecode = 0;
  block_timecode = 0;
  flags = 0;
  flags |= 0x80;

  block_length = buffer_len + 4;
  block_length |= 0x10000000;

  track_number = track_num;
  track_number |= 0x80;

  track = &mkv->tracks[track_num];

  track->total_audio_frames += frames;
  track->audio_frames_since_cluster += frames;

  if ((mkv->video_tracks > 0) && (mkv->seen_keyframe == 0) &&
      (track->total_audio_frames == 0) && (mkv->segment_timecode == 0)) {
    mkv->audio_init_cluster = 1;
    kr_mkv_cluster (mkv, timecode);
  }
  
  timecode = round ((1000000000 *
                     track->total_audio_frames /
                     track->sample_rate / mkv->timecode_scale));

  if (((track->audio_frames_since_cluster >= track->sample_rate) ||
       (track->total_audio_frames == 0)) &&
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
  
  kr_mkv_sync (mkv, 0);
}


kr_mkv_t *kr_mkv_create_file (char *filename) {
  
  kr_mkv_t *mkv;
  kr_file_t *file;

  file = kr_file_create (filename);  

  if (file == NULL) {
    return NULL;
  }
  
  mkv = kr_mkv_create ();
  mkv->file = file;
  mkv->fd = file->fd;
  kr_ebml2_set_buffer ( mkv->e, mkv->io->buf, mkv->io->space );
  kr_io2_set_fd (mkv->io, mkv->fd);

  kr_mkv_start_segment (mkv, "A Krad Radio File");

  return mkv;
}

kr_mkv_t *kr_mkv_create_stream (char *host, int port,
                                char *mount, char *password) {
  
  kr_mkv_t *mkv;
  krad_stream_t *stream;
  
  /* Temp */
  char *content_type = "video/webm";
  
  stream = kr_stream_create (host, port, mount, content_type, password);
  
  if (stream == NULL) {
    return NULL;
  }
  
  while (stream->ready != 1) {
    kr_stream_handle_headers (stream);
  }

  //printk ("Krad MKV: %s %d %s %s - SD %s",
  //        host, port, mount, password, stream->sd);
  //printk ("Krad MKV: SD %d", stream->sd);
  
  mkv = kr_mkv_create ();
  mkv->stream = stream;
  kr_ebml2_set_buffer ( mkv->e, mkv->io->buf, mkv->io->space );
  mkv->fd = stream->sd;  
  kr_io2_set_fd (mkv->io, mkv->fd);

  kr_mkv_start_segment (mkv, "A Krad Radio Stream");

  return mkv;
}

kr_mkv_t *kr_mkv_create_bufsize (size_t size) {
  kr_mkv_t *mkv;
  mkv = calloc (1, sizeof(kr_mkv_t));
  mkv->tracks = calloc (KR_MKV_MAX_TRACKS, sizeof(kr_mkv_track_t));
  mkv->io = kr_io2_create_size (size);
  mkv->e = &mkv->ebml;
  return mkv;
}

kr_mkv_t *kr_mkv_create () {
  return kr_mkv_create_bufsize (2000000);
}

kr_mkv_t *kr_mkv_create_transmission (krad_transmitter_t *transmitter,
                                      char *mount,
                                      char *content_type) {

  
  kr_mkv_t *mkv;
  krad_transmission_t *transmission;

  if ((transmitter == NULL) || (mount == NULL) || (content_type == NULL)) {
    return NULL;
  }

  transmission = kr_transmission_create (transmitter, mount, content_type);

  if (transmission == NULL) {
    return NULL;
  }
  
  mkv = kr_mkv_create ();
  mkv->transmission = transmission;
  kr_ebml2_set_buffer ( mkv->e, mkv->io->buf, mkv->io->space );

  kr_mkv_start_segment (mkv, "A Krad Radio Stream");

  return mkv;
}

kr_mkv_t *kr_mkv_create_io_callback (mkv_io_callback cb, void *ptr) {

  kr_mkv_t *mkv;

  if ((cb == NULL) || (ptr == NULL)) {
    return NULL;
  }
  
  mkv = kr_mkv_create ();
  mkv->io_callback = cb;
  mkv->io_cb_ptr = ptr;
  kr_ebml2_set_buffer ( mkv->e, mkv->io->buf, mkv->io->space );

  kr_mkv_start_segment (mkv, "A Krad Radio Stream");

  return mkv;
}

int kr_mkv_destroy (kr_mkv_t **mkv) {
  if ((mkv != NULL) && (*mkv != NULL)) {
    kr_io2_destroy (&(*mkv)->io);
    free ((*mkv)->tracks);
    if ((*mkv)->stream_hdr_len > 0) {
      free ((*mkv)->stream_hdr);
    }
    if ((*mkv)->file != NULL) {
      kr_file_close (&(*mkv)->file);
    }
    if ((*mkv)->stream != NULL) {
      kr_stream_destroy (&(*mkv)->stream);
    }
    if ((*mkv)->transmission != NULL) {
      kr_transmission_destroy ((*mkv)->transmission);
      (*mkv)->transmission = NULL;
    }
    free (*mkv);
    *mkv = NULL;
    return 0;
  }
  return -1;
}
