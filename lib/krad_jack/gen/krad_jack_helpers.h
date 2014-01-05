#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_jack_common.h"
int kr_jack_setup_info_init(void *st);
int kr_jack_setup_info_valid(void *st);
int kr_jack_setup_info_random(void *st);
int kr_jack_info_init(void *st);
int kr_jack_info_valid(void *st);
int kr_jack_info_random(void *st);
int kr_jack_path_info_init(void *st);
int kr_jack_path_info_valid(void *st);
int kr_jack_path_info_random(void *st);
int kr_jack_direction_to_index(int val);
int kr_strto_kr_jack_direction(char *string);
char *kr_strfr_kr_jack_direction(int val);
int kr_jack_state_to_index(int val);
int kr_strto_kr_jack_state(char *string);
char *kr_strfr_kr_jack_state(int val);
