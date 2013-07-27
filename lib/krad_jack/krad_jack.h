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
#include <endian.h>

#include <jack/jack.h>

#include "krad_transponder_common.h"
#include "krad_mixer_common.h"

#define KR_JACK 343

typedef struct kr_jack kr_jack;
typedef struct kr_jack_portgroup kr_jack_portgroup;

struct kr_jack_portgroup {
  kr_jack *kr_jack;
  char name[256];
  jack_port_t *ports[8];
  float *samples[8];
  int direction;
  int channels;
};

struct kr_jack {
  int active;
  int set_thread_name_process;
  int xruns;
  char server_name[256];
  const char *name;
  jack_options_t options;
  jack_status_t status;
  jack_client_t *client;
  uint32_t sample_rate;
  uint32_t period_size;
  char *stay_connected[256];
  char *stay_connected_to[256];
};

int kr_jack_detect();
int kr_jack_detect_for_jack_server_name(char *server_name);
void kr_jack_portgroup_samples_callback(int frames, void *user, float **smpls);
void kr_jack_portgroup_plug(kr_jack_portgroup *portgroup, char *remote_name);
void kr_jack_portgroup_unplug(kr_jack_portgroup *portgroup, char *remote_name);
void kr_jack_portgroup_destroy(kr_jack_portgroup *portgroup);
kr_jack_portgroup *kr_jack_portgroup_create(kr_jack *jack, char *name,
 int direction, int channels);

void kr_jack_destroy(kr_jack *jack);
kr_jack *kr_jack_create();
kr_jack *kr_jack_create_for_jack_server_name(char *server_name);

int kr_jack_process(jack_nframes_t nframes, void *arg);
void kr_jack_shutdown(void *arg);
int kr_jack_xrun(void *arg);

#endif
