#include "kr_client.h"
#include "krad_wayland.h"

#include "hex.h"

typedef struct wayrad_St wayrad_t;

struct wayrad_St {

	krad_wayland_t *krad_wayland;

	int width;
	int height;

	void *buffer;  

  char sysname[96];
  kr_client_t *client;

};

void wayrad_run (wayrad_t *wayrad);

void wayrad_destroy (wayrad_t *wayrad);
wayrad_t *wayrad_create (char *sysname);
