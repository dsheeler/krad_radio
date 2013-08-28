#include "kr_client.h"
#include "krad_wayland.h"

#include "hex.h"

typedef struct wayrad_St wayrad_t;
typedef struct cratezone_St cratezone_t;

struct cratezone_St {
  uint64_t crates;
  uint64_t crates_last;
};

struct wayrad_St {

  kr_wayland *wayland;
  kr_wayland_path *window;

  int width;
  int height;

  char sysname[96];
  kr_client_t *client;

  cratezone_t cratezone;
  pthread_t deliveries_thread;
  int done;

  float current_level;
  float master_peak;
};

void wayrad_run (wayrad_t *wayrad);

void wayrad_destroy (wayrad_t *wayrad);
wayrad_t *wayrad_create (char *sysname);
