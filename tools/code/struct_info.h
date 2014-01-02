#include "stdio.h"
#include "inttypes.h"
#include "string.h"

#define FILENAME_MAX_LEN 64
#define NAME_MAX_LEN 48
#define MEMBER_MAX 48
#define DEF_MAX_LEN 4096

typedef struct struct_info struct_info;
typedef struct struct_data struct_data;
typedef struct member_info member_info;

typedef struct char_member_info char_member_info;
typedef struct int32_member_info int32_member_info;
typedef struct uint32_member_info uint32_member_info;
typedef struct int64_member_info int64_member_info;
typedef struct uint64_member_info uint64_member_info;
typedef struct float_member_info float_member_info;
typedef struct double_member_info double_member_info;
typedef struct struct_member_info struct_member_info;

struct char_member_info {
  char init[64];
  int notnull;
};

struct int32_member_info {
  int32_t init;
  int32_t min;
  int32_t max;
};

struct int64_member_info {
  int64_t init;
  int64_t min;
  int64_t max;
};

struct uint64_member_info {
  uint64_t init;
  uint64_t min;
  uint64_t max;
};

struct uint32_member_info {
  uint32_t init;
  uint32_t min;
  uint32_t max;
};

struct float_member_info {
  float init;
  float min;
  float max;
};

struct double_member_info {
  double init;
  double min;
  double max;
};

struct struct_member_info {
  char type_name[NAME_MAX_LEN];
};

typedef enum {
  T_CHAR,
  T_INT32,
  T_INT64,
  T_UINT32,
  T_UINT64,
  T_FLOAT,
  T_DOUBLE,
  T_STRUCT
} member_type;

typedef enum {
  ST_ENUM,
  ST_STRUCT,
  ST_UNION
} struct_type;

typedef union {
  char_member_info char_info;
  int32_member_info int32_info;
  uint32_member_info uint32_info;
  int64_member_info int64_info;
  uint64_member_info uint64_info;
  float_member_info float_info;
  double_member_info double_info;
  struct_member_info substruct_info;
} member_type_info;

struct member_info {
  char name[NAME_MAX_LEN];
  int arr;
  char len_def[32];
  int ptr;
  member_type type;
  member_type_info type_info;
};

struct struct_info {
  struct_type type;
  char name[NAME_MAX_LEN];
  int member_count;
  member_info members[MEMBER_MAX];
  int is_typedef;
};

struct struct_data {
  char filename[FILENAME_MAX_LEN];
  int line_number;
  char definition[DEF_MAX_LEN];
  struct_info info;
};