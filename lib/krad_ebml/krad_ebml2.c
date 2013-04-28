#include "krad_ebml2.h"

void kr_ebml2_advance (kr_ebml2_t *ebml, size_t bytes);
void rmemcpy1 (uint8_t *dst, uint8_t *src);
void rmemcpy2 (uint8_t *dst, uint8_t *src);
void rmemcpy3 (uint8_t *dst, uint8_t *src);
void rmemcpy4 (uint8_t *dst, uint8_t *src);
void rmemcpy5 (uint8_t *dst, uint8_t *src);
void rmemcpy6 (uint8_t *dst, uint8_t *src);
void rmemcpy7 (uint8_t *dst, uint8_t *src);
void rmemcpy8 (uint8_t *dst, uint8_t *src);

void rmemcpy1 (uint8_t *dst, uint8_t *src) {
  dst[0] = src[0];
}

void rmemcpy2 (uint8_t *dst, uint8_t *src) {
  dst[0] = src[1];
  dst[1] = src[0];
}

void rmemcpy3 (uint8_t *dst, uint8_t *src) {
  dst[0] = src[2];
  dst[1] = src[1];
  dst[2] = src[0];
}

void rmemcpy4 (uint8_t *dst, uint8_t *src) {
  dst[0] = src[3];
  dst[1] = src[2];
  dst[2] = src[1];
  dst[3] = src[0];
}

void rmemcpy5 (uint8_t *dst, uint8_t *src) {
  dst[0] = src[4];
  dst[1] = src[3];
  dst[2] = src[2];
  dst[3] = src[1];
  dst[4] = src[0];
}

void rmemcpy6 (uint8_t *dst, uint8_t *src) {
  dst[0] = src[5];
  dst[1] = src[4];
  dst[2] = src[3];
  dst[3] = src[2];
  dst[4] = src[1];
  dst[5] = src[0];
}

void rmemcpy7 (uint8_t *dst, uint8_t *src) {
  dst[0] = src[6];
  dst[1] = src[5];
  dst[2] = src[4];
  dst[3] = src[3];
  dst[4] = src[2];
  dst[5] = src[1];
  dst[6] = src[0];
}

void rmemcpy8 (uint8_t *dst, uint8_t *src) {
  dst[0] = src[7];
  dst[1] = src[6];
  dst[2] = src[5];
  dst[3] = src[4];
  dst[4] = src[3];
  dst[5] = src[2];
  dst[6] = src[1];
  dst[7] = src[0];
}

void kr_ebml2_advance (kr_ebml2_t *ebml, size_t bytes) {
  ebml->pos += bytes;
  ebml->buf += bytes;
}

void kr_ebml2_pack (kr_ebml2_t *ebml, void *buffer, size_t len) {
  memcpy (ebml->buf, buffer, len);
  kr_ebml2_advance (ebml, len);
}

void kr_ebml2_revpack1 (kr_ebml2_t *ebml, void *buffer) {
  rmemcpy1 (ebml->buf, buffer);
  kr_ebml2_advance (ebml, 1);
}

void kr_ebml2_revpack2 (kr_ebml2_t *ebml, void *buffer) {
  rmemcpy2 (ebml->buf, buffer);
  kr_ebml2_advance (ebml, 2);
}

void kr_ebml2_revpack3 (kr_ebml2_t *ebml, void *buffer) {
  rmemcpy3 (ebml->buf, buffer);
  kr_ebml2_advance (ebml, 3);
}

void kr_ebml2_revpack4 (kr_ebml2_t *ebml, void *buffer) {
  rmemcpy4 (ebml->buf, buffer);
  kr_ebml2_advance (ebml, 4);
}

void kr_ebml2_revpack5 (kr_ebml2_t *ebml, void *buffer) {
  rmemcpy5 (ebml->buf, buffer);
  kr_ebml2_advance (ebml, 5);
}

void kr_ebml2_revpack6 (kr_ebml2_t *ebml, void *buffer) {
  rmemcpy6 (ebml->buf, buffer);
  kr_ebml2_advance (ebml, 6);
}

void kr_ebml2_revpack7 (kr_ebml2_t *ebml, void *buffer) {
  rmemcpy7 (ebml->buf, buffer);
  kr_ebml2_advance (ebml, 7);
}

void kr_ebml2_revpack8 (kr_ebml2_t *ebml, void *buffer) {
  rmemcpy8 (ebml->buf, buffer);
  kr_ebml2_advance (ebml, 8);
}

