#include <stdio.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <stddef.h>

/* gcc -Wall kr_sysinfo.c -o kr_sysinfo */

int main (void) {

  struct utsname unix_info;

  uname (&unix_info);
  
  printf ("Host: %s\n", unix_info.nodename);
  printf ("Machine: %s\n", unix_info.machine);
  printf ("Sysname: %s\n", unix_info.sysname);
  printf ("Release: %s\n", unix_info.release);
  printf ("Version: %s\n", unix_info.version);

  printf ("sizeof short %zu\n", sizeof(short));
  printf ("sizeof int %zu\n", sizeof(int));
  printf ("sizeof float %zu\n", sizeof(float));
  printf ("sizeof long int %zu\n", sizeof(long int));
  printf ("sizeof long long %zu\n", sizeof(long long));
  printf ("sizeof size_t %zu\n", sizeof(size_t));
  printf ("sizeof ssize_t %zu\n", sizeof(ssize_t));
  printf ("sizeof off_t %zu\n", sizeof(off_t));
  printf ("sizeof ptrdiff_t %zu\n", sizeof(ptrdiff_t));
  printf ("sizeof wchar_t %zu\n", sizeof(wchar_t));
  printf ("sizeof time_t %zu\n", sizeof(time_t));

  return 0;
}
