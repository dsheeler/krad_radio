#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_v4l2_common.h"
int kr_v4l2_state_to_json(char *json, void *st, int32_t max);
int kr_v4l2_mode_to_json(char *json, void *st, int32_t max);
int kr_v4l2_info_to_json(char *json, void *st, int32_t max);
int kr_v4l2_open_info_to_json(char *json, void *st, int32_t max);