void kr_ebml2_pack_element (kr_ebml2_t *ebml, uint32_t element) {
  if (element < 0x00000100) {
    kr_ebml2_revpack1 (ebml, (uint8_t *)&element);
  } else {
    if (element < 0x00010000) {
      kr_ebml2_revpack2 (ebml, (uint8_t *)&element);
    } else {
      if (element < 0x01000000) {
        kr_ebml2_revpack3 (ebml, (uint8_t *)&element);
      } else {
        kr_ebml2_revpack4 (ebml, (uint8_t *)&element);
      }
    }
  }
}

void kr_ebml2_pack_data_size_unknown (kr_ebml2_t *ebml) {
  uint64_t data_size;
  data_size = EBML_DATA_SIZE_UNKNOWN;
  kr_ebml2_revpack8 ( ebml, &data_size);
}

void kr_ebml2_pack_big_data_size ( kr_ebml2_t *ebml, uint64_t data_size ) {

  uint32_t data_size_length;
  uint64_t data_size_length_mask;

  data_size_length_mask = 0x00000000000000FFLLU;
  data_size_length = 1;

  if (data_size < data_size_length_mask) {
    data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
    kr_ebml2_revpack1 ( ebml, (void *)&data_size);
  } else {
    data_size_length_mask <<= 7;
    data_size_length++;
    if (data_size < data_size_length_mask) {
      data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
      kr_ebml2_revpack2 ( ebml, (void *)&data_size);
    } else {
      data_size_length_mask <<= 7;
      data_size_length++;
      if (data_size < data_size_length_mask) {
        data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
        kr_ebml2_revpack3 ( ebml, (void *)&data_size);
      } else {
        data_size_length_mask <<= 7;
        data_size_length++;
        if (data_size < data_size_length_mask) {
          data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
          kr_ebml2_revpack4 ( ebml, (void *)&data_size);
        } else {
          data_size_length_mask <<= 7;
          data_size_length++;
          if (data_size < data_size_length_mask) {
            data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
            kr_ebml2_revpack5 ( ebml, (void *)&data_size);
          } else {
            data_size_length_mask <<= 7;
            data_size_length++;
            if (data_size < data_size_length_mask) {
              data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
              kr_ebml2_revpack6 ( ebml, (void *)&data_size);
            } else {
              data_size_length_mask <<= 7;
              data_size_length++;
              if (data_size < data_size_length_mask) {
                data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
                kr_ebml2_revpack7 ( ebml, (void *)&data_size);
              } else {
                data_size_length_mask <<= 7;
                data_size_length++;
                if (data_size < data_size_length_mask) {
                  data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
                  kr_ebml2_revpack8 ( ebml, (void *)&data_size);
                } else {
                  kr_ebml2_pack_data_size_unknown ( ebml );
                }
              }
            }
          }
        }
      }
    }
  }
}

void kr_ebml2_pack_data_size ( kr_ebml2_t *ebml, uint64_t data_size ) {

  uint32_t data_size_length;
  uint64_t data_size_length_mask;

  data_size_length_mask = 0x00000000000000FFLLU;
  data_size_length = 1;

  if (data_size < data_size_length_mask) {
    data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
    kr_ebml2_revpack1 ( ebml, (void *)&data_size);
  } else {
    data_size_length_mask <<= 7;
    data_size_length++;
    if (data_size < data_size_length_mask) {
      data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
      kr_ebml2_revpack2 ( ebml, (void *)&data_size);
    } else {
      data_size_length_mask <<= 7;
      data_size_length++;
      if (data_size < data_size_length_mask) {
        data_size |= (0x000000000000080LLU << ((data_size_length - 1) * 7));
        kr_ebml2_revpack3 ( ebml, (void *)&data_size);
      } else {
        kr_ebml2_pack_big_data_size ( ebml, data_size );
      }
    }
  }
}

void kr_ebml2_pack_data_size_update (kr_ebml2_t *ebml, uint8_t *element_position, uint64_t data_size) {
  data_size |= (0x000000000000080LLU << ((EBML_DATA_SIZE_UNKNOWN_LENGTH - 1) * 7));
  rmemcpy8 (element_position, (uint8_t *)&data_size);
}

void kr_ebml2_start_element (kr_ebml2_t *ebml, uint32_t element, uint8_t **position) {
  kr_ebml2_pack_element ( ebml, element );
  *position = ebml->buf;
  kr_ebml2_pack_data_size_unknown ( ebml ); 
}

void kr_ebml2_finish_element (kr_ebml2_t *ebml, uint8_t *element_position) {

  uint64_t element_data_size;

  element_data_size = ebml->buf - element_position - EBML_DATA_SIZE_UNKNOWN_LENGTH;
  kr_ebml2_pack_data_size_update ( ebml, element_position, element_data_size);
}

