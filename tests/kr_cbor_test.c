#include "krad_cbor.h"

void cbor_buffer_print(kr_cbor *cbor) {

  size_t i;
  size_t len;
  uint8_t *buffer;

  if (cbor == NULL) {
    fprintf(stderr, "cbor_buffer_print called with null kr_cbor\n");
    return;
  }
  len = kr_cbor_position(cbor);
  if (len == 0) {
    printf("cbor buffer is empty\n");
    return;
  }
  buffer = kr_cbor_buffer(cbor);
  printf("CBOR Buffer (%zu bytes):\n", len);
  for (i = 0; i < len; i++) {
    printf("%02X", buffer[i]);
  }
  printf("\n");
}

void cbor_state_print(kr_cbor *cbor) {

  size_t len;

  if (cbor == NULL) {
    fprintf(stderr, "cbor_state_print called with null kr_cbor\n");
    return;
  }

  len = kr_cbor_space(cbor);
  printf("kr_cbor_space ret: %zu\n", len);

  cbor_buffer_print(cbor);
}

void cbor_pack_test(kr_cbor *cbor) {

  int ret;

  ret = kr_cbor_pack_uint32(cbor, 1000000);
  printf("kr_cbor_pack_uint32 ret: %d\n", ret);
  cbor_state_print(cbor);

  ret = kr_cbor_pack_uint8(cbor, 100);
  printf("kr_cbor_pack_uint8 ret: %d\n", ret);
  cbor_state_print(cbor);

  ret = kr_cbor_pack_uint16(cbor, 1000);
  printf("kr_cbor_pack_uint16 ret: %d\n", ret);
  cbor_state_print(cbor);

  ret = kr_cbor_pack_uint32(cbor, 1000000);
  printf("kr_cbor_pack_uint32 ret: %d\n", ret);
  cbor_state_print(cbor);

  ret = kr_cbor_pack_uint64(cbor, 1000000000000);
  printf("kr_cbor_pack_uint64 ret: %d\n", ret);
  cbor_state_print(cbor);
}

void cbor_pack_test2(kr_cbor *cbor) {

  int ret;

  ret = kr_cbor_pack_int64(cbor, -1000);
  printf("kr_cbor_pack_int64 ret: %d\n", ret);
  cbor_state_print(cbor);
}

int test_kr_cbor() {

  int ret;
  void *buffer;
  size_t buffer_size;
  kr_cbor *cbor;

  buffer_size = 1024;
  ret = 0;

  cbor = kr_cbor_create();
  if (cbor == NULL) return -1;

  buffer = malloc(buffer_size);
  ret = kr_cbor_buffer_set(cbor, buffer, buffer_size);
  printf("kr_cbor_buffer_set ret: %d\n", ret);
  cbor_state_print(cbor);

  cbor_pack_test(cbor);

  ret = kr_cbor_buffer_reset(cbor);
  printf("kr_cbor_buffer_reset ret: %d\n", ret);
  cbor_state_print(cbor);

  cbor_pack_test2(cbor);

  ret = kr_cbor_free(cbor);
  printf("kr_cbor_free ret: %d\n", ret);

  free(buffer);

  return ret;
}

int main (int argc, char *argv[]) {

  int ret;

  ret = test_kr_cbor();

  return ret;
}
