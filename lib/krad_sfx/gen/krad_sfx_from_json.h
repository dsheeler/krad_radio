#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "jsmn.h"
#include "krad_sfx_common.h"
int kr_sfx_control_fr_json(char *json, void *st);
int kr_sfx_effect_type_fr_json(char *json, void *st);
int kr_sfx_effect_control_fr_json(char *json, void *st);
int kr_eq_band_info_fr_json(char *json, void *st);
int kr_eq_info_fr_json(char *json, void *st);
int kr_lowpass_info_fr_json(char *json, void *st);
int kr_highpass_info_fr_json(char *json, void *st);
int kr_analog_info_fr_json(char *json, void *st);
