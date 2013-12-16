#ifndef COMMON_GEN_H
#define COMMON_GEN_H

typedef struct {
  int type;
  void *actual;
} uber_St;

typedef enum {
  TEXT_KR_V4L2_STATE = 1,
  TEXT_KR_V4L2_MODE,
  TEXT_KR_V4L2_INFO,
  TEXT_KR_V4L2_OPEN_INFO
} text_enum;

enum {
  TEXT_ENUM_LAST = 4
};

typedef enum {
  JSON_KR_V4L2_STATE = 1,
  JSON_KR_V4L2_MODE,
  JSON_KR_V4L2_INFO,
  JSON_KR_V4L2_OPEN_INFO
} json_enum;

enum {
  JSON_ENUM_LAST = 4
};

typedef enum {
  EBML_KR_V4L2_STATE = 1,
  EBML_KR_V4L2_MODE,
  EBML_KR_V4L2_INFO,
  EBML_KR_V4L2_OPEN_INFO
} ebml_enum;

enum {
  EBML_ENUM_LAST = 4
};

typedef enum {
  DEBML_KR_V4L2_STATE = 1,
  DEBML_KR_V4L2_MODE,
  DEBML_KR_V4L2_INFO,
  DEBML_KR_V4L2_OPEN_INFO
} debml_enum;

enum {
  DEBML_ENUM_LAST = 4
};

typedef enum {
  HELPERS_KR_V4L2_STATE = 1,
  HELPERS_KR_V4L2_MODE,
  HELPERS_KR_V4L2_INFO,
  HELPERS_KR_V4L2_OPEN_INFO
} helpers_enum;

enum {
  HELPERS_ENUM_LAST = 4
};


#endif

