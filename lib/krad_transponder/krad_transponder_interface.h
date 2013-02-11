#include "krad_radio.h"
#include "krad_radio_client.h"

#ifndef KRAD_TRANSPONDER_INTERFACE_H
#define KRAD_TRANSPONDER_INTERFACE_H


void krad_transponder_link_to_ebml ( krad_ipc_server_client_t *client, krad_link_t *krad_link );
int krad_transponder_handler ( krad_transponder_t *krad_transponder, krad_ipc_server_t *krad_ipc );

#endif
