#include "krad_radio.h"
#include "krad_radio_client.h"

#ifndef KRAD_RADIO_INTERFACE_H
#define KRAD_RADIO_INTERFACE_H

typedef union {
  krad_mixer_portgroup_t *portgroup;
  kr_remote_t *remote;
} krad_subunit_ptr_t;

int krad_radio_broadcast_subunit_created ( krad_app_broadcaster_t *broadcaster, kr_address_t *address, void *subunit_in );
int krad_radio_broadcast_subunit_control (krad_app_broadcaster_t *broadcaster, kr_address_t *address_in, int control, float value, void *client);
int krad_radio_broadcast_subunit_update (krad_app_broadcaster_t *broadcaster, kr_address_t *address_in, int control, int type, void *value, void *client);
int krad_radio_broadcast_subunit_destroyed (krad_app_broadcaster_t *broadcaster, kr_address_t *address);


typedef struct krad_radio_client_St krad_radio_client_t;

struct krad_radio_client_St {
  krad_radio_t *krad_radio;
  int valid;
};

void krad_radio_pack_shipment_terminator (kr_ebml2_t *ebml);

void *krad_radio_client_create (void *ptr);
void krad_radio_client_destroy (void *ptr);
int krad_radio_client_handler (kr_io2_t *in, kr_io2_t *out, void *ptr);

#endif
