#include "krad_mkv_demux.h"

static int kr_mkv_check_ebml_header (kr_mkv_t *mkv);
static int kr_mkv_parse_segment_header (kr_mkv_t *mkv);
static int kr_mkv_parse_header (kr_mkv_t *mkv);
static int kr_mkv_parse_tracks (kr_mkv_t *mkv);
static int kr_mkv_parse_track (kr_mkv_t *mkv, uint64_t max_pos);
static int kr_mkv_read_simpleblock ( kr_mkv_t *mkv,
                                     int len,
                                     int *track,
                                     uint64_t *timecode,
                                     unsigned char *buffer);
static krad_codec_t kr_mkv_codec_to_kr_codec (char *codec_id);


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
  if ((strlen(codec_id) == 6) && (strncmp(codec_id, "V_KVHS", 6) == 0)) {
    return KVHS;
  }
  if ((strlen(codec_id) == 8) && (strncmp(codec_id, "V_THEORA", 8) == 0)) {
    return THEORA;
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
    if (id != MKV_TRACKS) {
      printk ("Skipping unknown element: %"PRIu64" bytes", size);
      kr_ebml2_advance (mkv->e, size);
      continue;
    } else {
      printk ("Got Tracks!");
      break;
    }  
  }
  return 0;
}

static int kr_mkv_parse_tracks (kr_mkv_t *mkv) {

  int ret;
  uint32_t id;
  uint64_t size;

  while (1) {
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
  memset (&track, 0, sizeof (kr_mkv_track_t));
  memset (codec_id, 0, sizeof (codec_id));

  while (1) {
    if (mkv->e->pos >= max_pos) {
      printk ("Got to end of this tracks info");
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
        printk ("Got codec: %s", krad_codec_to_string (track.codec));
        break;
      case MKV_CODECDATA:
        printk ("Got codec data size %"PRIu64"", size);
        kr_ebml2_advance (mkv->e, size);
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
  if (track.codec == VP8) {
    track.changed = 1;
  }
  memcpy (&mkv->tracks[number], &track, sizeof(kr_mkv_track_t));
  mkv->track_count++;

  return 0;
}

static int kr_mkv_parse_header (kr_mkv_t *mkv) {
  if (kr_mkv_check_ebml_header (mkv) < 0) {
    return -1;
  }
  if (kr_mkv_parse_segment_header (mkv) < 0) {
    return -1;
  }
  if (kr_mkv_parse_tracks (mkv) < 0) {
    return -1;
  }
  return 0;
}

kr_mkv_t *kr_mkv_open_file (char *filename) {
  
  kr_mkv_t *mkv;
  int flags;
  int fd;
  
  flags = O_RDONLY;
  
  if (!file_exists(filename)) {
    return NULL;
  }
  
  fd = open ( filename, flags );
  
  if (fd < 0) {
    return NULL;
  }
  
  mkv = kr_mkv_create_bufsize (50000000);
  mkv->fd = fd;  
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

int kr_mkv_track_count (kr_mkv_t *kr_mkv) {
  return kr_mkv->track_count;
}

krad_codec_t kr_mkv_track_codec (kr_mkv_t *kr_mkv, int track) {
  return kr_mkv->tracks[track].codec;
}

int kr_mkv_track_header_count (kr_mkv_t *kr_mkv, int track) {
  return kr_mkv->tracks[track].headers;
}

int kr_mkv_track_header_size (kr_mkv_t *kr_mkv, int track, int header) {
  return kr_mkv->tracks[track].header_len[header];
}

int kr_mkv_read_track_header (kr_mkv_t *kr_mkv, unsigned char *buffer,
                              int track, int header) {
  if (kr_mkv->tracks[track].codec_data_size) {
    memcpy (buffer,
            kr_mkv->tracks[track].header[header],
            kr_mkv->tracks[track].header_len[header]);
    return kr_mkv->tracks[track].header_len[header];
  }
  return 0;
}

int kr_mkv_track_active (kr_mkv_t *kr_mkv, int track) {
  //FIXME temp
  if (kr_mkv->tracks[track].codec == VP8) {
    return 1;
  } else {
    return 0;
  }
  //ENDFIXME

  if (kr_mkv->tracks[track].codec != NOCODEC) {
    return 1;
  }
  return 0;
}

int kr_mkv_track_changed (kr_mkv_t *kr_mkv, int track) {
  if (kr_mkv->tracks[track].changed == 1) {
    kr_mkv->tracks[track].changed = 0;
    return 1;
  }
  return 0;
}

int kr_mkv_read_packet (kr_mkv_t *mkv, int *track,
                        uint64_t *timecode, unsigned char *buffer) {

  int ret;
  uint32_t id;
  uint64_t size;

  printk ("kr_mkv_read_packet");

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
        return kr_mkv_read_simpleblock (mkv, size, track, timecode, buffer);
      default:
        printke ("Err! Unknown element: %"PRIu64" bytes", size);
        return -1;
        break;
    }
  }

  return 0;      
}

static int kr_mkv_read_simpleblock ( kr_mkv_t *mkv,
                                     int len,
                                     int *track,
                                     uint64_t *timecode,
                                     unsigned char *buffer) {

  int16_t block_timecode;
  unsigned char flags;
  unsigned char byte;

  block_timecode = 0;

  kr_ebml2_unpack_data ( mkv->e, &byte, 1 );
  if (track != NULL) {
    *track = (byte - 0x80);
  }
  printk ("tracknum is %d", *track);

  kr_ebml2_unpack_int16 (mkv->e, &block_timecode, 2);
  mkv->current_timecode = mkv->cluster_timecode + block_timecode;
  printk ("Timecode is: %6.3f\n",
          ((mkv->cluster_timecode +
          (int64_t)block_timecode)/1000.0));
  if (timecode != NULL) {
    *timecode = mkv->current_timecode;
  }
  
  kr_ebml2_unpack_data ( mkv->e, &flags, 1 );
  kr_ebml2_unpack_data ( mkv->e, buffer, len - 4 );
  return len - 4;
}
