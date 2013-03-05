#include "krad_ebml2w.h"

static inline void rmemcpy (void *dst, void *src, int len) {

  unsigned char *a_dst;
  unsigned char *a_src;
  int count;
  
  count = 0;
  len -= 1;
  a_dst = dst;
  a_src = src;

  while (len > -1) {
    a_dst[len--] = a_src[count++];
  }
}
/*
uint64_t kr_ebml2_unpack_number (unsigned char *ebml_frag, uint64_t ebml_data_size, int *pos) {

  unsigned char temp[7];
  uint64_t number;
  
  number = 0;
  
  *pos += ebml_data_size;
  
  memset (temp, '\0', sizeof(temp));
  memcpy (&temp, &ebml_frag[0], ebml_data_size);
  rmemcpy ( &number, &temp, ebml_data_size);
  
  return number;

}

float kr_ebml2_unpack_float (unsigned char *ebml_frag, uint64_t ebml_data_size, int *pos) {

  unsigned char temp[7];
  float number;
  
  number = 0;
  
  *pos += ebml_data_size;
  
  memset (temp, '\0', sizeof(temp));
  memcpy (&temp, &ebml_frag[0], ebml_data_size);
  rmemcpy ( &number, &temp, ebml_data_size);

  return number;

}
*/
static inline uint32_t kr_ebml2_length (unsigned char byte) {

  if (byte & EBML_LENGTH_1) {
    return 1;
  }

  if (byte & EBML_LENGTH_2) {
    return 2;
  }
  
  if (byte & EBML_LENGTH_3) {
    return 3;
  }
  
  if (byte & EBML_LENGTH_4) {
    return 4;
  }
  
  if (byte & EBML_LENGTH_5) {
    return 5;
  }
  
  if (byte & EBML_LENGTH_6) {
    return 6;
  }
  
  if (byte & EBML_LENGTH_7) {
    return 7;
  }
  
  if (byte & EBML_LENGTH_8) {
    return 8;
  }
  // this case is not possible
  return 0;
}

inline void kr_ebml2_unpack (kr_ebml2_t *ebml, void *buffer, size_t len) {
  memcpy (buffer, ebml->buf, len);
  kr_ebml2_advance (ebml, len);
}

inline void kr_ebml2_runpack (kr_ebml2_t *ebml, void *buffer, size_t len) {
  rmemcpy (buffer, ebml->buf, len);
  kr_ebml2_advance (ebml, len);
}

void kr_ebml2_print_id (uint32_t id) {

  unsigned char *bytes;

  bytes = ((unsigned char *)&id);

  if (bytes[3] != 0) {
    printf ("ID: %02X%02X%02X%02X\n",
            bytes[3], bytes[2], bytes[1], bytes[0]);  
  } else {
    if (bytes[2] != 0) {
      printf ("ID: %02X%02X%02X\n",
              bytes[2], bytes[1], bytes[0]);
    } else {
      if (bytes[1] != 0) {
        printf ("ID: %02X%02X\n",
                bytes[1], bytes[0]);
      } else {
        if (bytes[0] != 0) {
          printf ("ID: %02X\n", bytes[0]);
        } else {
          printf ("ID: INVALID\n");
        }
      }
    }
  }
}

int kr_ebml2_unpack_id (kr_ebml2_t *ebml, uint32_t *element, uint64_t *data_size) {

  uint32_t ebml_id;
  uint32_t ebml_data_size_length;
  uint64_t ebml_data_size;
  unsigned char byte;

  ebml_id = 0;
  ebml_data_size = 0;
  
  printf ("Unpacking ID at position: %zu\n", ebml->pos);
  printf(" ");
  byte = ebml->buffer[ebml->pos];

  if (byte & EBML_LENGTH_1) {
    rmemcpy ( &ebml_id, &ebml->buffer[ebml->pos], 1);
    kr_ebml2_advance (ebml, 1);
    printf ("ID Length: %u\n ", 1);
  } else {
    if (byte & EBML_LENGTH_2) {
      rmemcpy ( &ebml_id, &ebml->buffer[ebml->pos], 2);
      kr_ebml2_advance (ebml, 2);
      printf ("ID Length: %u\n ", 2);
    } else {
      if (byte & EBML_LENGTH_3) {
        rmemcpy ( &ebml_id, &ebml->buffer[ebml->pos], 3);
        kr_ebml2_advance (ebml, 3);
        printf ("ID Length: %u\n ", 3);
      } else {
        if (byte & EBML_LENGTH_4) {
          rmemcpy ( &ebml_id, &ebml->buffer[ebml->pos], 4);
          kr_ebml2_advance (ebml, 4);
          printf ("ID Length: %u\n ", 4);
        } else {
          printf ("ID Length Invalid\n");
          return -1;
        }
      }
    }
  }

  *element = ebml_id;

  kr_ebml2_print_id (ebml_id);
  printf(" ");
  
  byte = ebml->buffer[ebml->pos];
  ebml_data_size_length = kr_ebml2_length ( byte );
  printf("Data size length: %u\n", ebml_data_size_length);
  printf(" ");

  if (ebml_data_size_length == 1) {
    kr_ebml2_advance (ebml, 1);
    ebml_data_size = byte - EBML_LENGTH_1;
  } else {
    if (ebml_data_size_length == 2) {
      kr_ebml2_runpack (ebml, &ebml_data_size, 2);
      ((unsigned char *)&ebml_data_size)[1] = byte - EBML_LENGTH_2;
    } else {
      if (ebml_data_size_length == 3) {
        kr_ebml2_runpack (ebml, &ebml_data_size, 3);
        ((unsigned char *)&ebml_data_size)[2] = byte - EBML_LENGTH_3;
      } else {
        if (ebml_data_size_length == 8) {
          kr_ebml2_runpack (ebml, &ebml_data_size, 8);
          ((unsigned char *)&ebml_data_size)[7] = byte - EBML_LENGTH_8;
        } else {
          if (ebml_data_size_length == 4) {
            kr_ebml2_runpack (ebml, &ebml_data_size, 4);
            ((unsigned char *)&ebml_data_size)[3] = byte - EBML_LENGTH_4;
          } else {
            if (ebml_data_size_length == 5) {
              kr_ebml2_runpack (ebml, &ebml_data_size, 5);
              ((unsigned char *)&ebml_data_size)[4] = byte - EBML_LENGTH_5;
            } else {
              if (ebml_data_size_length == 6) {
                kr_ebml2_runpack (ebml, &ebml_data_size, 6);
                ((unsigned char *)&ebml_data_size)[5] = byte - EBML_LENGTH_6;
              }
            }
          }
        }
      }
    }
  }
  
  if (ebml_data_size != EBML_DATA_SIZE_UNKNOWN_UNPACKED) {
    *data_size = ebml_data_size;
    printf("Data size is %"PRIu64"\n", ebml_data_size);  
  } else {
    *data_size = 0;
    printf ("Data size is Unknown!\n");
  }

  return 0;
}
