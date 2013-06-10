#include "krad_base64.h"

void kr_base64_encode (char *dest, char *src, int maxlen) {
  kr_base64 ((uint8_t *)dest, (uint8_t *)src, strlen(src), maxlen);
}

int32_t kr_base64 (uint8_t *dest, uint8_t *src, int len, int maxlen) {

  char b64t[64] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
                    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
                    '8', '9', '+', '/' };
  int32_t base64_len;
  int32_t slice;
  char *out;
  char *result;
  char buffer[1024];

  base64_len = len * 4 / 3 + 4;
  out = buffer;
  result = out;
  
  if ((dest == NULL) || (base64_len >= 1024) || (base64_len >= maxlen)) {
    return -1;
  }

  while (len > 0) {
   slice = (len > 3) ? 3 : len;
    *out++ = b64t[(*src & 0xFC) >> 2];
    *out++ = b64t[((*src & 0x03) << 4) | ((*(src + 1) & 0xF0) >> 4)];
    switch (slice) {
      case 3:
        *out++ = b64t[((*(src + 1) & 0x0F) << 2) | ((*(src + 2) & 0xC0) >> 6)];
        *out++ = b64t[(*(src + 2)) & 0x3F];
        break;

      case 2:
        *out++ = b64t[((*(src + 1) & 0x0F) << 2)];
        *out++ = '=';
        break;

      case 1:
        *out++ = '=';
        *out++ = '=';
        break;
    }
    src += slice;
    len -= slice;
  }
  *out = 0;
  memcpy(dest, result, base64_len);
  return base64_len;
}
