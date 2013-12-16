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

#define MAX_HEADERS 512
#define MAX_DEFS 1024
#define MAX_DEF_LENGTH 2048
#define MAX_MEMB 64
#define MAX_TARGETS 32
#define MAX_HEADER_DEFS 64
#define MAX_TARGET_TYPES 5

typedef enum {
  MEMB_TYPE_UNKNOWN = 0,
  MEMB_TYPE_INT,
  MEMB_TYPE_UINT,
  MEMB_TYPE_FLOAT,
  MEMB_TYPE_STRING
} memb_type;

typedef enum {
  TO_TEXT = 1,
  TO_JSON,
  TO_EBML,
  FR_EBML,
  HELPERS,
  CONFIG
} cgen_target_type;

struct cgen_target {
  cgen_target_type types[MAX_TARGETS];
  char *targets[MAX_TARGETS];
  int ntargets;
};

struct memb_data_info {
  memb_type type;
  union {
    int int_info[3];
    float float_info[3];
  } info;
};

struct struct_memb_def {
  char *name;
  char *type;
  int array;
  char *array_str_val;
  int pointer;
  struct struct_def *sub;
  struct memb_data_info data_info;
};

struct struct_def {
  char *fullpath;
  char *name;
  char definition[MAX_DEF_LENGTH];
  struct struct_memb_def members_info[MAX_MEMB];
  int members;
  int line;
  int istypedef;
  int isunion;
  int isenum;
  int issub;
};

struct header_defs {
  char *name;
  struct cgen_target targets;
  struct struct_def defs[MAX_HEADER_DEFS];
  int ndefs;
};

int is_prefix (const char *str, const char *prefix);
int is_suffix (const char *str, const char *suffix);
int gather_struct_definitions(struct header_defs *hdefs, 
  char *fprefix, char *path);
int print_structs_defs(struct header_defs *hdef, 
  char *prefix, char *suffix, char *format);
