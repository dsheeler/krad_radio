#ifndef KRAD_RADIO_COMMON_H
#define KRAD_RADIO_COMMON_H

#include "krad_radio_version.h"
#include "krad_ebml.h"
#include "krad_ebmlx.h"
#include "krad_io.h"
#include "krad_radio_ipc.h"

typedef struct krad_radio_rep_St krad_radio_rep_t;
typedef struct krad_radio_rep_St kr_radio_t;
typedef struct kr_remote_St kr_remote_t;
typedef struct kr_tag_St kr_tag_t;

typedef enum {
  KR_LOG_ERROR = 1,
  KR_LOG_INFO = 2,
  KR_LOG_DEBUG = 3
} kr_log_level;

typedef struct {
  uint64_t time;
  uint64_t num;
  kr_log_level level;
  char address[96];
  char str[192];
} kr_log_msg;

struct krad_radio_rep_St {
  uint64_t uptime;
  uint32_t cpu_usage;
  uint32_t clients;
  char sysinfo[256];
  char logname[256];
};

struct kr_remote_St {
  uint16_t port;
  char interface[128];
};

struct kr_tag_St {
  char unit[256];
  char name[256];
  char value[256];
  char source[256];
};

#endif // KRAD_RADIO_COMMON_H
