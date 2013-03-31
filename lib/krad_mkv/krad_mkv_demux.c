#include "krad_mkv_demux.h"

static int kr_mkv_read_and_check_ebml_header (kr_mkv_t *mkv);
static int kr_mkv_read_and_parse_segment_header (kr_mkv_t *mkv);
static int kr_mkv_read_and_check_header (kr_mkv_t *mkv);

static int kr_mkv_read_and_check_ebml_header (kr_mkv_t *mkv) {

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

  //printk ("Got EBML doctype: %s Version %u Read Version %u",
  //        doctype, version, read_version);

  return 0;
}

static int kr_mkv_read_and_parse_segment_header (kr_mkv_t *mkv) {



  return 0;
}

static int kr_mkv_read_and_check_header (kr_mkv_t *mkv) {

  if (!kr_mkv_read_and_check_ebml_header (mkv)) {
    return -1;
  }
  if (!kr_mkv_read_and_parse_segment_header (mkv)) {
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

  //printk ("read %zu bytes", mkv->io->len);

  if (!kr_mkv_read_and_check_header (mkv)) {
    kr_mkv_destroy (&mkv);
    return NULL;
  }

  return mkv;
}


