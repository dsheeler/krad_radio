#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_perspective.h"
int kr_pos_to_json(char *json, void *st, int32_t max);
int kr_perspective_view_to_json(char *json, void *st, int32_t max);
int kr_perspective_to_json(char *json, void *st, int32_t max);
