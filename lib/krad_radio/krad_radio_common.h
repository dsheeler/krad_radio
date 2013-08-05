#ifndef KRAD_RADIO_COMMON_H
#define KRAD_RADIO_COMMON_H

#include "krad_radio_version.h"
#include "krad_ebml2.h"
#include "krad_ebml2w.h"
#include "krad_io2.h"
#include "krad_radio_ipc.h"

typedef struct krad_radio_rep_St krad_radio_rep_t;
typedef struct krad_radio_rep_St kr_radio_t;
typedef struct kr_remote_St kr_remote_t;
typedef struct kr_tag_St kr_tag_t;

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
