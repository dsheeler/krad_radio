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

int main (int argc, char *argv[]) {

//  krad_debug_init ("machine_test");
  
  kr_encoder_machine_t *encoder_machine;
  int ret;

usleep (250000);
  
  encoder_machine = kr_encoder_machine_create ();
  
  kr_encoder_machine_encode (encoder_machine);
  kr_encoder_machine_encode (encoder_machine);
  kr_encoder_machine_encode (encoder_machine);
usleep (1000000);
  kr_encoder_machine_encode (encoder_machine);
  kr_encoder_machine_encode (encoder_machine);
  kr_encoder_machine_encode (encoder_machine);
  kr_encoder_machine_encode (encoder_machine);
  
  
  ret = kr_encoder_machine_destroy (&encoder_machine);
  if (ret != 0) {
    printf ("Encoder machine destroy problemo!\n");
  }

 // krad_debug_shutdown ();

  return 0;
}
