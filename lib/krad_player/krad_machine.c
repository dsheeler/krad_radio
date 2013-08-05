#include "krad_machine.h"

static void *krad_machine_run (void *arg);

static void *krad_machine_run (void *arg) {

  kr_machine_t *machine;
  void *msg;
  int ret;

  machine = (kr_machine_t *)arg;
  krad_system_set_thread_name ("kr_machine");

  msg = calloc (1, machine->params.msg_sz);
  //printf ("kr_msg size %zu\n", machine->params.msg_sz);
  if (machine->params.start != NULL) {
    machine->params.start (machine->params.actual);
  }

  while (1) {
    ret = kr_msgsys_wait (machine->msgsys, msg);
    //printf ("kr_msgsys_wait ret %d\n", ret);
    if (ret == 0) {
      break;
    }
    ret = machine->params.process (msg, machine->params.actual);
    //printf ("machine->params.process ret %d\n", ret);
    if (ret == 0) {
      break;
    }
  }

  free (msg);

  if (machine->params.destroy != NULL) {
    machine->params.destroy (machine->params.actual);
  }

  return NULL;
}

void krad_machine_destroy (kr_machine_t **machine) {
  if ((machine != NULL) && (*machine != NULL)) {
    pthread_join ((*machine)->thread, NULL);
    kr_msgsys_destroy (&(*machine)->msgsys);
    free (*machine);
    *machine = NULL;
  }
}

kr_machine_t *krad_machine_create (kr_machine_params_t *params) {

  kr_machine_t *machine;

  machine = calloc (1, sizeof (kr_machine_t));
  memcpy (&machine->params, params, sizeof(kr_machine_params_t));
  machine->msgsys = kr_msgsys_create (2, machine->params.msg_sz);
  pthread_create (&machine->thread, NULL, krad_machine_run, (void *)machine);
  return machine;
}

void krad_machine_msg (kr_machine_t *machine, void *msg) {
  kr_msgsys_write (machine->msgsys, 0, msg);
}

void krad_machine_msg2 (kr_machine_t *machine, void *msg) {
  kr_msgsys_write (machine->msgsys, 1, msg);
}
