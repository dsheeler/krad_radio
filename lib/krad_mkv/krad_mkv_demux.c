#include "krad_mkv_demux.h"
#include "krad_mkv_internal.h"

static int kr_mkv_check_ebml_header (kr_mkv_t *mkv);
static int kr_mkv_parse_segment_header (kr_mkv_t *mkv);
static int kr_mkv_parse_header (kr_mkv_t *mkv);
static int kr_mkv_parse_tracks (kr_mkv_t *mkv, uint64_t max_pos);
static int kr_mkv_parse_track (kr_mkv_t *mkv, uint64_t max_pos);
static int kr_mkv_parse_simpleblock ( kr_mkv_t *mkv,
                                      uint32_t len,
                                      uint32_t *track,
                                      uint64_t *timecode,
                                      uint8_t *flags);
static krad_codec_t kr_mkv_codec_to_kr_codec (char *codec_id);
static int kr_mkv_track_read_codec_hdr (kr_mkv_t *mkv,
                                        kr_mkv_track_t *track,
                                        uint64_t size);

static krad_codec_t kr_mkv_codec_to_kr_codec (char *codec_id) {
  if ((strlen(codec_id) == 8) && (strncmp(codec_id, "A_VORBIS", 8) == 0)) {
    return VORBIS;
  }
  if ((strlen(codec_id) == 6) && (strncmp(codec_id, "A_FLAC", 6) == 0)) {
    return FLAC;
  }
  if ((strlen(codec_id) == 6) && (strncmp(codec_id, "A_OPUS", 6) == 0)) {
    return OPUS;
  }
  if ((strlen(codec_id) == 5) && (strncmp(codec_id, "V_VP8", 5) == 0)) {
    return VP8;
  }
  if ((strlen(codec_id) == 8) && (strncmp(codec_id, "V_THEORA", 8) == 0)) {
    return THEORA;
  }
  if ((strlen(codec_id) == 6) && (strncmp(codec_id, "V_KVHS", 6) == 0)) {
    return KVHS;
  }
  return NOCODEC;
}

static int kr_mkv_check_ebml_header (kr_mkv_t *mkv) {

  char doctype[64];
  uint32_t version;
  uint32_t read_version;
  

  if (0 > kr_ebml2_unpack_header (mkv->e, doctype, 64,
                                  &version, &read_version)) {
    printke ("Could not read EBML header");
    return -1;                            
  }

  if ((strncmp(doctype, "webm", 4) != 0) &&
      (strncmp(doctype, "matroska", 8) != 0)) {
    printke ("Invalid Doctype: %s", doctype);
    return -1;      
  }
  
  if ((version < 2) || (version > 4) ||
      (read_version < 2) || (read_version > 4)) {
    printke ("Crazy Version Number: %u - %u",
              version, read_version);
    return -1;
  }

  printk ("Got EBML doctype: %s Version %u Read Version %u",
          doctype, version, read_version);

  return 0;
}

static int kr_mkv_parse_segment_info (kr_mkv_t *mkv, uint64_t max_pos) {

  int ret;
  uint32_t id;
  uint64_t size;

  while (1) {
    if (mkv->e->pos >= max_pos) {
      printk ("Got to end of segment info");
      break;
    }  
    ret = kr_ebml2_unpack_id (mkv->e, &id, &size);
    if (ret < 0) {
      printke ("Read error..");
      return -1;
    }

    switch (id) {
      case MKV_SEGMENT_TITLE:
        //kr_ebml2_unpack_uint64 (mkv->e, &mkv->timecode_scale, size);
        //break;
        printk ("Skipping TITLE: %"PRIu64" bytes",
                size);
        kr_ebml2_advance (mkv->e, size);
        break;
      case MKV_SEGMENT_TIMECODESCALE:
        kr_ebml2_unpack_uint64 (mkv->e, &mkv->timecode_scale, size);
        break;
      case MKV_SEGMENT_DURATION:
        kr_ebml2_unpack_double (mkv->e, &mkv->duration, size);
        break;
      case MKV_SEGMENT_UID:
      case MKV_SEGMENT_FILENAME:
      case MKV_SEGMENT_PREVUID:
      case MKV_SEGMENT_PREVFILENAME:
      case MKV_SEGMENT_NEXTUID:
      case MKV_SEGMENT_NEXTFILENAME:
      case MKV_SEGMENT_FAMILY:
      case MKV_SEGMENT_CHAPTERTRANSLATE:
      case MKV_SEGMENT_DATEUTC:
      case MKV_SEGMENT_MUXINGAPP:
      case MKV_SEGMENT_WRITINGAPP:
        kr_ebml2_advance (mkv->e, size);
        break;
      default:
        printk ("Skipping unknown element in segment info: %"PRIu64" bytes",
                size);
        kr_ebml2_advance (mkv->e, size);
        break;
    }
  }

  return 0;
}

