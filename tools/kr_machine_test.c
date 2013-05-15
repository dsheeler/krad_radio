#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>

#include "krad_system.h"
#include "krad_encoder_machine.h"

#include "krad_debug.c"

typedef struct kr_test_machine_St kr_test_machine_t;

struct kr_test_machine_St {
  int yay;
};

int encoded_callback (kr_encoder_machine_t *encoder_machine, kr_em_sector_t *sector, void *user) {
  
  kr_test_machine_t *test_machine;
  
  test_machine = (kr_test_machine_t *)user;
  
  printf ("Encoded callback!\n");
  
  kr_medium_kludge_destroy (&sector->medium);
  kr_codeme_kludge_destroy (&sector->codeme);
  
  return 0;
}


int main (int argc, char *argv[]) {

//  krad_debug_init ("machine_test");

  kr_test_machine_t *test_machine;
  kr_encoder_machine_t *encoder_machine;
  kr_encoder_machine_sector_t sector;
  int ret;
  int i;

  test_machine = calloc (1, sizeof(kr_test_machine_t));

  encoder_machine = kr_encoder_machine_create (encoded_callback, test_machine);
  
  // set encoder params: bitrate, kf int, threads, deadline etc
  
  // encode medium - codeme - callback?
  
  for (i = 0; i < 10; i++) {
    sector.medium = kr_medium_kludge_create ();
    sector.codeme = kr_codeme_kludge_create ();
    kr_encoder_machine_encode (encoder_machine, &sector);
  }
  
  ret = kr_encoder_machine_destroy (&encoder_machine);
  if (ret != 0) {
    printf ("Encoder machine destroy problemo!\n");
  }

  free (test_machine);

 // krad_debug_shutdown ();

  return 0;
}
