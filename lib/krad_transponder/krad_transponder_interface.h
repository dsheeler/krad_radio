#ifndef KRAD_TRANSPONDER_INTERFACE_H
#define KRAD_TRANSPONDER_INTERFACE_H

#include "krad_radio.h"
#include "krad_radio_client.h"
#include "krad_radio_interface.h"
#include "krad_transponder_common.h"

int kr_transponder_cmd(kr_io2_t *in, kr_io2_t *out, kr_radio_client *client);

#endif
