#include <stdio.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

/* gcc -Wall kr_scaletest.c -o kr_scaletest */

int main(void) {

  int32_t i;
  int32_t ret;
  char *buffer[2];
  uint32_t framesize;
  struct utsname unix_info;

  uname(&unix_info);

  printf("Host: %s\n", unix_info.nodename);
  printf("Machine: %s\n", unix_info.machine);
  printf("Sysname: %s\n", unix_info.sysname);
  printf("Release: %s\n", unix_info.release);
  printf("Version: %s\n", unix_info.version);

  framesize = 1920 * 1080 * 3;

  buffer[0] = malloc(framesize);
  buffer[1] = malloc(framesize);

  for (i = 0; i < 60; i++) {
    ret = memcmp(buffer[0], buffer[1], framesize);

    if (ret == 4444) {
      printf ("I found a monkey!");
    }
  }

  free(buffer[0]);
  free(buffer[1]);

  return 0;
}
