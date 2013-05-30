#include <sys/types.h>
#include <sys/stat.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <string.h>
#include <inttypes.h>

#ifndef KRAD_BASE64_H
#define KRAD_BASE64_H
void kr_base64_encode(char *dest, char *src, int maxlen);
void kr_base64(char *dest, char *src, int len, int maxlen);
#endif
