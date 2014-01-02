#ifndef KRAD_COMPOSITOR_SERVER_H
#define KRAD_COMPOSITOR_SERVER_H

#include "krad_compositor.h"
#include "krad_radio_server.h"

int kr_compositor_cmd(kr_io2_t *in, kr_io2_t *out, kr_radio_client *client);

#endif
