#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_sfx_common.h"
int kr_sfx_control_to_text(char *text, void *st, int32_t max);
int kr_sfx_effect_type_to_text(char *text, void *st, int32_t max);
int kr_sfx_effect_control_to_text(char *text, void *st, int32_t max);
int kr_eq_band_info_to_text(char *text, void *st, int32_t max);
int kr_eq_info_to_text(char *text, void *st, int32_t max);
int kr_lowpass_info_to_text(char *text, void *st, int32_t max);
int kr_highpass_info_to_text(char *text, void *st, int32_t max);
int kr_analog_info_to_text(char *text, void *st, int32_t max);
