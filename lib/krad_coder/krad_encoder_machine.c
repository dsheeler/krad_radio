#include "krad_encoder_machine.h"

typedef struct kr_encoder_msg_St kr_encoder_msg_t;

typedef enum {
  DOENCODE,
  ENCODERDESTROY
} kr_encoder_cmd_t;

struct kr_encoder_msg_St {
  kr_encoder_cmd_t cmd;
  union {
    float real;
    int64_t integer;
  } param;
};

struct kr_encoder_machine_St {
  kr_encoder2_t *encoder;
  kr_machine_t *machine;
};

static int kr_encoder_machine_process (void *msgin, void *actual) {

  kr_encoder2_t *encoder;
  kr_encoder_msg_t *msg;

  msg = (kr_encoder_msg_t *)msgin;
  encoder = (kr_encoder2_t *)actual;

  printf ("kr_encoder_process cmd %p\n", encoder);

  switch (msg->cmd) {
    case DOENCODE:
      printf ("Got ENCODE command!\n");
      return 1;
    case ENCODERDESTROY:
      printf ("Got ENCODERDESTROY command!\n");
  }

  return 0;
}

int kr_encoder_machine_destroy (kr_encoder_machine_t **encoder_machine) {
  kr_encoder_msg_t msg;
  if ((encoder_machine == NULL) || (*encoder_machine == NULL)) {
    return -1;
  }
  printf ("kr_encoder_machine_destroy()!\n");
  msg.cmd = ENCODERDESTROY;
  krad_machine_msg ((*encoder_machine)->machine, &msg);
  krad_machine_destroy (&(*encoder_machine)->machine);
  kr_encoder_destroy_direct (&(*encoder_machine)->encoder);
  return 0;
}

kr_encoder_machine_t *kr_encoder_machine_create () {
  
  kr_encoder_machine_t *encoder_machine;
  kr_machine_params_t machine_params;

  encoder_machine = calloc (1, sizeof(kr_encoder_machine_t));
  memset (&machine_params, 0, sizeof(kr_machine_params_t));

  encoder_machine->encoder = kr_encoder_create_direct ();

  machine_params.actual = encoder_machine->encoder;
  machine_params.msg_sz = sizeof (kr_encoder_msg_t);
  machine_params.process = kr_encoder_machine_process;

  encoder_machine->machine = krad_machine_create (&machine_params);

  return encoder_machine;
};

int kr_encoder_machine_encode (kr_encoder_machine_t *encoder_machine) {
  kr_encoder_msg_t msg;
  if (encoder_machine == NULL) {
    return -1;
  }
  printf ("kr_encoder_machine_encode()!\n");
  msg.cmd = DOENCODE;
  krad_machine_msg (encoder_machine->machine, &msg);
  return 0;
}
