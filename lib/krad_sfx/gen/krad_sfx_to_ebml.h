#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_ebmlx.h"
#include "krad_sfx_common.h"
int kr_sfx_control_to_ebml(kr_ebml *ebml, void *st);
int kr_sfx_effect_type_to_ebml(kr_ebml *ebml, void *st);
int kr_sfx_effect_control_to_ebml(kr_ebml *ebml, void *st);
int kr_eq_band_info_to_ebml(kr_ebml *ebml, void *st);
int kr_eq_info_to_ebml(kr_ebml *ebml, void *st);
int kr_lowpass_info_to_ebml(kr_ebml *ebml, void *st);
int kr_highpass_info_to_ebml(kr_ebml *ebml, void *st);
int kr_analog_info_to_ebml(kr_ebml *ebml, void *st);
