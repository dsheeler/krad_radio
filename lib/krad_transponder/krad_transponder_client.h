#ifndef KRAD_TRANSPONDER_CLIENT_H
#define KRAD_TRANSPONDER_CLIENT_H

#include "krad_transponder_common.h"

void kr_transponder_response_print(kr_response_t *kr_response);
int kr_transponder_crate_to_string(kr_crate_t *crate, char **string);

void kr_transponder_info_get(kr_client_t *client);
void kr_transponder_adapters(kr_client_t *client);

int kr_transponder_transmitter_enable(kr_client *client, int port);
void kr_transponder_transmitter_disable(kr_client *client);
int kr_transponder_receiver_enable(kr_client *client, int port);
void kr_transponder_receiver_disable(kr_client *client);

void kr_transponder_subunit_list(kr_client *client);
void kr_transponder_subunit_destroy(kr_client *client, uint32_t number);
void kr_transponder_subunit_update(kr_client *client, uint32_t number, uint32_t ebml_id, int newval);
void kr_transponder_subunit_update_str(kr_client *client, uint32_t number, uint32_t ebml_id, char *newval);

void kr_transponder_subunit_create(kr_client *client, char *mode, char *option);

int kr_xpdr_mkpath(kr_client *client, kr_transponder_path_info *info);

#endif
