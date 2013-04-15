#include "krad_demuxer.h"
#include "kr_client.h"

typedef struct kr_demuxer_msg_St kr_demuxer_msg_t;

static void kr_demuxer_start (void *actual);
static int32_t kr_demuxer_process (void *msgin, void *actual);
static void kr_demuxer_destroy_actual (void *actual);

struct kr_demuxer_msg_St {
  kr_demuxer_cmd_t cmd;
  union {
    float real;
    int64_t integer;
  } param;
};

struct kr_demuxer_St {
  kr_demuxer_params_t params;
  int64_t position;
  kr_demuxer_state_t state;
  kr_direction_t direction;
  krad_container_t *input;
  kr_machine_t *machine;  
};

/* Private Functions */

static void kr_demuxer_step (kr_demuxer_t *demuxer) {

  kr_packet_t packet;
  int track;
  uint64_t timecode;
  int ret;
  int size;
  uint8_t *buffer;
  
  buffer = malloc (2000000);
  
  while (1) {
    size = krad_container_read_packet (demuxer->input, &track,
                                      &timecode, buffer);
    if (size < 1) {
      break;
    }
    packet.size = size;
    packet.buffer = buffer;
    packet.track = track;
    ret = demuxer->params.packet_cb (&packet, demuxer->params.controller);
    if (ret != 1) {
      break;
    }
  }

  free (buffer);
}

static int32_t kr_demuxer_process (void *msgin, void *actual) {

  kr_demuxer_t *demuxer;
  kr_demuxer_msg_t *msg;

  msg = (kr_demuxer_msg_t *)msgin;
  demuxer = (kr_demuxer_t *)actual;

  //printf ("kr_demuxer_process cmd %d\n", msg->cmd);

  switch (msg->cmd) {
    case DMSEEK:
      printf ("Got DMSEEK command!\n");
      break;
    case SETDMDIR:
      printf ("Got SETDMDIR command!\n");
      break; 
    case DMPAUSE:
      printf ("Got DMPAUSE command!\n");
      demuxer->state = DMCUED;
      demuxer->params.status_cb (demuxer->state, demuxer->params.controller);
      break;      
    case ROLL:
      printf ("Got ROLL command!\n");
      if (demuxer->state == DMCUED) {
        demuxer->state = DEMUXING;
      }
      if (demuxer->state == DEMUXING) {
        kr_demuxer_step (demuxer);
      }
      break;
    case DEMUXERDESTROY:
      printf ("Got DEMUXERDESTROY command!\n");
      krad_container_destroy (&demuxer->input);
      return 0;
  }
  
  //printf ("kr_demuxer_process done\n");  
  
  return 1;
}

static void kr_demuxer_destroy_actual (void *actual) {

  printf ("kr_demuxer_destroy_actual()!\n");

  kr_demuxer_t *demuxer;

  demuxer = (kr_demuxer_t *)actual;

  krad_container_destroy (&demuxer->input);
  free (demuxer->params.url);
}

static void kr_demuxer_start (void *actual) {
  
  kr_demuxer_t *demuxer;

  demuxer = (kr_demuxer_t *)actual;

  demuxer->input = krad_container_open_file (demuxer->params.url,
                                             KRAD_IO_READONLY);
  if (demuxer->input != NULL) {
    demuxer->state = DMCUED;
  }
  demuxer->params.status_cb (demuxer->state, demuxer->params.controller);

  printf ("kr_demuxer_start()!\n");
}

/* Public Functions */

void kr_demuxer_destroy (kr_demuxer_t **demuxer) {
  kr_demuxer_msg_t msg;
  if ((demuxer != NULL) && (*demuxer != NULL)) {
    printf ("kr_demuxer_destroy()!\n");
    msg.cmd = DEMUXERDESTROY;
    krad_machine_msg ((*demuxer)->machine, &msg);
    krad_machine_destroy (&(*demuxer)->machine);
    free (*demuxer);
    *demuxer = NULL;
  }
}

kr_demuxer_t *kr_demuxer_create (kr_demuxer_params_t *demuxer_params) {
  
  kr_demuxer_t *demuxer;
  kr_machine_params_t machine_params;

  demuxer = calloc (1, sizeof(kr_demuxer_t));

  demuxer->params.url = strdup (demuxer_params->url);
  demuxer->params.controller = demuxer_params->controller;
  demuxer->params.status_cb = demuxer_params->status_cb;  
  demuxer->params.packet_cb = demuxer_params->packet_cb;

  demuxer->direction = FORWARD;
  demuxer->state = DMIDLE;

  machine_params.actual = demuxer;
  machine_params.msg_sz = sizeof (kr_demuxer_msg_t);
  machine_params.start = kr_demuxer_start;
  machine_params.process = kr_demuxer_process;
  machine_params.destroy = kr_demuxer_destroy_actual;

  demuxer->machine = krad_machine_create (&machine_params);
  
  return demuxer;
};

kr_direction_t kr_demuxer_direction_get (kr_demuxer_t *demuxer) {
  return demuxer->direction;
}

int64_t kr_demuxer_position_get (kr_demuxer_t *demuxer) {
  return demuxer->position;
}

kr_demuxer_state_t kr_demuxer_state_get (kr_demuxer_t *demuxer) {
  return demuxer->state;
}

void kr_demuxer_direction_set (kr_demuxer_t *demuxer, kr_direction_t direction) {
  kr_demuxer_msg_t msg;
  msg.cmd = SETDMDIR;
  msg.param.integer = direction;
  krad_machine_msg (demuxer->machine, &msg);
}

void kr_demuxer_seek (kr_demuxer_t *demuxer, int64_t position) {
  kr_demuxer_msg_t msg;
  msg.cmd = DMSEEK;
  msg.param.integer = position;
  krad_machine_msg (demuxer->machine, &msg);
}

void kr_demuxer_roll (kr_demuxer_t *demuxer) {
  kr_demuxer_msg_t msg;
  msg.cmd = ROLL;
  krad_machine_msg (demuxer->machine, &msg);
}

void kr_demuxer_pause (kr_demuxer_t *demuxer) {
  kr_demuxer_msg_t msg;
  msg.cmd = DMPAUSE;
  krad_machine_msg (demuxer->machine, &msg);
}
