#include "krad_mkv_demux.h"

static int kr_mkv_check_ebml_header (kr_mkv_t *mkv);
static int kr_mkv_parse_segment_header (kr_mkv_t *mkv);
static int kr_mkv_parse_header (kr_mkv_t *mkv);
static int kr_mkv_parse_tracks (kr_mkv_t *mkv);
static int kr_mkv_parse_track (kr_mkv_t *mkv, uint64_t len);
static char *mkv_identify (uint32_t element_id);

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
      printk ("Skipping %s size %"PRIu64"",
              mkv_identify (id), size);
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
      printk ("Done with tracks");
      return 0;
    }
    if (id != MKV_TRACK) {
      printke ("No Track :/");
      return -1;
    }
    printk ("Got Track!");
    ret = kr_mkv_parse_track (mkv, size);
    if (ret < 0) {
      printke ("Track parse error...");
      return -1;
    }
  }
  return 0;
}

static int kr_mkv_parse_track (kr_mkv_t *mkv, uint64_t len) {

  int ret;
  uint32_t id;
  uint64_t size;

  printke ("Got track %d", ++mkv->track_count);

  kr_ebml2_advance (mkv->e, len);
  return 0;

  while (1) {
    ret = kr_ebml2_unpack_id (mkv->e, &id, &size);
    if (ret < 0) {
      printke ("Read error..");
      return -1;
    }
    
  }
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

  if (!kr_mkv_parse_header (mkv)) {
    kr_mkv_destroy (&mkv);
    return NULL;
  }

  return mkv;
}

static char *mkv_identify (uint32_t element_id) {

  switch (element_id) {
    case MKV_CLUSTER:
      return "Cluster";
    case MKV_SEEKHEAD:
      return "Seek Head";
    case EID_VOID:
      return "Void Space";
    case EID_CRC32:
      return "CRC32";
    case MKV_SEGMENT:
      return "Segment Header";
    case MKV_SEGMENT_INFO:
      return "Segment Info";
    case MKV_TRACKS:
      return "Segment Tracks";
    case MKV_SIMPLEBLOCK:
      return "Simple Block";
    case MKV_CLUSTER_TIMECODE:
      return "Cluster Timecode";
    case MKV_MUXINGAPP:
      return "Muxing App";
    case MKV_WRITINGAPP:
      return "Writing App";
    case MKV_CODECID:
      return "Codec";
    case MKV_VIDEOWIDTH:
      return "Width";
    case MKV_VIDEOHEIGHT:
      return "Height";
    case MKV_AUDIOCHANNELS:
      return "Channels";
    case MKV_DEFAULTDURATION:
      return "Default Duration";
    case MKV_TRACKNUMBER:
      return "Track Number";
    case MKV_AUDIOBITDEPTH:
      return "Bit Depth";
    case MKV_BLOCKGROUP:
      return "Block Group";
    case MKV_3D:
      return "3D Mode";
    default:
      return "Unknown";
  }
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

