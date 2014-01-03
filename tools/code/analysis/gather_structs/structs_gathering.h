#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include "struct_info.h"

#define MAX_HEADERS 256
#define MAX_HEADER_DEFS 64
#define TARGET_TYPES 7
#define PATH_MAX_LEN 128
#define MAX_TARGETS 32
#define PATH_MAX_LEN 128

typedef struct cgen_target cgen_target;
typedef struct header_data header_data;

typedef enum {
  TO_TEXT = 1,
  TO_JSON,
  FR_JSON,
  TO_EBML,
  FR_EBML,
  HELPERS,
  CONFIG
} cgen_target_type;

struct cgen_target {
  cgen_target_type type;
  char path[PATH_MAX_LEN];
};

struct header_data {
  char path[PATH_MAX_LEN];
  cgen_target *targets;
  int target_count;
  struct_data *defs;
  int def_count;
};

int is_prefix (const char *str, const char *prefix);
int is_suffix (const char *str, const char *suffix);
member_type str_to_member_type(char *str);
char *member_type_to_str(member_type type);
int gather_struct_definitions(header_data *hdata, 
  char *fprefix, char *path);
int print_structs_defs(header_data *hdata, 
  char *prefix, char *suffix, char *format);