static int kr_mkv_parse_segment_header (kr_mkv_t *mkv) {

  int ret;
  uint32_t id;
  uint64_t size;

  ret = kr_ebml2_unpack_id (mkv->e, &id, &size);
  if (ret < 0) {
    printke ("Read error...");
    return -1;
  }
  if (id != MKV_SEGMENT) {
    printke ("No Segment :/");
    return -1;
  }
  
  printk ("Got Segment");

  while (1) {
  
    ret = kr_ebml2_unpack_id (mkv->e, &id, &size);
    if (ret < 0) {
      printke ("Read error..");
      return -1;
    }

    switch (id) {
      case MKV_TRACKS:
        mkv->tracks_info_data = mkv->e->bufstart + mkv->e->pos;
        mkv->tracks_info_data_size = size;
        if (kr_mkv_parse_tracks (mkv, mkv->e->pos + size) < 0) {
          return -1;
        } else {
          return 0;
        }
      case MKV_SEGMENT_INFO:
        mkv->segment_info_data = mkv->e->bufstart + mkv->e->pos;
        mkv->segment_info_data_size = size;
        printk ("Got Segment Info: %"PRIu64" bytes", size);
        if (kr_mkv_parse_segment_info (mkv, mkv->e->pos + size) < 0) {
          return -1;
        }
        break;        
      default:
        printk ("Skipping unknown element: %"PRIu64" bytes", size);
        kr_ebml2_advance (mkv->e, size);
        break;
    }
  }
  return 0;
}

static int kr_mkv_parse_tracks (kr_mkv_t *mkv, uint64_t max_pos) {

  int ret;
  uint32_t id;
  uint64_t size;

  while (1) {
    if (mkv->e->pos >= max_pos) {
      printk ("Got to end of tracks info");
      break;
    }  
    ret = kr_ebml2_unpack_id (mkv->e, &id, &size);
    if (ret < 0) {
      printke ("Read error...");
      return -1;
    }
    if ((id == MKV_CLUSTER) || (id == EID_VOID) ||
        (id == EID_CRC32) || (id == MKV_TAGS) || (id == MKV_CUES) ||
        (id == MKV_ATTACHMENTS) || (id == MKV_CHAPTERS)) {
      printk ("Done with tracks. Got %d", mkv->track_count);
      return 0;
    }
    if (id != MKV_TRACK) {
      printke ("No Track :/");
      return -1;
    }
    printk ("Got Track!");
    ret = kr_mkv_parse_track (mkv, mkv->e->pos + size);
    if (ret < 0) {
      printke ("Track parse error...");
      return -1;
    }
  }
  return 0;
}

