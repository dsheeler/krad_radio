#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#ifndef KRAD_CBOR_H
#define KRAD_CBOR_H

typedef struct kr_cbor kr_cbor;

int kr_cbor_free(kr_cbor *cbor);
kr_cbor *kr_cbor_create();
int kr_cbor_init(kr_cbor *cbor);
size_t kr_cbor_sizeof();

void *kr_cbor_buffer(kr_cbor *cbor);
int kr_cbor_buffer_unset(kr_cbor *cbor);
int kr_cbor_buffer_set(kr_cbor *cbor, void *buffer, size_t len);
int kr_cbor_buffer_reset(kr_cbor *cbor);
size_t kr_cbor_position(kr_cbor *cbor);
size_t kr_cbor_space(kr_cbor *cbor);

int kr_cbor_pack_binary(kr_cbor *cbor, void *data, uint64_t len);
int kr_cbor_pack_string(kr_cbor *cbor, char *string, size_t len);
int kr_cbor_pack_float(kr_cbor *cbor, float number);
int kr_cbor_pack_double(kr_cbor *cbor, double number);
int kr_cbor_pack_int8(kr_cbor *cbor, int8_t number);
int kr_cbor_pack_uint8(kr_cbor *cbor, uint8_t number);
int kr_cbor_pack_int16(kr_cbor *cbor, int16_t number);
int kr_cbor_pack_uint16(kr_cbor *cbor, uint16_t number);
int kr_cbor_pack_int32(kr_cbor *cbor, int32_t number);
int kr_cbor_pack_uint32(kr_cbor *cbor, uint32_t number);
int kr_cbor_pack_int64(kr_cbor *cbor, int64_t number);
int kr_cbor_pack_uint64(kr_cbor *cbor, uint64_t number);

#endif
