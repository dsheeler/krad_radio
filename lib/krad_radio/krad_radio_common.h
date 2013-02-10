#ifndef KRAD_RADIO_COMMON_H
#define KRAD_RADIO_COMMON_H

#include "krad_ebml.h"
#include "krad_radio_ipc.h"

typedef struct krad_radio_rep_St krad_radio_rep_t;
typedef struct krad_radio_rep_St kr_radio_t;
typedef struct kr_remote_St kr_remote_t;
typedef struct kr_tag_St kr_tag_t;

struct krad_radio_rep_St {
  

  uint64_t uptime;
  uint32_t cpu_usage;
  char sysinfo[256];
  char logname[256];
  
};

struct kr_remote_St {
  int port;
  char interface[128];
};

struct kr_tag_St {
  char unit[256];
  char name[256];
  char value[256];
  char source[256];
};



kr_radio_t *kr_radio_rep_create ();
void kr_radio_rep_destroy (kr_radio_t *radio_rep);

kr_remote_t *kr_remote_rep_create_with (char *interface, int port);
kr_remote_t *kr_remote_rep_create ();
void kr_remote_rep_destroy (kr_remote_t *remote_rep);






#endif // KRAD_RADIO_COMMON_H