static int kr_mkv_track_read_codec_hdr (kr_mkv_t *mkv,
                                        kr_mkv_track_t *track,
                                        uint64_t size) {

  int ret;

  if (size > KRAD_MKV_CODEC_HDR_MAX_SANE_SZ) {
    printke ("Got codec data size %"PRIu64". To big!", size);
    return -1;
  }
  
  if (size == 0) {
    printke ("Got codec data size of zero!");
    return -1;
  }

  //FIXME FREE

  printk ("Got codec data size %"PRIu64"", size);

  track->codec_data_size = size;
  track->codec_data = malloc (track->codec_data_size);
  ret = kr_ebml2_unpack_data (mkv->e,
                              track->codec_data,
                              track->codec_data_size);
  if (ret != 0) {
    printke ("Got error unpacking codec data!");
    return -1;
  }

  if ((track->codec == OPUS) || (track->codec == FLAC)) {
    track->headers = 1;
    track->header_len[0] = track->codec_data_size;
    track->header[0] = track->codec_data;
  }
  
  if ((track->codec == VORBIS) || (track->codec == THEORA)) {
    
    uint8_t byte;
    uint32_t bytes_read;    
    uint32_t value;
    uint32_t maxlen;
    uint32_t current_header;
    
    current_header = 0;
    bytes_read = 0;
    value = 0;
    maxlen = MIN (10, track->codec_data_size);

    byte = track->codec_data[bytes_read];
    bytes_read += 1;

    if (byte != 2) {
      printke ("Unknown number of Xiph laced headers");
      return -1;
    }

    while ((current_header < 3) && (bytes_read < maxlen)) {
      value = 0;
      while ((current_header < 2) && (bytes_read < maxlen)) {
        byte = track->codec_data[bytes_read];
        bytes_read += 1;
        value += byte;
        if (byte != 255) {
          track->header_len[current_header] = value;
          printk ("Xiph lace value %u is %u",
                  current_header, track->header_len[current_header]);
          current_header++;
          break;
        } else {
          value += 255;
        }
      }
      if (current_header == 2) {
        track->header_len[current_header] = track->codec_data_size -
                                            (bytes_read +
                                            track->header_len[0] +
                                            track->header_len[1]);
        printk ("Xiph lace value %u is %u",
                current_header, track->header_len[current_header]);
        break;
      }
    }
    
    if ((track->header_len[0] == 0) ||
        (track->header_len[1] == 0) ||
        (track->header_len[2] == 0)) {
      printke ("Error reading Xiph lace sizes, a zero");
      return -1;
    }
    if ((track->header_len[0] + track->header_len[1] +
         track->header_len[2] + bytes_read) != track->codec_data_size) {
      printke ("Error reading Xiph lace sizes, miss-sized");
      return -1;
    }
    track->header[0] = track->codec_data + bytes_read;
    track->header[1] = track->codec_data + (bytes_read + track->header_len[0]);
    track->header[2] = track->codec_data + (bytes_read +
                                            track->header_len[0] +
                                            track->header_len[1]);
    track->headers = 3;
  }

  return 0;
}

static int kr_mkv_parse_track (kr_mkv_t *mkv, uint64_t max_pos) {

  int ret;
  uint32_t id;
  uint64_t size;
  kr_mkv_track_t track;
  uint8_t number;
  uint8_t type;
  char codec_id[32];
  float samplerate;
  
  samplerate = 0;
  number = 0;
  type = 0;
  memset (&track, 0, sizeof (kr_mkv_track_t));
  memset (codec_id, 0, sizeof (codec_id));

  while (1) {
    if (mkv->e->pos >= max_pos) {
      printk ("Got to end of track info");
      break;
    }  
    ret = kr_ebml2_unpack_id (mkv->e, &id, &size);
    if (ret < 0) {
      printke ("Read error..");
      return -1;
    }

    switch (id) {
      case MKV_AUDIO:
      case MKV_VIDEO:
        break;
      case MKV_TRACKNUMBER:
        if (number != 0) {
          printke ("Got a second track number for the same track.");
          return -1;
        }
        kr_ebml2_unpack_uint8 (mkv->e, &number, size);
        break;
      case MKV_TRACKTYPE:
        if (type != 0) {
          printke ("Got a second track type for the same track.");
          return -1;
        }
        kr_ebml2_unpack_uint8 (mkv->e, &type, size);
        break;
      case MKV_CODECID:
        kr_ebml2_unpack_string (mkv->e, codec_id, size);
        track.codec = kr_mkv_codec_to_kr_codec (codec_id);
        if (track.codec == NOCODEC) {
          printke ("Unsupported Codec: %s", codec_id);
          return -1;
        }
        printk ("Got codec: %s", codec_id);
        break;
      case MKV_CODECDATA:
        ret = kr_mkv_track_read_codec_hdr (mkv, &track, size);
        if (ret < 0) {
          printke ("Error reading codec data header");
          return -1;
        }
        break;
      case MKV_WIDTH:
        kr_ebml2_unpack_uint32 (mkv->e, &track.width, size);
        break;
      case MKV_HEIGHT:
        kr_ebml2_unpack_uint32 (mkv->e, &track.height, size);
        break;
      case MKV_CHANNELS:
        kr_ebml2_unpack_uint32 (mkv->e, &track.channels, size);
        break;
      case MKV_SAMPLERATE:
        kr_ebml2_unpack_float (mkv->e, &samplerate, size);
        track.sample_rate = samplerate;
        break;
      case MKV_BITDEPTH:
        kr_ebml2_unpack_uint32 (mkv->e, &track.bit_depth, size);
        break; 
      default:
        printk ("Skipping unknown element: %"PRIu64" bytes", size);
        kr_ebml2_advance (mkv->e, size);
        break;
    }
  }

  //FIXME temp
  track.changed = 1;

  memcpy (&mkv->tracks[number], &track, sizeof(kr_mkv_track_t));
  mkv->track_count++;

  return 0;
}

