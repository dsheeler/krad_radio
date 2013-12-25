#include <stdio.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <stddef.h>
#ifdef __linux__
#include <sys/sysinfo.h>
#endif

/* gcc -Wall kr_sysinfo.c -o kr_sysinfo */

int main (int argc, char **argv) {

  struct utsname unix_info;
#ifdef __linux__
  int days, hours, mins;
  struct sysinfo sys_info;
#endif

  uname (&unix_info);

  printf("Host: %s\n", unix_info.nodename);
  printf("Machine: %s\n", unix_info.machine);
  printf("Sysname: %s\n", unix_info.sysname);
  printf("Release: %s\n", unix_info.release);
  printf("Version: %s\n", unix_info.version);

  printf("sizeof short %zu\n", sizeof(short));
  printf("sizeof int %zu\n", sizeof(int));
  printf("sizeof float %zu\n", sizeof(float));
  printf("sizeof long int %zu\n", sizeof(long int));
  printf("sizeof long long %zu\n", sizeof(long long));
  printf("sizeof size_t %zu\n", sizeof(size_t));
  printf("sizeof ssize_t %zu\n", sizeof(ssize_t));
  printf("sizeof off_t %zu\n", sizeof(off_t));
  printf("sizeof ptrdiff_t %zu\n", sizeof(ptrdiff_t));
  printf("sizeof wchar_t %zu\n", sizeof(wchar_t));
  printf("sizeof time_t %zu\n", sizeof(time_t));

#ifdef __linux__
  if (sysinfo(&sys_info) != 0) {
    fprintf(stderr, "Could not run sysinfo() command\n");
    return -1;
  }

  days = sys_info.uptime / 86400;
  hours = (sys_info.uptime / 3600) - (days * 24);
  mins = (sys_info.uptime / 60) - (days * 1440) - (hours * 60);

  printf("Uptime: %d days, %d hours, %d minutes, %ld seconds\n",
   days, hours, mins, sys_info.uptime % 60);

  printf("Load Avgs: 1min(%ld) 5min(%ld) 15min(%ld)\n",
   sys_info.loads[0], sys_info.loads[1], sys_info.loads[2]);

  printf("Total Ram: %ldk\tFree: %ldk\n", sys_info.totalram / 1024,
   sys_info.freeram / 1024);
#endif

  return 0;
}
