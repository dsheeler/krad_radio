#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_ebmlx.h"
#include "krad_jack_common.h"
int kr_jack_direction_fr_ebml(kr_ebml *ebml, void *st);
int kr_jack_state_fr_ebml(kr_ebml *ebml, void *st);
int kr_jack_setup_info_fr_ebml(kr_ebml *ebml, void *st);
int kr_jack_info_fr_ebml(kr_ebml *ebml, void *st);
int kr_jack_path_info_fr_ebml(kr_ebml *ebml, void *st);
