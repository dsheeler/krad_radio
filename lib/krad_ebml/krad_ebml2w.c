#include "krad_ebml2w.h"

static void rmemcpy (void *dst, void *src, int len);
static int kr_ebml2_unpack (kr_ebml2_t *ebml, void *buffer, size_t len);
static int kr_ebml2_runpack (kr_ebml2_t *ebml, void *buffer, size_t len);
static uint32_t kr_ebml2_length (uint8_t byte);
//static void kr_ebml2_print_id (uint32_t id);

static void rmemcpy (void *dst, void *src, int len) {

  uint8_t *a_dst;
  uint8_t *a_src;
  int count;
  
  count = 0;
  len -= 1;
  a_dst = dst;
  a_src = src;

  while (len > -1) {
    a_dst[len--] = a_src[count++];
  }
}

static int kr_ebml2_runpack (kr_ebml2_t *ebml, void *buffer, size_t len) {
  // len should be <= 8
  rmemcpy (buffer, ebml->buf, len);
  kr_ebml2_advance (ebml, len);
  return 0;
}

static int kr_ebml2_unpack (kr_ebml2_t *ebml, void *buffer, size_t len) {
  memcpy (buffer, ebml->buf, len);
  kr_ebml2_advance (ebml, len);
  return 0;
}

