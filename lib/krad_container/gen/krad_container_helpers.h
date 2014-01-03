#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_codec_header.h"
int krad_codec_header_St_init(void *st);
int krad_codec_header_St_valid(void *st);
int krad_codec_header_St_random(void *st);
int krad_codec_t_to_index(int val);
int kr_strto_krad_codec_t(char *string);
char *kr_strfr_krad_codec_t(int val);
