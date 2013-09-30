#include "krad_radio.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [station sysname]\n A sysname is just a "
     "shortname.", argv[0]);
    exit(1);
  } else {
    kr_radio_daemon(argv[1]);
  }
  return 0;
}
