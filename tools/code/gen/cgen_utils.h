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
#include <errno.h>
#define MAX_BUFFER 65536
#define MAX_DYNAMIC_MEMBERS 128
#define MAX_DYNAMIC_STRUCTS 64

typedef struct dynamic_struct dynamic_struct;
typedef struct dynamic_member dynamic_member;
typedef struct code_gen_options code_gen_opts;
typedef struct cgen_limits cgen_limits;

typedef enum {
  KR_DY_INT32,
  KR_DY_UINT32,
  KR_DY_UINT64,
  KR_DY_FLOAT,
  KR_DY_CHAR,
  KR_DY_STRING,
  KR_DY_STRUCT,
  KR_DY_ENUM
} member_type;
 
struct dynamic_member {
  char name[64];
  member_type type;
  char struct_name[64];
  uint8_t array;
  uint32_t array_length;
};
 
struct dynamic_struct {
  char name[64];
  uint32_t members;
  uint8_t sub;
  dynamic_member dynamic_members[MAX_DYNAMIC_MEMBERS];
};

struct cgen_limits {
  uint32_t memb_max;
  uint32_t memb_name_max;
  uint32_t struct_name_max;
};

struct code_gen_options {
  dynamic_struct *dystructs;
  uint32_t nstructs;
  int32_t (*codegen_fun) (code_gen_opts *);
  char *output_buffer;
  uint32_t buff_pos;
  uint32_t buff_size;
  cgen_limits limits;
};

int32_t *clean_string (char *input, char *output);
uint32_t is_array(char *fname);
int32_t parse_and_init(char *basepath,dynamic_struct *dystructs);
dynamic_struct *get_struct_by_name(char *name, 
  dynamic_struct *dystructs, uint32_t n);

