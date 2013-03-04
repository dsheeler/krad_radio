#include "krad_ebml2.h"

inline void rmemcpy2 (unsigned char *dst, unsigned char *src) {
  dst[0] = src[1];
  dst[1] = src[0];
}

inline void rmemcpy3 (unsigned char *dst, unsigned char *src) {
  dst[0] = src[2];
  dst[1] = src[1];
  dst[2] = src[0];
}

inline void rmemcpy4 (unsigned char *dst, unsigned char *src) {
  dst[0] = src[3];
  dst[1] = src[2];
  dst[2] = src[1];
  dst[3] = src[0];
}

inline void rmemcpy5 (unsigned char *dst, unsigned char *src) {
  dst[0] = src[4];
  dst[1] = src[3];
  dst[2] = src[2];
  dst[3] = src[1];
  dst[4] = src[0];
}

inline void rmemcpy6 (unsigned char *dst, unsigned char *src) {
  dst[0] = src[5];
  dst[1] = src[4];
  dst[2] = src[3];
  dst[3] = src[2];
  dst[4] = src[1];
  dst[5] = src[0];
}

inline void rmemcpy7 (unsigned char *dst, unsigned char *src) {
  dst[0] = src[6];
  dst[1] = src[5];
  dst[2] = src[4];
  dst[3] = src[3];
  dst[4] = src[2];
  dst[5] = src[1];
  dst[6] = src[0];
}

inline void rmemcpy8 (unsigned char *dst, unsigned char *src) {
  dst[0] = src[7];
  dst[1] = src[6];
  dst[2] = src[5];
  dst[3] = src[4];
  dst[4] = src[3];
  dst[5] = src[2];
  dst[6] = src[1];
  dst[7] = src[0];
}

inline void kr_ebml2_write_element (kr_ebml2_t *ebml, uint32_t element) {
  if (element < 0x00000100) {
    ebml->buf[0] = ((unsigned char *)&element)[0];
    kr_ebml2_advance (ebml, 1);
  } else {
    if (element < 0x00010000) {
      rmemcpy2 (ebml->buf, (unsigned char *)&element);
      kr_ebml2_advance (ebml, 2);
    } else {
      if (element < 0x01000000) {
        rmemcpy3 (ebml->buf, (unsigned char *)&element);
        kr_ebml2_advance (ebml, 3);
      } else {
        rmemcpy4 (ebml->buf, (unsigned char *)&element);
        kr_ebml2_advance (ebml, 4);
      }
    }
  }
}

