#include "jsmn.h"
#include "cgen_utils.h"
#include "meta_gen.h"
#include "kr_client.h"
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
#include <errno.h>

typedef struct json_handler json_handler;

typedef enum cmd_type {
  CMD_DESTROY,
  CMD_CREATE,
  CMD_UPDATE
} cmd_type;

struct json_handler {
  kr_client_t *client;
  jsmntok_t *tokens;
  uint32_t ntokens;
  char *json;
};
