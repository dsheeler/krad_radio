#ifndef KRAD_MIXER_INTERFACE_H
#define KRAD_MIXER_INTERFACE_H

#include "krad_mixer.h"
#include "krad_radio_interface.h"

void kr_mixer_unit_to_rep(kr_mixer_path *unit, kr_mixer_path_info *unit_rep);
int kr_mixer_command(kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client);

#endif
