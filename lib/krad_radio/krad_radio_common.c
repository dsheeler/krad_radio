#include "krad_radio_common.h"


kr_radio_t *kr_radio_rep_create () {
  kr_radio_t *radio_rep;
  radio_rep = calloc (1, sizeof (kr_radio_t));
  return radio_rep;
}

void kr_radio_rep_destroy (kr_radio_t *radio_rep) {
  free (radio_rep);
}


kr_remote_t *kr_remote_rep_create_with (char *interface, int port) {
  kr_remote_t *remote_rep;
  remote_rep = kr_remote_rep_create ();
  remote_rep->port = port;
  strncpy (remote_rep->interface, interface, sizeof(remote_rep->interface));
  return remote_rep;
}

kr_remote_t *kr_remote_rep_create () {
  kr_remote_t *remote_rep;
  remote_rep = calloc (1, sizeof (kr_remote_t));
  return remote_rep;
}

void kr_remote_rep_destroy (kr_remote_t *remote_rep) {
  free (remote_rep);
}
