#include "krad_radio_client.h"
#include "krad_radio_client_internal.h"
#include "krad_transponder_common.h"

static int kr_transponder_crate_get_string_from_adapter (kr_crate_t *crate, char **string, int maxlen);

int kr_transponder_receiver_enable (kr_client_t *client, int port) {

  unsigned char *linker_command;
  unsigned char *enable_linker;
  uint16_t port_actual;

  if (!(kr_sys_port_valid (port))) {
    return -1;
  } else {
    port_actual = port;
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_LISTEN_ENABLE, &enable_linker);  

  kr_ebml2_pack_uint16 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port_actual);

  kr_ebml2_finish_element (client->ebml2, enable_linker);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
  
  return 1;
}

void kr_transponder_receiver_disable (kr_client_t *client) {

  unsigned char *linker_command;
  unsigned char *disable_linker;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_LISTEN_DISABLE, &disable_linker);
  kr_ebml2_finish_element (client->ebml2, disable_linker);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

int kr_transponder_transmitter_enable (kr_client_t *client, int port) {

  unsigned char *linker_command;
  unsigned char *enable_transmitter;
  uint16_t port_actual;

  if (!(kr_sys_port_valid (port))) {
    return -1;
  } else {
    port_actual = port;
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_TRANSMITTER_ENABLE, &enable_transmitter);  

  kr_ebml2_pack_uint16 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port_actual);

  kr_ebml2_finish_element (client->ebml2, enable_transmitter);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);

  return 1;
}

void kr_transponder_transmitter_disable (kr_client_t *client) {

  unsigned char *linker_command;
  unsigned char *disable_transmitter;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_TRANSMITTER_DISABLE, &disable_transmitter);
  kr_ebml2_finish_element (client->ebml2, disable_transmitter);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_info (kr_client_t *client) {

  unsigned char *command;
  unsigned char *info_command;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_GET_INFO, &info_command);
  kr_ebml2_finish_element (client->ebml2, info_command);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_transponder_adapters (kr_client_t *client) {

  unsigned char *linker_command;
  unsigned char *adapters;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_LIST_ADAPTERS, &adapters);

  kr_ebml2_finish_element (client->ebml2, adapters);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_subunit_list (kr_client_t *client) {

  unsigned char *linker_command;
  unsigned char *list_links;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_LIST, &list_links);
  kr_ebml2_finish_element (client->ebml2, list_links);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_subunit_destroy (kr_client_t *client, uint32_t number) {

  unsigned char *linker_command;
  unsigned char *destroy_link;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_DESTROY, &destroy_link);

  kr_ebml2_pack_uint32 (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER, number);

  kr_ebml2_finish_element (client->ebml2, destroy_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);

  kr_client_push (client);
}

void kr_transponder_subunit_update (kr_client_t *client, uint32_t number, uint32_t ebml_id, int newval) {

  unsigned char *linker_command;
  unsigned char *update_link;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_UPDATE, &update_link);
/*
  kr_ebml2_pack_int8 (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER, number);
  kr_ebml2_pack_int32 (client->ebml2, ebml_id, newval);
*/
  kr_ebml2_finish_element (client->ebml2, update_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_subunit_update_str (kr_client_t *client, uint32_t number, uint32_t ebml_id, char *newval) {

  unsigned char *linker_command;
  unsigned char *update_link;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_UPDATE, &update_link);
/*
  kr_ebml2_pack_int8 (client->ebml2, EBML_ID_KRAD_TRANSPONDER_LINK_NUMBER, number);
  kr_ebml2_pack_string (client->ebml2, ebml_id, newval);
*/
  kr_ebml2_finish_element (client->ebml2, update_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

void kr_transponder_subunit_create (kr_client_t *client, char *mode, char *option) {

  unsigned char *linker_command;
  unsigned char *create_link;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD, &linker_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_CREATE, &create_link);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_OPERATION_MODE, mode);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_LINK_LINK_FILENAME, option);

  kr_ebml2_finish_element (client->ebml2, create_link);
  kr_ebml2_finish_element (client->ebml2, linker_command);
    
  kr_client_push (client);
}

static int kr_transponder_crate_get_string_from_adapter (kr_crate_t *crate, char **string, int maxlen) {

  int len;

  len = 0;

  len += sprintf (*string + len, "Adapter: ");
  kr_ebml2_unpack_element_string (&crate->payload_ebml, NULL, *string + len, maxlen);

  return len; 
}

static void kr_ebml_to_transponder_rep (kr_ebml2_t *ebml, kr_transponder_t *transponder_rep) {
  kr_ebml2_unpack_element_uint16 (ebml, NULL, &transponder_rep->receiver_port);
  kr_ebml2_unpack_element_uint16 (ebml, NULL, &transponder_rep->transmitter_port);
}

static int kr_transponder_crate_get_string_from_transponder (kr_crate_t *crate, char **string, int maxlen) {

  kr_transponder_t transponder;
  int len;

  len = 0;

  kr_ebml_to_transponder_rep (&crate->payload_ebml, &transponder);

  len += sprintf (*string + len, "Transponder Status:\n");
  if (transponder.receiver_port == 0) {
    len += sprintf (*string + len, "Receiver Off\n");
  } else {
    len += sprintf (*string + len, "Receiver On: Port %u\n", transponder.receiver_port);
  }

  if (transponder.transmitter_port == 0) {
    len += sprintf (*string + len, "Transmitter Off");
  } else {
    len += sprintf (*string + len, "Transmitter On: Port %u", transponder.transmitter_port);
  }
  
  return len; 
}

int kr_transponder_crate_to_string (kr_crate_t *crate, char **string) {

  if (crate->notice == EBML_ID_KRAD_UNIT_INFO) {
    *string = kr_response_alloc_string (crate->size * 16);
    return kr_transponder_crate_get_string_from_transponder (crate, string, crate->size * 16);
  }

  switch ( crate->address.path.subunit.transponder_subunit ) {
    case KR_ADAPTER:
      *string = kr_response_alloc_string (crate->size * 8);
      return kr_transponder_crate_get_string_from_adapter (crate, string, crate->size * 8);
    case KR_TRANSMITTER:
      break;
    case KR_RECEIVER:
      break;
    case KR_DEMUXER:
      break;  
    case KR_MUXER:
      break;
    case KR_ENCODER:
      break;
    case KR_DECODER:
      break;
  }
  
  return 0;  
}

