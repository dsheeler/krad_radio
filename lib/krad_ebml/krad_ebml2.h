#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#ifndef KRAD_EBML2_H
#define KRAD_EBML2_H

#include "krad_system.h"
#include "krad_ebml2_ids.h"

typedef struct kr_ebml2_St kr_ebml2_t;

struct kr_ebml2_St {
  size_t pos;
  size_t len;
  uint8_t *buf;
  uint8_t *bufstart;
};

int kr_ebml2_destroy (kr_ebml2_t **ebml);
kr_ebml2_t *kr_ebml2_create ();

void kr_ebml2_advance (kr_ebml2_t *ebml, size_t bytes);
int kr_ebml2_set_buffer ( kr_ebml2_t *ebml, uint8_t *buffer, size_t len);

void kr_ebml2_pack (kr_ebml2_t *ebml, void *buffer, size_t len);
void kr_ebml2_revpack2 (kr_ebml2_t *ebml, void *buffer);
void kr_ebml2_revpack4 (kr_ebml2_t *ebml, void *buffer);

void kr_ebml2_pack_element (kr_ebml2_t *ebml, uint32_t element);
void kr_ebml2_start_element (kr_ebml2_t *ebml, uint32_t element, uint8_t **position);
void kr_ebml2_finish_element (kr_ebml2_t *ebml, uint8_t *element_position);
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
