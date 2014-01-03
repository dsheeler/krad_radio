#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_v4l2_common.h"
int kr_v4l2_mode_init(void *st);
int kr_v4l2_mode_valid(void *st);
int kr_v4l2_mode_random(void *st);
int kr_v4l2_info_init(void *st);
int kr_v4l2_info_valid(void *st);
int kr_v4l2_info_random(void *st);
int kr_v4l2_open_info_init(void *st);
int kr_v4l2_open_info_valid(void *st);
int kr_v4l2_open_info_random(void *st);
int kr_v4l2_state_to_index(int val);
int kr_strto_kr_v4l2_state(char *string);
char *kr_strfr_kr_v4l2_state(int val);
