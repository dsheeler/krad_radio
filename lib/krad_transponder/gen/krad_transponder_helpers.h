#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_adapter_common.h"
#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_transponder_common.h"
int kr_adapter_api_info_init(void *st, int idx);
int kr_adapter_api_info_valid(void *st, int idx);
int kr_adapter_api_info_random(void *st, int idx);
int kr_adapter_api_path_info_init(void *st, int idx);
int kr_adapter_api_path_info_valid(void *st, int idx);
int kr_adapter_api_path_info_random(void *st, int idx);
int kr_adapter_info_init(void *st);
int kr_adapter_info_valid(void *st);
int kr_adapter_info_random(void *st);
int kr_adapter_path_info_init(void *st);
int kr_adapter_path_info_valid(void *st);
int kr_adapter_path_info_random(void *st);
int kr_adapter_path_direction_to_index(int val);
int kr_strto_kr_adapter_path_direction(char *string);
char *kr_strfr_kr_adapter_path_direction(int val);
int kr_adapter_api_to_index(int val);
int kr_strto_kr_adapter_api(char *string);
char *kr_strfr_kr_adapter_api(int val);
int kr_adapter_api_to_index(int val);
int kr_strto_kr_adapter_api(char *string);
char *kr_strfr_kr_adapter_api(int val);
int kr_transponder_info_init(void *st);
int kr_transponder_info_valid(void *st);
int kr_transponder_info_random(void *st);
int kr_transponder_path_io_path_info_init(void *st, int idx);
int kr_transponder_path_io_path_info_valid(void *st, int idx);
int kr_transponder_path_io_path_info_random(void *st, int idx);
int kr_transponder_path_io_info_init(void *st);
int kr_transponder_path_io_info_valid(void *st);
int kr_transponder_path_io_info_random(void *st);
int kr_transponder_path_info_init(void *st);
int kr_transponder_path_info_valid(void *st);
int kr_transponder_path_info_random(void *st);
int kr_txpdr_su_type_t_to_index(int val);
int kr_strto_kr_txpdr_su_type_t(char *string);
char *kr_strfr_kr_txpdr_su_type_t(int val);
int kr_transponder_path_io_type_to_index(int val);
int kr_strto_kr_transponder_path_io_type(char *string);
char *kr_strfr_kr_transponder_path_io_type(int val);
