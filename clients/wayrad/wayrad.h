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

	krad_wayland_t *krad_wayland;

	int width;
	int height;

	void *buffer;  

  char sysname[96];
  kr_client_t *client;

  cratezone_t cratezone;
  pthread_t deliveries_thread;
  int done;
};

void wayrad_run (wayrad_t *wayrad);

void wayrad_destroy (wayrad_t *wayrad);
wayrad_t *wayrad_create (char *sysname);
