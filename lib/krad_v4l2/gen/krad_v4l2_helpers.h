#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_v4l2_common.h"
int kr_v4l2_mode_init(struct kr_v4l2_mode *st);
int kr_v4l2_mode_valid(struct kr_v4l2_mode *st);
int kr_v4l2_mode_random(struct kr_v4l2_mode *st);
int kr_v4l2_info_init(struct kr_v4l2_info *st);
int kr_v4l2_info_valid(struct kr_v4l2_info *st);
int kr_v4l2_info_random(struct kr_v4l2_info *st);
int kr_v4l2_open_info_init(struct kr_v4l2_open_info *st);
int kr_v4l2_open_info_valid(struct kr_v4l2_open_info *st);
int kr_v4l2_open_info_random(struct kr_v4l2_open_info *st);
int kr_v4l2_state_to_index(int val);
int kr_strto_kr_v4l2_state(char *string);
char *kr_strfr_kr_v4l2_state(int val);
