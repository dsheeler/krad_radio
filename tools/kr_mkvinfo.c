#include <stdio.h>
#include <unistd.h>

#include <krad_mkv_demux.h>

void krad_debug_init () {

  char logfile[256];

  krad_system_init ();
   
  sprintf (logfile, "%s/kr_mkvinfo_%"PRIu64".log",
           getenv ("HOME"), krad_unixtime ());

  krad_system_log_on (logfile);

  printf ("Logging to: %s\n", logfile);
}

int main (int argc, char *argv[]) {

  int32_t ret;
  kr_mkv_t *mkv;

  krad_debug_init ();
 
  mkv = kr_mkv_open_file (argv[1]);
 
  if (mkv == NULL) {
    printf ("Error opening %s\n", argv[1]);
  }

  if (mkv != NULL) {
    ret = kr_mkv_destroy (&mkv);
    if (ret < 0) {
      printf ("Error closing %s\n", argv[1]);
    }
  }

  return 0;
}
