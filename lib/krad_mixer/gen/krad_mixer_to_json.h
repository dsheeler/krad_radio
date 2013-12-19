#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_mixer_common.h"
int kr_mixer_channels_to_json(char *json, void *st, int32_t max);
int kr_mixer_control_to_json(char *json, void *st, int32_t max);
int kr_mixer_path_type_to_json(char *json, void *st, int32_t max);
int kr_mixer_adv_ctl_to_json(char *json, void *st, int32_t max);
int kr_mixer_info_to_json(char *json, void *st, int32_t max);
int kr_mixer_path_info_to_json(char *json, void *st, int32_t max);
