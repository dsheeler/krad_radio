/**
 * @file krad_transponder_client.h
 * @brief Krad Radio Transponder Controller API
 */

/**
 * @mainpage Krad Radio Transponder Controller
 *
 * Krad Radio Transponder Controller (Kripton this is where you might want to hold back beacause it will change a bit)
 *
 */
 
 
/** @defgroup krad_transponder_client Krad Radio Transponder Control
  @{
  */

#ifndef KRAD_TRANSPONDER_CLIENT_H
#define KRAD_TRANSPONDER_CLIENT_H

#include "krad_transponder_common.h"

void kr_transponder_response_print (kr_response_t *kr_response);
int kr_transponder_crate_to_string (kr_crate_t *crate, char **string);

void kr_transponder_info (kr_client_t *client);
void kr_transponder_adapters (kr_client_t *client);

/** Transponder **/

int kr_transponder_transmitter_enable (kr_client_t *client, int port);
void kr_transponder_transmitter_disable (kr_client_t *client);
int kr_transponder_receiver_enable (kr_client_t *client, int port);
void kr_transponder_receiver_disable (kr_client_t *client);

void kr_transponder_subunit_list (kr_client_t *client);
void kr_transponder_subunit_destroy (kr_client_t *client, uint32_t number);
void kr_transponder_subunit_update (kr_client_t *client, uint32_t number, uint32_t ebml_id, int newval);
void kr_transponder_subunit_update_str (kr_client_t *client, uint32_t number, uint32_t ebml_id, char *newval);

void kr_transponder_subunit_create (kr_client_t *client, char *mode, char *option);


/**@}*/

#endif
