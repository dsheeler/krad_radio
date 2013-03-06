#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#ifndef KRAD_EBML2_H
#define KRAD_EBML2_H

#include "krad_ebml2_ids.h"

#define KRAD_EBML2_BUF_SZ 1024 * 1024 * 1024

typedef struct kr_ebml2_St kr_ebml2_t;

struct kr_ebml2_St {
  size_t pos;
  size_t len;
  unsigned char *buf;
  unsigned char *bufstart;
  //unsigned char buffer[KRAD_EBML2_BUF_SZ];
};

inline void kr_ebml2_advance (kr_ebml2_t *ebml, size_t bytes);
inline void rmemcpy1 (unsigned char *dst, unsigned char *src);
inline void rmemcpy2 (unsigned char *dst, unsigned char *src);
inline void rmemcpy3 (unsigned char *dst, unsigned char *src);
inline void rmemcpy4 (unsigned char *dst, unsigned char *src);
inline void rmemcpy5 (unsigned char *dst, unsigned char *src);
inline void rmemcpy6 (unsigned char *dst, unsigned char *src);
inline void rmemcpy7 (unsigned char *dst, unsigned char *src);
inline void rmemcpy8 (unsigned char *dst, unsigned char *src);

int kr_ebml2_destroy (kr_ebml2_t **ebml);
kr_ebml2_t *kr_ebml2_create ();

int kr_ebml2_set_buffer ( kr_ebml2_t *ebml, unsigned char *buffer, size_t len);

inline void kr_ebml2_pack_element (kr_ebml2_t *ebml, uint32_t element);
void kr_ebml2_start_element (kr_ebml2_t *ebml, uint32_t element, unsigned char **position);
void kr_ebml2_finish_element (kr_ebml2_t *ebml, unsigned char *element_position);
void kr_ebml2_pack_data ( kr_ebml2_t *ebml, uint32_t element, void *data, uint64_t length);
void kr_ebml2_pack_string ( kr_ebml2_t *ebml, uint32_t element, char *string);
void kr_ebml2_pack_float ( kr_ebml2_t *ebml, uint32_t element, float number);
void kr_ebml2_pack_double ( kr_ebml2_t *ebml, uint32_t element, double number);
void kr_ebml2_pack_int8 ( kr_ebml2_t *ebml, uint32_t element, int8_t number);
void kr_ebml2_pack_uint8 ( kr_ebml2_t *ebml, uint32_t element, uint8_t number);
void kr_ebml2_pack_int16 ( kr_ebml2_t *ebml, uint32_t element, int16_t number);
void kr_ebml2_pack_uint16 ( kr_ebml2_t *ebml, uint32_t element, uint16_t number);
void kr_ebml2_pack_int32 ( kr_ebml2_t *ebml, uint32_t element, int32_t number);
void kr_ebml2_pack_uint32 ( kr_ebml2_t *ebml, uint32_t element, uint32_t number);
void kr_ebml2_pack_int64 ( kr_ebml2_t * kr_ebml2, uint32_t element, int64_t number);
void kr_ebml2_pack_uint64 ( kr_ebml2_t * kr_ebml2, uint32_t element, uint64_t number);

void kr_ebml2_pack_header ( kr_ebml2_t *ebml, char *doctype, uint32_t version, uint32_t read_version);

#endif
