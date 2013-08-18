#ifndef KRAD_JACK_H
#define KRAD_JACK_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <limits.h>

#include <jack/jack.h>

#include "krad_system.h"
#include "krad_jack_common.h"

#define KR_JACK_CHANNELS_MAX 8

typedef struct kr_jack kr_jack;
typedef struct kr_jack_setup kr_jack_setup;
typedef struct kr_jack_path_setup kr_jack_path_setup;
typedef struct kr_jack_path kr_jack_path;
typedef struct kr_jack_path kr_jack_input;
typedef struct kr_jack_path kr_jack_output;

struct kr_jack_setup {
  char client_name[64];
  char server_name[64];
  void *user;
  //state callback - server shutdown/xrun/period-sampleratechange
  void *cb;
};

struct kr_jack_path_setup {
  kr_jack_path_info info;
  //samples callback?
  //state callback - connection/disconnection
  //callpack pointer
};

/*
void kr_jack_portgroup_plug(kr_jack_portgroup *portgroup, char *remote_name);
void kr_jack_portgroup_unplug(kr_jack_portgroup *portgroup, char *remote_name);
*/

int kr_jack_detect();
int kr_jack_detect_server_name(char *name);

int kr_jack_path_unlink(kr_jack_path *path);
kr_jack_path *kr_jack_mkpath(kr_jack *jack, kr_jack_path_setup *setup);

int kr_jack_destroy(kr_jack *jack);
kr_jack *kr_jack_create(kr_jack_setup *setup);

#endif