/*
void kr_ebml2l2_start_element ( kr_ebml2_t * kr_ebml2, uint32_t element, uint64_t *position) {

  kr_ebml2l2_write_element ( kr_ebml2, element);
  *position = kr_ebml2l2_tell( kr_ebml2);
  kr_ebml2l2_write_data_size ( kr_ebml2, EBML_DATA_SIZE_UNKNOWN);
  
}

void kr_ebml2l2_finish_element ( kr_ebml2_t * kr_ebml2, uint64_t element_position) {

  uint64_t current_position;
  uint64_t element_data_size;

  current_position = kr_ebml2l2_tell( kr_ebml2);
  element_data_size = current_position - element_position - EBML_DATA_SIZE_UNKNOWN_LENGTH;
  
  if ((current_position - element_position) <= kr_ebml2l2->io_adapter.write_buffer_pos) {

    kr_ebml2l2_seek( kr_ebml2, element_position, SEEK_SET);
    kr_ebml2l2_write_data_size_update ( kr_ebml2, element_data_size);
    kr_ebml2l2_seek( kr_ebml2, current_position, SEEK_SET);
  
  }
}

void kr_ebml2l2_write_data_size_update ( kr_ebml2_t * kr_ebml2, uint64_t data_size) {
  data_size |= (0x000000000000080LLU << ((EBML_DATA_SIZE_UNKNOWN_LENGTH - 1) * 7));
  kr_ebml2l2_write_reversed( kr_ebml2, (void *)&data_size, EBML_DATA_SIZE_UNKNOWN_LENGTH);
}

void kr_ebml2l2_write_data_size ( kr_ebml2_t * kr_ebml2, uint64_t data_size) {

  //Max 0x0100000000000000LLU

  uint32_t data_size_length;
  uint64_t data_size_length_mask;

  data_size_length_mask = 0x00000000000000FFLLU;
  data_size_length = 1;

  while (data_size_length < 8) {
    if (data_size < data_size_length_mask) {
      break;
    }
    data_size_length_mask <<= 7;
    data_size_length++;
  }

  data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
  kr_ebml2l2_write_reversed( kr_ebml2, (void *)&data_size, data_size_length);
}

void kr_ebml2l2_write_data ( kr_ebml2_t * kr_ebml2, uint32_t element, void *data, uint64_t length) {
  kr_ebml2l2_write_element ( kr_ebml2, element);
  kr_ebml2l2_write_data_size ( kr_ebml2, length);
  kr_ebml2l2_write ( kr_ebml2, data, length);
}

void kr_ebml2l2_write_string ( kr_ebml2_t * kr_ebml2, uint32_t element, char *string) {

  uint64_t size;
  
  size = strlen(string);

  kr_ebml2l2_write_element ( kr_ebml2, element);
  kr_ebml2l2_write_data_size ( kr_ebml2, size);
  kr_ebml2l2_write ( kr_ebml2, string, strlen(string));
}

void kr_ebml2l2_write_int64 ( kr_ebml2_t * kr_ebml2, uint32_t element, int64_t number) {

  kr_ebml2l2_write_element ( kr_ebml2, element);
  kr_ebml2l2_write_data_size ( kr_ebml2, 8);
  kr_ebml2l2_write_reversed ( kr_ebml2, &number, 8);
}

void kr_ebml2l2_write_int32 ( kr_ebml2_t * kr_ebml2, uint32_t element, int32_t number) {

  kr_ebml2l2_write_element ( kr_ebml2, element);
  kr_ebml2l2_write_data_size ( kr_ebml2, 4);
  kr_ebml2l2_write_reversed ( kr_ebml2, &number, 4);
}

void kr_ebml2l2_write_int16 ( kr_ebml2_t * kr_ebml2, uint32_t element, int16_t number) {

  kr_ebml2l2_write_element ( kr_ebml2, element);
  kr_ebml2l2_write_data_size ( kr_ebml2, 2);
  kr_ebml2l2_write_reversed ( kr_ebml2, &number, 2);
}

void kr_ebml2l2_write_int8 ( kr_ebml2_t * kr_ebml2, uint32_t element, int8_t number) {

  kr_ebml2l2_write_element ( kr_ebml2, element);
  kr_ebml2l2_write_data_size ( kr_ebml2, 1);
  kr_ebml2l2_write_reversed ( kr_ebml2, &number, 1);
}

void kr_ebml2l2_write_float ( kr_ebml2_t * kr_ebml2, uint32_t element, float number) {

  kr_ebml2l2_write_element ( kr_ebml2, element);
  kr_ebml2l2_write_data_size ( kr_ebml2, 4);
  kr_ebml2l2_write_reversed ( kr_ebml2, &number, 4);
}

void kr_ebml2l2_write_double ( kr_ebml2_t * kr_ebml2, uint32_t element, double number) {

  kr_ebml2l2_write_element ( kr_ebml2, element);
  kr_ebml2l2_write_data_size ( kr_ebml2, 8);
  kr_ebml2l2_write_reversed ( kr_ebml2, &number, 8);
}
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

inline void kr_ebml2_advance (kr_ebml2_t *ebml, size_t bytes) {
  ebml->pos += bytes;
  ebml->buf = ebml->buffer + ebml->pos;
}

kr_ebml2_t *kr_ebml2_create () {
  kr_ebml2_t *ebml;
  ebml = calloc (1, sizeof(kr_ebml2_t));
  ebml->buf = ebml->buffer;
  return ebml; 
}

int kr_ebml2_destroy (kr_ebml2_t **ebml) {
  if ((ebml != NULL) && (*ebml != NULL)) {
    free (*ebml);
    return 0;
  }
  return -1;
}

void kr_print_ebml (unsigned char *buffer, int len) {

  int i;
  
  i = 0;

  printf ("Raw EBML: \n");
  for (i = 0; i < len; i++) {
    printf ("%02X", buffer[i]);
  }
  printf ("\nEnd Raw EBML\n");
}

int main (int argc, char *argv[]) {

  kr_ebml2_t *ebml;
  
  ebml = kr_ebml2_create ();

  kr_ebml2_write_element (ebml, EBML_ID_CLUSTER);
  kr_ebml2_write_element (ebml, EBML_ID_TRACK_UID);
  kr_ebml2_write_element (ebml, EBML_ID_TRACK);
  kr_ebml2_write_element (ebml, EBML_ID_CODECDATA);
  kr_ebml2_write_element (ebml, EBML_ID_CLUSTER_TIMECODE);
  kr_ebml2_write_element (ebml, EBML_ID_SEGMENT_INFO);
  kr_ebml2_write_element (ebml, EBML_ID_MUXINGAPP);

  kr_print_ebml (ebml->buffer, ebml->pos);

  kr_ebml2_destroy (&ebml);
  
  return 0;

}

