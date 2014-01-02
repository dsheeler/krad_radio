#ifndef KRAD_MIXER_INTERFACE_H
#define KRAD_MIXER_INTERFACE_H

#include "krad_mixer.h"
#include "krad_radio_interface.h"

int kr_mixer_command(kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client);

#endif
