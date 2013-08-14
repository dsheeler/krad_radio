#include <stdio.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <stddef.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

/* gcc -Wall kr_strtest.c -o kr_strtest */

int main(void) {

  int32_t i;
  int ret;
  char *string1 = "kr_mixer";
  char *string2 = "kr_mixer_path_info";


  ret = strcmp(string1, string2);
  printf("strcmp string1 string2: %d\n", ret);
  ret = strcmp(string2, string1);
  printf("strcmp string2 string1: %d\n", ret);

  return 0;
}
