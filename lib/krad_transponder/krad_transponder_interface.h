#include "krad_radio.h"
#include "krad_radio_client.h"
#include "krad_radio_interface.h"
#include "krad_transponder_common.h"

#ifndef KRAD_TRANSPONDER_INTERFACE_H
#define KRAD_TRANSPONDER_INTERFACE_H



int krad_transponder_command ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client );

#endif