static uint32_t kr_ebml2_length (uint8_t byte) {

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

/*
static void kr_ebml2_print_id (uint32_t id) {

  uint8_t *bytes;

  bytes = ((uint8_t *)&id);

  if (bytes[3] != 0) {
    //printk ("ID: %02X%02X%02X%02X\n",
    //        bytes[3], bytes[2], bytes[1], bytes[0]);  
  } else {
    if (bytes[2] != 0) {
      //printk ("ID: %02X%02X%02X\n",
      //        bytes[2], bytes[1], bytes[0]);
    } else {
      if (bytes[1] != 0) {
        //printk ("ID: %02X%02X\n",
        //        bytes[1], bytes[0]);
      } else {
        if (bytes[0] != 0) {
          //printk ("ID: %02X\n", bytes[0]);
        } else {
          //printk ("ID: INVALID\n");
        }
      }
    }
  }
}
*/

/* Pub Funcs */

int kr_ebml2_unpack_id (kr_ebml2_t *ebml, uint32_t *element, uint64_t *data_size) {

  uint32_t ebml_id;
  uint32_t ebml_data_size_length;
  uint64_t ebml_data_size;
  uint8_t byte;

  ebml_id = 0;
  ebml_data_size = 0;
  
  //printk ("Unpacking ID at position: %zu\n", ebml->pos);
  //printk(" ");
  //byte = ebml->buffer[ebml->pos];  
  byte = *ebml->buf;

  if (byte & EBML_LENGTH_1) {
    rmemcpy ( &ebml_id, ebml->buf, 1);
    kr_ebml2_advance (ebml, 1);
    //printk ("ID Length: %u\n ", 1);
  } else {
    if (byte & EBML_LENGTH_2) {
      rmemcpy ( &ebml_id, ebml->buf, 2);
      kr_ebml2_advance (ebml, 2);
      //printk ("ID Length: %u\n ", 2);
    } else {
      if (byte & EBML_LENGTH_3) {
        rmemcpy ( &ebml_id, ebml->buf, 3);
        kr_ebml2_advance (ebml, 3);
        //printk ("ID Length: %u\n ", 3);
      } else {
        if (byte & EBML_LENGTH_4) {
          rmemcpy ( &ebml_id, ebml->buf, 4);
          kr_ebml2_advance (ebml, 4);
          //printk ("ID Length: %u\n ", 4);
        } else {
          //printk ("ID Length Invalid\n");
          return -1;
        }
      }
    }
  }

  *element = ebml_id;

  //kr_ebml2_print_id (ebml_id);
  //printk(" ");
  
  //byte = ebml->buffer[ebml->pos];
  byte = *ebml->buf;
  ebml_data_size_length = kr_ebml2_length ( byte );
  //printk("Data size length: %u\n", ebml_data_size_length);
  //printk(" ");

  if (ebml_data_size_length == 1) {
    kr_ebml2_advance (ebml, 1);
    ebml_data_size = byte - EBML_LENGTH_1;
  } else {
    if (ebml_data_size_length == 2) {
      kr_ebml2_runpack (ebml, &ebml_data_size, 2);
      ((uint8_t *)&ebml_data_size)[1] = byte - EBML_LENGTH_2;
    } else {
      if (ebml_data_size_length == 3) {
        kr_ebml2_runpack (ebml, &ebml_data_size, 3);
        ((uint8_t *)&ebml_data_size)[2] = byte - EBML_LENGTH_3;
      } else {
        if (ebml_data_size_length == 8) {
          kr_ebml2_runpack (ebml, &ebml_data_size, 8);
          ((uint8_t *)&ebml_data_size)[7] = byte - EBML_LENGTH_8;
        } else {
          if (ebml_data_size_length == 4) {
            kr_ebml2_runpack (ebml, &ebml_data_size, 4);
            ((uint8_t *)&ebml_data_size)[3] = byte - EBML_LENGTH_4;
          } else {
            if (ebml_data_size_length == 5) {
              kr_ebml2_runpack (ebml, &ebml_data_size, 5);
              ((uint8_t *)&ebml_data_size)[4] = byte - EBML_LENGTH_5;
            } else {
              if (ebml_data_size_length == 6) {
                kr_ebml2_runpack (ebml, &ebml_data_size, 6);
                ((uint8_t *)&ebml_data_size)[5] = byte - EBML_LENGTH_6;
              }
            }
          }
        }
      }
    }
  }
  
  if (ebml_data_size != EBML_DATA_SIZE_UNKNOWN_UNPACKED) {
    *data_size = ebml_data_size;
    //printk("Data size is %"PRIu64"\n", ebml_data_size);  
  } else {
    *data_size = 0;
    printk ("Data size is Unknown!");
  }

  return 0;
}

int kr_ebml2_unpack_data (kr_ebml2_t *ebml, void *data, size_t len) {
  kr_ebml2_unpack (ebml, data, len);
  return 0;
}

int kr_ebml2_unpack_string (kr_ebml2_t *ebml, char *string, size_t len) {
  memcpy (string, ebml->buf, len);
  kr_ebml2_advance (ebml, len);
  string[len] = '\0';
  return 0;  
}

int kr_ebml2_unpack_int8 (kr_ebml2_t *ebml, int8_t *integer, size_t len) {
  *integer = 0;
  kr_ebml2_runpack (ebml, integer, len);
  return 0;
}

int kr_ebml2_unpack_uint8 (kr_ebml2_t *ebml, uint8_t *integer, size_t len) {
  *integer = 0;
  kr_ebml2_runpack (ebml, integer, len);
  return 0;
}

int kr_ebml2_unpack_int16 (kr_ebml2_t *ebml, int16_t *integer, size_t len) {
  *integer = 0;
  kr_ebml2_runpack (ebml, integer, len);
  return 0;
}

int kr_ebml2_unpack_uint16 (kr_ebml2_t *ebml, uint16_t *integer, size_t len) {
  *integer = 0;
  kr_ebml2_runpack (ebml, integer, len);
  return 0;
}

int kr_ebml2_unpack_int32 (kr_ebml2_t *ebml, int32_t *integer, size_t len) {
  *integer = 0;
  kr_ebml2_runpack (ebml, integer, len);
  return 0;
}

int kr_ebml2_unpack_uint32 (kr_ebml2_t *ebml, uint32_t *integer, size_t len) {
  *integer = 0;
  kr_ebml2_runpack (ebml, integer, len);
  return 0;
}

int kr_ebml2_unpack_int64 (kr_ebml2_t *ebml, int64_t *integer, size_t len) {
  *integer = 0;
  kr_ebml2_runpack (ebml, integer, len);
  return 0;
}

int kr_ebml2_unpack_uint64 (kr_ebml2_t *ebml, uint64_t *integer, size_t len) {
  *integer = 0;
  kr_ebml2_runpack (ebml, integer, len);
  return 0;
}

int kr_ebml2_unpack_float (kr_ebml2_t *ebml, float *real, size_t len) {
  *real = 0;
  kr_ebml2_runpack (ebml, real, len);
  return 0;
}

int kr_ebml2_unpack_double (kr_ebml2_t *ebml, double *real, size_t len) {
  *real = 0;
  kr_ebml2_runpack (ebml, real, len);
  return 0;
}

int kr_ebml2_unpack_element_data (kr_ebml2_t *ebml, uint32_t *id, void *data, size_t maxlen) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_data (ebml, data, size);
  return 0;  
}

int kr_ebml2_unpack_element_string (kr_ebml2_t *ebml, uint32_t *id, char *string, size_t maxlen) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_string (ebml, string, size);
  return 0;  
}