void kr_ebml2_pack_data ( kr_ebml2_t *ebml, uint32_t element, void *data, uint64_t length) {
  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, length );
  kr_ebml2_pack ( ebml, data, length );
}

void kr_ebml2_pack_string ( kr_ebml2_t *ebml, uint32_t element, char *string) {

  uint64_t size;
  
  size = strlen(string);

  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, size );
  kr_ebml2_pack ( ebml, string, size );
}

void kr_ebml2_pack_int8 ( kr_ebml2_t *ebml, uint32_t element, int8_t number) {
  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, 1 );
  kr_ebml2_revpack1 ( ebml, &number );
}

void kr_ebml2_pack_uint8 ( kr_ebml2_t *ebml, uint32_t element, uint8_t number) {
  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, 1 );
  kr_ebml2_revpack1 ( ebml, &number );
}

void kr_ebml2_pack_int16 ( kr_ebml2_t *ebml, uint32_t element, int16_t number) {
  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, 2 );
  kr_ebml2_revpack2 ( ebml, &number );
}

void kr_ebml2_pack_uint16 ( kr_ebml2_t *ebml, uint32_t element, uint16_t number) {
  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, 2 );
  kr_ebml2_revpack2 ( ebml, &number );
}

void kr_ebml2_pack_int32 ( kr_ebml2_t *ebml, uint32_t element, int32_t number) {
  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, 4 );
  kr_ebml2_revpack4 ( ebml, &number );
}

void kr_ebml2_pack_uint32 ( kr_ebml2_t *ebml, uint32_t element, uint32_t number) {
  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, 4 );
  kr_ebml2_revpack4 ( ebml, &number );
}

void kr_ebml2_pack_int64 ( kr_ebml2_t *ebml, uint32_t element, int64_t number) {
  kr_ebml2_pack_element ( ebml, element);
  kr_ebml2_pack_data_size ( ebml, 8);
  kr_ebml2_revpack8 ( ebml, &number );
}

void kr_ebml2_pack_uint64 ( kr_ebml2_t *ebml, uint32_t element, uint64_t number) {
  kr_ebml2_pack_element ( ebml, element);
  kr_ebml2_pack_data_size ( ebml, 8);
  kr_ebml2_revpack8 ( ebml, &number );
}

void kr_ebml2_pack_float ( kr_ebml2_t *ebml, uint32_t element, float number) {
  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, 4 );
  kr_ebml2_revpack4 ( ebml, &number );
}

void kr_ebml2_pack_double ( kr_ebml2_t *ebml, uint32_t element, double number) {
  kr_ebml2_pack_element ( ebml, element );
  kr_ebml2_pack_data_size ( ebml, 8 );
  kr_ebml2_revpack8 ( ebml, &number );
}

void kr_ebml2_pack_header ( kr_ebml2_t *ebml, char *doctype, uint32_t version, uint32_t read_version) {

  uint8_t *header;

  kr_ebml2_start_element (ebml, EID_HEADER, &header);
  
  kr_ebml2_pack_uint8 (ebml, EID_VERSION, 1);  
  kr_ebml2_pack_uint8 (ebml, EID_READVERSION, 1);
  kr_ebml2_pack_uint8 (ebml, EID_MAXIDLENGTH, 4);
  kr_ebml2_pack_uint8 (ebml, EID_MAXSIZELENGTH, 8);
  
  kr_ebml2_pack_string (ebml, EID_DOCTYPE, doctype);
  kr_ebml2_pack_uint8 (ebml, EID_DOCTYPEVERSION, version);
  kr_ebml2_pack_uint8 (ebml, EID_DOCTYPEREADVERSION, read_version);

  kr_ebml2_finish_element (ebml, header);
}

int kr_ebml2_set_buffer ( kr_ebml2_t *ebml, uint8_t *buffer, size_t len) {
  if ((buffer == NULL) || (len < 1)) {
    return -1;
  }
  ebml->buf = buffer;
  ebml->bufstart = buffer;
  ebml->pos = 0;
  ebml->len = len;
  return 0;
}

kr_ebml2_t *kr_ebml2_create () {
  kr_ebml2_t *ebml;
  ebml = malloc (sizeof(kr_ebml2_t));
  //ebml->buf = ebml->buffer;
  ebml->buf = NULL;
  ebml->bufstart = NULL;
  ebml->pos = 0;
  return ebml; 
}

int kr_ebml2_destroy (kr_ebml2_t **ebml) {
  if ((ebml != NULL) && (*ebml != NULL)) {
    free (*ebml);
    return 0;
  }
  return -1;
}

