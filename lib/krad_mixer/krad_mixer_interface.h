#include "krad_mixer.h"
#include "krad_radio_interface.h"

void krad_mixer_portgroup_to_rep (krad_mixer_portgroup_t *portgroup,
                                  kr_portgroup_t *portgroup_rep);

int krad_mixer_command ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client );
