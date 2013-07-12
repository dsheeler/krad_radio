#include "krad_mixer.h"
#include "krad_radio_interface.h"

void kr_mixer_unit_to_rep(kr_mixer_unit *unit, kr_mxr_unit_rep *unit_rep);
int kr_mixer_command(kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client);
