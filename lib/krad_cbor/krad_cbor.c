#include "krad_cbor.h"

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

static int cbor_pack(kr_cbor *cbor) {
  cbor->cur[0] = 'X';
  cbor->pos += 1;
  cbor->cur += 1;
  return 1;
}

int kr_cbor_pack(kr_cbor *cbor) {
  if (cbor == NULL) return -1;
  if (kr_cbor_space(cbor) < 1) return -2;
  return cbor_pack(cbor);
}