static void kr_mkv_rebuild_header_for_streaming (kr_mkv_t *mkv) {

  size_t len;
  uint8_t *segment;
  uint8_t *segment_info;
  kr_mkv_t *shdr;
  char *title;
  
  len = mkv->e->pos;
  mkv->stream_hdr = malloc (len);

  shdr = kr_mkv_create_bufsize (len);
  kr_ebml2_set_buffer (shdr->e, shdr->io->buf, shdr->io->space);

  kr_ebml2_pack_header (shdr->e, "webm", 2, 2);
  kr_ebml2_start_element (shdr->e, MKV_SEGMENT, &segment);

  title = "A Krad Restream";

  kr_ebml2_start_element (shdr->e, MKV_SEGMENT_INFO, &segment_info);
  kr_ebml2_pack_string (shdr->e, MKV_SEGMENT_TITLE, title);
  kr_ebml2_pack_int32 (shdr->e, MKV_SEGMENT_TIMECODESCALE, mkv->timecode_scale);
  kr_ebml2_pack_string (shdr->e, MKV_SEGMENT_MUXINGAPP, KRAD_MKV_VERSION);
  kr_ebml2_pack_string (shdr->e, MKV_SEGMENT_WRITINGAPP, KRAD_VERSION_STRING);
  kr_ebml2_finish_element (shdr->e, segment_info);

  kr_ebml2_pack_data (shdr->e, MKV_TRACKS,
                      mkv->tracks_info_data, mkv->tracks_info_data_size);
    
  mkv->stream_hdr_len = shdr->e->pos;
  memcpy (mkv->stream_hdr, shdr->e->bufstart, mkv->stream_hdr_len);

  kr_mkv_destroy (&shdr);
}

static int kr_mkv_parse_header (kr_mkv_t *mkv) {
  if (kr_mkv_check_ebml_header (mkv) < 0) {
    return -1;
  }
  if (kr_mkv_parse_segment_header (mkv) < 0) {
    return -1;
  }

  kr_mkv_rebuild_header_for_streaming (mkv);
  
  return 0;
}

static int kr_mkv_parse_simpleblock ( kr_mkv_t *mkv,
                                       uint32_t len,
                                       uint32_t *track,
                                       uint64_t *timecode,
                                       uint8_t *flags) {

  int16_t block_timecode;
  uint8_t flags_tmp;
  uint8_t byte;

  block_timecode = 0;

  kr_ebml2_unpack_data ( mkv->e, &byte, 1 );
  if (track != NULL) {
    *track = (byte - 0x80);
  }

  kr_ebml2_unpack_int16 (mkv->e, &block_timecode, 2);
  mkv->current_timecode = mkv->cluster_timecode + block_timecode;
  //printk ("Timecode is: %6.3f\n",
  //        ((mkv->cluster_timecode +
  //        (int64_t)block_timecode)/1000.0));
  if (timecode != NULL) {
    *timecode = mkv->current_timecode;
  }
  
  kr_ebml2_unpack_data ( mkv->e, &flags_tmp, 1 );

  if (flags != NULL) {
    *flags = flags_tmp;
  }

  return len - 4;
}