int kr_ebml2_unpack_element_int8 (kr_ebml2_t *ebml, uint32_t *id, int8_t *integer) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_int8 (ebml, integer, size);
  return 0;  
}

int kr_ebml2_unpack_element_uint8 (kr_ebml2_t *ebml, uint32_t *id, uint8_t *integer) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_uint8 (ebml, integer, size);
  return 0;  
}

int kr_ebml2_unpack_element_int16 (kr_ebml2_t *ebml, uint32_t *id, int16_t *integer) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_int16 (ebml, integer, size);
  return 0;  
}

int kr_ebml2_unpack_element_uint16 (kr_ebml2_t *ebml, uint32_t *id, uint16_t *integer) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_uint16 (ebml, integer, size);
  return 0;  
}

int kr_ebml2_unpack_element_int32 (kr_ebml2_t *ebml, uint32_t *id, int32_t *integer) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_int32 (ebml, integer, size);
  return 0;  
}

int kr_ebml2_unpack_element_uint32 (kr_ebml2_t *ebml, uint32_t *id, uint32_t *integer) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_uint32 (ebml, integer, size);
  return 0;  
}

int kr_ebml2_unpack_element_int64 (kr_ebml2_t *ebml, uint32_t *id, int64_t *integer) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_int64 (ebml, integer, size);
  return 0;  
}

int kr_ebml2_unpack_element_uint64 (kr_ebml2_t *ebml, uint32_t *id, uint64_t *integer) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_uint64 (ebml, integer, size);
  return 0;  
}

int kr_ebml2_unpack_element_float (kr_ebml2_t *ebml, uint32_t *id, float *real) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_float (ebml, real, size);
  return 0;  
}

int kr_ebml2_unpack_element_double (kr_ebml2_t *ebml, uint32_t *id, double *real) {

  uint32_t element_id;
  uint64_t size;

  kr_ebml2_unpack_id (ebml, &element_id, &size);
  //fixme check id and maxlen
  kr_ebml2_unpack_double (ebml, real, size);
  return 0;  
}

int kr_ebml2_unpack_header (kr_ebml2_t *ebml,
                            char *doctype, int doctype_maxlen,
                            uint32_t *version, uint32_t *read_version) {

  int ret;
  uint32_t element;
  uint64_t size;
  uint64_t header_size;
  int header_items;

  header_items = 7;

  ret = kr_ebml2_unpack_id (ebml, &element, &header_size);

  if (ret < 0) {
    printke ("EBML Header Read Error: %d", ret);
    return -1;
  }

  if (element != EID_HEADER) {
    printke ("EBML Header ID Not found");
    return -1;
  }

  header_size += ebml->pos;

  if (ebml->len < header_size) {
    printke ("Not Enough bytes in header..");
    return -1;
  }

  while ((header_items) && (ebml->pos < header_size)) {

    ret = kr_ebml2_unpack_id (ebml, &element, &size);
    
    if (ret < 0) {
      printke ("EBML Header Read Error: %d", ret);
      return -1;
    }
    
    switch (element) {
      case EID_DOCTYPE:
        if (doctype != NULL) {
          if ((size + 1) <= doctype_maxlen) {
            kr_ebml2_unpack_string (ebml, doctype, size);
          } else {
            printke ("Doctype Length Excedes max");
            return -1;
          }
        } else {
          kr_ebml2_advance (ebml, size);
        }
        break;
      case EID_VERSION:
        kr_ebml2_advance (ebml, size);
        //printk ("Found VERSION");
        break;
      case EID_READVERSION:
        kr_ebml2_advance (ebml, size);
        //printk ("Found READVERSION");
        break;
      case EID_MAXIDLENGTH:
        kr_ebml2_advance (ebml, size);
        //printk ("Found MAXIDLENGTH");
        break;
      case EID_MAXSIZELENGTH:
        kr_ebml2_advance (ebml, size);
        //printk ("Found MAXSIZELENGTH");
        break;
      case EID_DOCTYPEVERSION:
        kr_ebml2_unpack_uint32 (ebml, version, size);
        //printk ("Found DOCTYPEVERSION");
        break;
      case EID_DOCTYPEREADVERSION:
        //printk ("Found DOCTYPEREADVERSION");
        kr_ebml2_unpack_uint32 (ebml, read_version, size);
        break;
      default:
        printke ("Unknown Element found in EBML Header");
        return -1;
    }
    header_items--;
  }

  return ebml->pos;
}

int kr_ebml2_at_end (kr_ebml2_t *ebml) {
  return ebml->pos == ebml->len;
}

