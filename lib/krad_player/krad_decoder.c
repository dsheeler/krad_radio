#include "krad_decoder.h"
#include "kr_client.h"

typedef struct kr_decoder_msg_St kr_decoder_msg_t;

static void kr_decoder_start (void *actual);
static int kr_decoder_process (void *msgin, void *actual);
static void kr_decoder_destroy_actual (void *actual);

struct kr_decoder_msg_St {
  kr_decoder_cmd_t cmd;
  union {
    float real;
    int64_t integer;
  } param;
};

struct kr_decoder_St {
  //float speed;
  //int64_t position;
  //kr_direction_t direction;

  kr_decoder_state_t state;

  kr_machine_t *machine;  
};

/* Private Functions */

static int kr_decoder_process (void *msgin, void *actual) {

  kr_decoder_t *decoder;
  kr_decoder_msg_t *msg;

  msg = (kr_decoder_msg_t *)msgin;
  decoder = (kr_decoder_t *)actual;

  printf ("kr_decoder_process cmd %p\n", decoder);

  //printf ("kr_decoder_process cmd %d\n", msg->cmd);

  switch (msg->cmd) {
    case DODECODE:
      printf ("Got DECODE command!\n");
      break;
    case DECODERDESTROY:
      printf ("Got DECODERDESTROY command!\n");
      return 0;
  }
  
  //printf ("kr_decoder_process done\n");  
  
  return 1;
}

static void kr_decoder_destroy_actual (void *actual) {

  kr_decoder_t *decoder;

  decoder = (kr_decoder_t *)actual;

  printf ("kr_decoder_destroy_actual cmd %p\n", decoder);
}

static void kr_decoder_start (void *actual) {

  kr_decoder_t *decoder;

  decoder = (kr_decoder_t *)actual;

  decoder->state = DEIDLE;
  printf ("kr_decoder_start()!\n");
}

/* Public Functions */

void kr_decoder_destroy (kr_decoder_t **decoder) {
  kr_decoder_msg_t msg;
  if ((decoder != NULL) && (*decoder != NULL)) {
    printf ("kr_decoder_destroy()!\n");
    msg.cmd = DECODERDESTROY;
    krad_machine_msg ((*decoder)->machine, &msg);
    krad_machine_destroy (&(*decoder)->machine);
    free (*decoder);
    *decoder = NULL;
  }
}

kr_decoder_t *kr_decoder_create () {
  
  kr_decoder_t *decoder;
  kr_machine_params_t machine_params;

  decoder = calloc (1, sizeof(kr_decoder_t));

  machine_params.actual = decoder;
  machine_params.msg_sz = sizeof (kr_decoder_msg_t);
  machine_params.start = kr_decoder_start;
  machine_params.process = kr_decoder_process;
  machine_params.destroy = kr_decoder_destroy_actual;

  decoder->machine = krad_machine_create (&machine_params);
  
  return decoder;
};

kr_decoder_state_t kr_decoder_state_get (kr_decoder_t *decoder) {
  return decoder->state;
}

/*
float kr_decoder_speed_get (kr_decoder_t *decoder) {
  return decoder->speed;
}

kr_direction_t kr_decoder_direction_get (kr_decoder_t *decoder) {
  return decoder->direction;
}

int64_t kr_decoder_position_get (kr_decoder_t *decoder) {
  return decoder->position;
}

void kr_decoder_speed_set (kr_decoder_t *decoder, float speed) {
  kr_decoder_msg_t msg;
  msg.cmd = SETSPEED;
  msg.param.real = roundf (speed*1000.0f)/1000.0f;
  krad_machine_msg (decoder->machine, &msg);
}

void kr_decoder_direction_set (kr_decoder_t *decoder, kr_direction_t direction) {
  kr_decoder_msg_t msg;
  msg.cmd = SETDIR;
  msg.param.integer = direction;
  krad_machine_msg (decoder->machine, &msg);
}

void kr_decoder_seek (kr_decoder_t *decoder, int64_t position) {
  kr_decoder_msg_t msg;
  msg.cmd = SEEK;
  msg.param.integer = position;
  krad_machine_msg (decoder->machine, &msg);
}

void kr_decoder_play (kr_decoder_t *decoder) {
  kr_decoder_msg_t msg;
  msg.cmd = PLAY;
  krad_machine_msg (decoder->machine, &msg);
}

void kr_decoder_pause (kr_decoder_t *decoder) {
  kr_decoder_msg_t msg;
  msg.cmd = PAUSE;
  krad_machine_msg (decoder->machine, &msg);
}

void kr_decoder_stop (kr_decoder_t *decoder) {
  kr_decoder_msg_t msg;
  msg.cmd = STOP;
  krad_machine_msg (decoder->machine, &msg);
}
*/
