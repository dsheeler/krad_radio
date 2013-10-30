#include "krad_cbor.h"

#define KR_CBOR_UINT 0x00
#define KR_CBOR_NINT 0x01
#define KR_CBOR_BIN 0x02
#define KR_CBOR_STR 0x03
#define KR_CBOR_ARR 0x04
#define KR_CBOR_MAP 0x05
#define KR_CBOR_TAG 0x06
#define KR_CBOR_FLOAT 0x07

struct kr_cbor {
  size_t pos;
  size_t len;
  uint8_t *cur;
  uint8_t *buf;
};

int kr_cbor_free(kr_cbor *cbor) {
  if (cbor == NULL) return -1;
  free(cbor);
  return 0;
}

kr_cbor *kr_cbor_create() {
  kr_cbor *cbor;
  cbor = malloc(kr_cbor_sizeof());
  if (cbor == NULL) return NULL;
  if (kr_cbor_init(cbor) != 0) {
    free(cbor);
    return NULL;
  }
  return cbor;
}

int kr_cbor_init(kr_cbor *cbor) {
  if (cbor == NULL) return -1;
  memset(cbor, 0, kr_cbor_sizeof());
  return 0;
}

size_t kr_cbor_sizeof() {
  return sizeof(kr_cbor);
}

int kr_cbor_buffer_unset(kr_cbor *cbor) {
  if (cbor == NULL) return -1;
  if (cbor->buf == NULL) return -2;
  cbor->buf = NULL;
  cbor->cur = cbor->buf;
  cbor->pos = 0;
  cbor->len = 0;
  return 0;
}

int kr_cbor_buffer_set(kr_cbor *cbor, void *buffer, size_t len) {
  if (cbor == NULL) return -1;
  if (buffer == NULL) return -2;
  if (len < 1) return -3;
  cbor->buf = buffer;
  cbor->cur = cbor->buf;
  cbor->pos = 0;
  cbor->len = len;
  return 0;
}

void *kr_cbor_buffer(kr_cbor *cbor) {
  if (cbor == NULL) return NULL;
  return cbor->buf;
}

int kr_cbor_buffer_reset(kr_cbor *cbor) {
  if (cbor == NULL) return -1;
  cbor->cur = cbor->buf;
  cbor->pos = 0;
  return 0;
}

size_t kr_cbor_position(kr_cbor *cbor) {
  if (cbor == NULL) return 0;
  return cbor->pos;
}

size_t kr_cbor_space(kr_cbor *cbor) {
  if (cbor == NULL) return 0;
  return cbor->len - cbor->pos;
}

static void cbor_advance(kr_cbor *cbor, int cnt) {
  cbor->pos += cnt;
  cbor->cur += cnt;
}

static void rmemcpy2(uint8_t *dst, uint8_t *src) {
  dst[0] = src[1];
  dst[1] = src[0];
}

static void rmemcpy4(uint8_t *dst, uint8_t *src) {
  dst[0] = src[3];
  dst[1] = src[2];
  dst[2] = src[1];
  dst[3] = src[0];
}

static void rmemcpy8(uint8_t *dst, uint8_t *src) {
  dst[0] = src[7];
  dst[1] = src[6];
  dst[2] = src[5];
  dst[3] = src[4];
  dst[4] = src[3];
  dst[5] = src[2];
  dst[6] = src[1];
  dst[7] = src[0];
}

static int kr_cbor_pack_nint64(kr_cbor *cbor, int64_t number) {
  uint64_t unumber;
  if (cbor == NULL) return -1;
  if (kr_cbor_space(cbor) < 9) return -2;
  cbor->cur[0] = 0x3b;
  cbor_advance(cbor, 1);
  unumber = number >> 63;
  unumber ^= number;
  rmemcpy8(cbor->cur, (uint8_t *)&unumber);
  cbor_advance(cbor, 8);
  return 9;
}

int kr_cbor_pack_int64(kr_cbor *cbor, int64_t number) {
  if (number < 0) {
    return kr_cbor_pack_nint64(cbor, number);
  } else {
    return kr_cbor_pack_uint64(cbor, number);
  }
}

int kr_cbor_pack_uint8(kr_cbor *cbor, uint8_t number) {
  if (cbor == NULL) return -1;
  if (kr_cbor_space(cbor) < 2) return -2;
  cbor->cur[0] = 0x18;
  cbor_advance(cbor, 1);
  cbor->cur[0] = number;
  cbor_advance(cbor, 1);
  return 2;
}

int kr_cbor_pack_uint16(kr_cbor *cbor, uint16_t number) {
  if (cbor == NULL) return -1;
  if (kr_cbor_space(cbor) < 3) return -2;
  cbor->cur[0] = 0x19;
  cbor_advance(cbor, 1);
  rmemcpy2(cbor->cur, (uint8_t *)&number);
  cbor_advance(cbor, 2);
  return 3;
}

int kr_cbor_pack_uint32(kr_cbor *cbor, uint32_t number) {
  if (cbor == NULL) return -1;
  if (kr_cbor_space(cbor) < 5) return -2;
  cbor->cur[0] = 0x1a;
  cbor_advance(cbor, 1);
  rmemcpy4(cbor->cur, (uint8_t *)&number);
  cbor_advance(cbor, 4);
  return 5;
}

int kr_cbor_pack_uint64(kr_cbor *cbor, uint64_t number) {
  if (cbor == NULL) return -1;
  if (kr_cbor_space(cbor) < 9) return -2;
  cbor->cur[0] = 0x1b;
  cbor_advance(cbor, 1);
  rmemcpy8(cbor->cur, (uint8_t *)&number);
  cbor_advance(cbor, 8);
  return 9;
}
