#ifndef KRAD_MIXER_SERVER_H
#define KRAD_MIXER_SERVER_H

#include "krad_mixer.h"
#include "krad_radio_server.h"

int kr_mixer_command(kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client);

#endif
