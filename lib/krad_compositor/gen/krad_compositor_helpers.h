#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_compositor_common.h"
int kr_compositor_controls_init(void *st);
int kr_compositor_controls_valid(void *st);
int kr_compositor_controls_random(void *st);
int kr_sprite_info_init(void *st);
int kr_sprite_info_valid(void *st);
int kr_sprite_info_random(void *st);
int kr_text_info_init(void *st);
int kr_text_info_valid(void *st);
int kr_text_info_random(void *st);
int kr_vector_info_init(void *st);
int kr_vector_info_valid(void *st);
int kr_vector_info_random(void *st);
int kr_compositor_path_info_init(void *st);
int kr_compositor_path_info_valid(void *st);
int kr_compositor_path_info_random(void *st);
int kr_compositor_info_init(void *st);
int kr_compositor_info_valid(void *st);
int kr_compositor_info_random(void *st);
int kr_compositor_path_type_to_index(int val);
int kr_strto_kr_compositor_path_type(char *string);
char *kr_strfr_kr_compositor_path_type(int val);
int kr_vector_type_to_index(int val);
int kr_strto_kr_vector_type(char *string);
char *kr_strfr_kr_vector_type(int val);
