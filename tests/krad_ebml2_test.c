
/*
126
16,382
2,097,150
268,435,454
34,359,738,366
*/

#define EBML_ID_CLUSTER 0x1F43B675
#define EBML_ID_CLUSTER          0x1F43B675
#define EBML_ID_TRACK_UID        0x73C5
#define EBML_ID_TRACK_TYPE        0x83
#define EBML_ID_LANGUAGE        0x22b59C
#define EBML_ID_SEGMENT          0x18538067
#define EBML_ID_SEGMENT_TITLE      0x7BA9
#define EBML_ID_SEGMENT_INFO      0x1549A966
#define EBML_ID_SEGMENT_TRACKS      0x1654AE6B
#define EBML_ID_TRACK          0xAE
#define EBML_ID_CODECDATA        0x63A2
#define EBML_ID_CLUSTER_TIMECODE    0xE7
#define EBML_ID_SIMPLEBLOCK        0xA3
#define EBML_ID_BLOCKGROUP        0xA0
#define EBML_ID_DOCTYPE          0x4282
#define EBML_ID_MUXINGAPP         0x4D80
#define EBML_ID_WRITINGAPP         0x5741


#define EBML_ID_TAGS          0x1254C367
#define EBML_ID_TAG            0x7373
#define EBML_ID_TAG_TARGETS        0x63C0
#define EBML_ID_TAG_TARGETTYPEVALUE    0x68CA
#define EBML_ID_TAG_TARGETTYPE      0x63CA
#define EBML_ID_TAG_SIMPLE        0x67C8
#define EBML_ID_TAG_NAME        0x45A3
#define EBML_ID_TAG_STRING        0x4487
#define EBML_ID_TAG_BINARY        0x4485


void kr_print_ebml (unsigned char *buffer, int len) {

  int i;

  i = 0;

  printf ("Raw EBML: \n");
  for (i = 0; i < len; i++) {
    printf ("%02X", buffer[i]);
  }
  printf ("\nEnd Raw EBML\n");
}

void test_kr_ebml2 (kr_ebml2_t *ebml) {

  unsigned char *my_element;


  kr_ebml2_pack_header (ebml, "testy", 2, 2);

  kr_ebml2_pack_element (ebml, EBML_ID_CLUSTER);
  kr_ebml2_pack_element (ebml, EBML_ID_TRACK_UID);
  kr_ebml2_pack_element (ebml, EBML_ID_TRACK);

  kr_ebml2_start_element (ebml, EBML_ID_TAGS, &my_element);

  kr_ebml2_pack_element (ebml, EBML_ID_CODECDATA);
  kr_ebml2_pack_element (ebml, EBML_ID_CLUSTER_TIMECODE);
  kr_ebml2_pack_element (ebml, EBML_ID_SEGMENT_INFO);
  kr_ebml2_pack_element (ebml, EBML_ID_MUXINGAPP);


  kr_ebml2_pack_int32 (ebml, EBML_ID_TAG, 666);
  kr_ebml2_pack_int64 (ebml, EBML_ID_TAG_SIMPLE, 44444444444444);
  kr_ebml2_pack_string (ebml, EBML_ID_TAG_STRING, "When I was a young girl!");

  kr_ebml2_finish_element (ebml, my_element);

  //kr_print_ebml (ebml->buffer, ebml->pos);

}

int main (int argc, char *argv[]) {

  int count;

  count = 0;

  kr_ebml2_t *ebml;
  ebml = kr_ebml2_create ();

  while (count < 1000000000) {
    test_kr_ebml2 (ebml);
    count++;
    ebml->pos = 0;
  }

  kr_ebml2_destroy (&ebml);

  return 0;
}




//////////



#include "krad_ebml2w.h"

void kr_print_ebml (unsigned char *buffer, int len) {

  int i;

  i = 0;

  printf ("Raw EBML: \n");
  for (i = 0; i < len; i++) {
    printf ("%02X", buffer[i]);
  }
  printf ("\nEnd Raw EBML\n");
}

int test_kr_ebml2 (char *filename) {

  kr_ebml2_t *ebml;
  //unsigned char *my_element;
  int fd;
  int ret;

  fd = 0;

  if (!filename) {
    fprintf (stderr, "Specify a file!\n");
    return 1;
  }

  fd = open ( filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
  if (fd < 0) {
    fprintf (stderr, "Cant open %s!\n", filename);
    return 1;
  }

  ebml = kr_ebml2_create ();

  kr_ebml2_pack_header (ebml, "testy", 2, 2);
  /*
  kr_ebml2_pack_element (ebml, EID_CLUSTER);
  kr_ebml2_pack_element (ebml, EID_TRACK_UID);
  kr_ebml2_pack_element (ebml, EID_TRACK);

  kr_ebml2_start_element (ebml, EID_TAGS, &my_element);

  kr_ebml2_pack_element (ebml, EID_CODECDATA);
  kr_ebml2_pack_element (ebml, EID_CLUSTER_TIMECODE);
  kr_ebml2_pack_element (ebml, EID_SEGMENT_INFO);
  kr_ebml2_pack_element (ebml, EID_MUXINGAPP);


  kr_ebml2_pack_int32 (ebml, EID_TAG, 666);
  kr_ebml2_pack_int64 (ebml, EID_TAG_SIMPLE, 44444444444444);
  kr_ebml2_pack_string (ebml, EID_TAG_STRING, "When I was a young girl!");

  kr_ebml2_finish_element (ebml, my_element);
  */
  ret = write (fd, ebml->buffer, ebml->pos);
  close (fd);

  kr_print_ebml (ebml->buffer, ebml->pos);
  kr_ebml2_destroy (&ebml);

  if (ret < 0) {
    fprintf (stderr, "Write failed!\n");
    return 1;
  }

  return 0;
}



  int fd;
  int ret;
  int err;
  int clusters;
  kr_io2_t *io;
  kr_ebml2_t *ebml;

  uint32_t element;
  uint64_t data_size;

  clusters = 0;
  element = 0;
  data_size = 0;

  fd = 0;

  if (argc < 2) {
    fprintf (stderr, "Specify a file!\n");
    return 1;
  }

  fd = open ( argv[1], O_RDONLY );
  if (fd < 0) {
    return 1;
  }

  ebml = kr_ebml2_create ();
  io = kr_io2_create ();
  kr_ebml2_set_buffer ( ebml, io->buf, io->space );

  ret = read (fd, ebml->buf, KRAD_EBML2_BUF_SZ);
  if (ret < 0) {
    close (fd);
    kr_ebml2_destroy (&ebml);
    return 1;
  }

  printf ("read %d bytes\n", ret);

  char doctype[32];
  uint32_t version;
  uint32_t read_version;

  err = kr_ebml2_unpack_header (ebml, doctype, sizeof (doctype), &version, &read_version);

  if (err < 0) {
    fprintf (stderr, "error reading ebml header\n");
  } else {
    printf ("Got ebml doctype: %s\n", doctype);
  }

/*
  err = kr_ebml2_unpack_id (ebml, &element, &data_size);
  if (!err) {
    while (ebml->pos < ret) {

      err = kr_ebml2_unpack_id (ebml, &element, &data_size);
      if (err == -1) {
        printf ("parsing error!\n");
        break;
      }
      if (element != EID_SEGMENT) {
        kr_ebml2_advance (ebml, data_size);
      }
      if (element == EID_CLUSTER) {
        clusters++;
      }
    }
  }

  if (clusters > 0) {
    printf ("Found %d clusters!\n", clusters);
  }
  */
  kr_ebml2_destroy (&ebml);


  //return test_kr_ebml2 (argv[1]);
