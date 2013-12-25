#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_compositor_common.h"
int kr_compositor_path_type_to_text(char *text, void *st, int32_t max);
int kr_compositor_subunit_type_to_text(char *text, void *st, int32_t max);
int kr_vector_type_to_text(char *text, void *st, int32_t max);
int kr_compositor_control_to_text(char *text, void *st, int32_t max);
int kr_compositor_controls_to_text(char *text, void *st, int32_t max);
int kr_sprite_info_to_text(char *text, void *st, int32_t max);
int kr_text_info_to_text(char *text, void *st, int32_t max);
int kr_vector_info_to_text(char *text, void *st, int32_t max);
int kr_compositor_path_info_to_text(char *text, void *st, int32_t max);
int kr_compositor_info_to_text(char *text, void *st, int32_t max);
