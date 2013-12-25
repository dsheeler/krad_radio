#include "krad_radio.h"
#include "krad_radio_client.h"
#include "krad_mixer_interface.h"
#include "krad_transponder_server.h"

#ifndef KRAD_RADIO_INTERFACE_H
#define KRAD_RADIO_INTERFACE_H

typedef struct kr_radio_client krad_radio_client_t;
typedef struct kr_radio_client kr_radio_client;

struct kr_radio_client {
  kr_radio *krad_radio;
  int valid;
};

typedef union {
  kr_mixer_path_info *portgroup;
  kr_remote_t *remote;
} krad_subunit_ptr_t;

int krad_radio_broadcast_subunit_created( krad_app_broadcaster_t *broadcaster, kr_address_t *address, void *subunit_in );
int krad_radio_broadcast_subunit_control(krad_app_broadcaster_t *broadcaster, kr_address_t *address_in, int control, float value, void *client);
int krad_radio_broadcast_subunit_update(krad_app_broadcaster_t *broadcaster, kr_address_t *address_in, int control, int type, void *value, void *client);
int krad_radio_broadcast_subunit_destroyed(krad_app_broadcaster_t *broadcaster, kr_address_t *address);

void krad_radio_pack_shipment_terminator(kr_ebml2_t *ebml);

void *krad_radio_client_create(void *ptr);
void krad_radio_client_destroy(void *ptr);
int krad_radio_client_handler(kr_io2_t *in, kr_io2_t *out, void *ptr);

#endif