kr_mkv_t *kr_mkv_open_stream (char *host, int port, char *mount) {
  //FIXME
  return NULL;
}

kr_mkv_t *kr_mkv_open_file (char *filename) {
  
  kr_mkv_t *mkv;
  kr_file_t *file;

  file = kr_file_open (filename);  

  if (file == NULL) {
    return NULL;
  }
  
  mkv = kr_mkv_create_bufsize (file->size);
  mkv->file = file;
  mkv->fd = file->fd;
  kr_io2_set_fd (mkv->io, mkv->fd);

  kr_io2_read (mkv->io);
  kr_ebml2_set_buffer ( mkv->e, mkv->io->rd_buf, mkv->io->len );

  printk ("read %zu bytes", mkv->io->len);

  if (kr_mkv_parse_header (mkv) < 0) {
    kr_mkv_destroy (&mkv);
    return NULL;
  }

  return mkv;
}

int kr_mkv_track_count (kr_mkv_t *mkv) {
  return mkv->track_count;
}

krad_codec_t kr_mkv_track_codec (kr_mkv_t *mkv, uint32_t track) {
  return mkv->tracks[track].codec;
}

int kr_mkv_track_header_count (kr_mkv_t *mkv, uint32_t track) {
  return mkv->tracks[track].headers;
}

int kr_mkv_track_header_size (kr_mkv_t *mkv, uint32_t track, uint32_t header) {
  return mkv->tracks[track].header_len[header];
}

int kr_mkv_read_track_header (kr_mkv_t *mkv, uint8_t *buffer,
                              uint32_t track, uint32_t header) {
  if (mkv->tracks[track].codec_data_size) {
    memcpy (buffer,
            mkv->tracks[track].header[header],
            mkv->tracks[track].header_len[header]);
    return mkv->tracks[track].header_len[header];
  }
  return 0;
}

int kr_mkv_track_active (kr_mkv_t *mkv, uint32_t track) {
  //FIXME F'UP
  if ((mkv->tracks[track].codec != NOCODEC) &&
      (mkv->tracks[track].codec != KVHS)) {
    return 1;
  }
  return 0;
}

int kr_mkv_track_changed (kr_mkv_t *mkv, uint32_t track) {
  if (mkv->tracks[track].changed == 1) {
    mkv->tracks[track].changed = 0;
    return 1;
  }
  return 0;
}

int kr_mkv_read_packet (kr_mkv_t *mkv, uint32_t *track,
                        uint64_t *timecode, uint8_t *flags, uint8_t *buffer) {

  int ret;
  uint32_t id;
  uint64_t size;

  //printk ("kr_mkv_read_packet");

  while (1) {
    ret = kr_ebml2_unpack_id (mkv->e, &id, &size);
    if (ret < 0) {
      printke ("Read error..");
      return -1;
    }

    switch (id) {
      case MKV_CLUSTER:
        break;
      case EID_VOID:
      case EID_CRC32:
      case MKV_TAGS:
      case MKV_CUES:
      case MKV_ATTACHMENTS:
      case MKV_CHAPTERS:
        kr_ebml2_advance (mkv->e, size);
        break;
      case MKV_CLUSTER_TIMECODE:
        kr_ebml2_unpack_uint64 (mkv->e, &mkv->cluster_timecode, size);
        break;
      case MKV_SIMPLEBLOCK:
        kr_mkv_parse_simpleblock (mkv, size, track, timecode, flags);
        kr_ebml2_unpack_data ( mkv->e, buffer, size - 4 );
        return size - 4;
      default:
        printke ("Err! Unknown element: %"PRIu64" bytes", size);
        return -1;
        break;
    }
  }
  return 0;      
}
