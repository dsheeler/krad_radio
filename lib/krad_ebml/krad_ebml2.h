#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#ifndef KRAD_EBML2_H
#define KRAD_EBML2_H

typedef struct kr_ebml2_St kr_ebml2_t;

struct kr_ebml2_St {
  unsigned char *buf;
  unsigned char buffer[8192];
  size_t pos;
};


void kr_print_ebml (unsigned char *buffer, int len);
inline void kr_ebml2_advance (kr_ebml2_t *ebml, size_t bytes);
int kr_ebml2_destroy (kr_ebml2_t **ebml);
kr_ebml2_t *kr_ebml2_create ();

void kr_ebml2_start_element (kr_ebml2_t *ebml, uint32_t element, unsigned char **position);
void kr_ebml2_finish_element (kr_ebml2_t *ebml, unsigned char *element_position);
void kr_ebml2_pack_data ( kr_ebml2_t *ebml, uint32_t element, void *data, uint64_t length);
void kr_ebml2_pack_string ( kr_ebml2_t *ebml, uint32_t element, char *string);
void kr_ebml2_pack_float ( kr_ebml2_t *ebml, uint32_t element, float number);
void kr_ebml2_pack_double ( kr_ebml2_t *ebml, uint32_t element, double number);
void kr_ebml2_pack_int8 ( kr_ebml2_t *ebml, uint32_t element, int8_t number);
void kr_ebml2_pack_int16 ( kr_ebml2_t *ebml, uint32_t element, int16_t number);
void kr_ebml2_pack_int32 ( kr_ebml2_t *ebml, uint32_t element, int32_t number);
void kr_ebml2_pack_int64 ( kr_ebml2_t * kr_ebml2, uint32_t element, int64_t number);

#endif
