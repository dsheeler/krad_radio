#include "krad_encoder_machine.h"

typedef struct kr_encoder_msg_St kr_encoder_msg_t;

typedef enum {
  DOENCODE,
  ENCODERDESTROY
} kr_encoder_cmd_t;

struct kr_encoder_msg_St {
  kr_encoder_cmd_t cmd;
  kr_em_sector_t sector;
};

struct kr_encoder_machine_St {
  kr_encoder2_t *encoder;
  kr_machine_t *machine;
  kr_encoder_machine_process_cb process_cb;
  void *user;
};

static int kr_encoder_machine_process (void *msgin, void *actual) {

  kr_encoder_machine_t *encoder_machine;
  kr_encoder_msg_t *msg;

  msg = (kr_encoder_msg_t *)msgin;
  encoder_machine = (kr_encoder_machine_t *)actual;

  switch (msg->cmd) {
    case DOENCODE:
      printf ("Got ENCODE command!\n");
      
      //kr_encoder_encode_direct (encoder_machine->encoder,
      //                          msg->sector.medium,
      //                          msg->sector.codeme);
      
      encoder_machine->process_cb (encoder_machine, &msg->sector, encoder_machine->user);
      
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
  free (*encoder_machine);
  *encoder_machine = NULL;
  return 0;
}

kr_encoder_machine_t *kr_encoder_machine_create (kr_encoder_machine_process_cb process_callback, void *user) {
  
  kr_encoder_machine_t *encoder_machine;
  kr_machine_params_t machine_params;

  if ((process_callback == NULL) || (user == NULL)) {
    return NULL;
  }

  encoder_machine = calloc (1, sizeof(kr_encoder_machine_t));
  memset (&machine_params, 0, sizeof(kr_machine_params_t));

  encoder_machine->process_cb = process_callback;
  encoder_machine->user = user;
  encoder_machine->encoder = kr_encoder_create_direct ();

  machine_params.actual = encoder_machine;
  machine_params.msg_sz = sizeof (kr_encoder_msg_t);
  machine_params.process = kr_encoder_machine_process;

  encoder_machine->machine = krad_machine_create (&machine_params);

  return encoder_machine;
};

int kr_encoder_machine_encode (kr_encoder_machine_t *encoder_machine,
                               kr_em_sector_t *sector) {
  kr_encoder_msg_t msg;
  if (encoder_machine == NULL) {
    return -1;
  }
  printf ("kr_encoder_machine_encode()!\n");
  msg.cmd = DOENCODE;
  memcpy (&msg.sector, sector, sizeof(kr_em_sector_t));
  krad_machine_msg (encoder_machine->machine, &msg);
  return 0;
}
