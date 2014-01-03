#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_jack_common.h"
int kr_jack_direction_to_json(char *json, void *st, int32_t max);
int kr_jack_state_to_json(char *json, void *st, int32_t max);
int kr_jack_setup_info_to_json(char *json, void *st, int32_t max);
int kr_jack_info_to_json(char *json, void *st, int32_t max);
int kr_jack_path_info_to_json(char *json, void *st, int32_t max);
