#include "krad_radio_interface.h"
#include "krad_radio_internal.h"

static void krad_radio_remote_rep_to_ebml (kr_remote_t *remote, kr_ebml2_t *ebml);

static void krad_radio_remote_rep_to_ebml (kr_remote_t *remote, kr_ebml2_t *ebml) {
  kr_ebml2_pack_string (ebml, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, remote->interface);
  kr_ebml2_pack_uint16 (ebml, EBML_ID_KRAD_RADIO_REMOTE_PORT, remote->port);
}

int krad_radio_broadcast_subunit_destroyed (krad_app_broadcaster_t *broadcaster, kr_address_t *address) {

  unsigned char buffer[256];
  krad_broadcast_msg_t *broadcast_msg;
  kr_ebml2_t ebml;
  unsigned char *message_loc;

  kr_ebml2_set_buffer ( &ebml, buffer, 256);

  krad_radio_address_to_ebml2 (&ebml, &message_loc, address);
  kr_ebml2_pack_int32 (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_RADIO_UNIT_DESTROYED);
  kr_ebml2_finish_element (&ebml, message_loc);

  broadcast_msg = krad_broadcast_msg_create (broadcaster, buffer, ebml.pos);

  if (broadcast_msg != NULL) {
    return krad_app_server_broadcaster_broadcast ( broadcaster, &broadcast_msg );
  }

  return -1;
}

int krad_radio_broadcast_subunit_created ( krad_app_broadcaster_t *broadcaster, kr_address_t *address, void *subunit_in ) {

  unsigned char buffer[4096];
  krad_broadcast_msg_t *broadcast_msg;
  kr_ebml2_t ebml;
  unsigned char *message_loc;
  unsigned char *payload_loc;
  krad_subunit_ptr_t subunit;
  kr_rep_actual_t rep;

  kr_ebml2_set_buffer ( &ebml, buffer, 4096 );

  krad_radio_address_to_ebml2 (&ebml, &message_loc, address);
  kr_ebml2_pack_int32 (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_CREATED);
  kr_ebml2_start_element (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload_loc);

  if ((address->path.unit == KR_MIXER) && (address->path.subunit.mixer_subunit == KR_PORTGROUP)) {
    subunit.portgroup = subunit_in;
    krad_mixer_portgroup_to_rep (subunit.portgroup, &rep.portgroup);
    krad_mixer_portgroup_rep_to_ebml2 (&rep.portgroup, &ebml);
  }

  if (address->path.unit == KR_COMPOSITOR) {
    if (address->path.subunit.compositor_subunit == KR_SPRITE) {
      krad_compositor_sprite_to_ebml2 ( &ebml, subunit_in );
    }
    if (address->path.subunit.compositor_subunit == KR_TEXT) {
      krad_compositor_text_to_ebml2 ( &ebml, subunit_in );
    }
    if (address->path.subunit.compositor_subunit == KR_VECTOR) {
      krad_compositor_vector_to_ebml2 ( &ebml, subunit_in );
    }
  }

  if ((address->path.unit == KR_STATION) && (address->path.subunit.station_subunit == KR_REMOTE)) {
    subunit.remote = subunit_in;
    krad_radio_remote_rep_to_ebml (subunit.remote, &ebml);
  }

  kr_ebml2_finish_element (&ebml, payload_loc);
  kr_ebml2_finish_element (&ebml, message_loc);

  broadcast_msg = krad_broadcast_msg_create (broadcaster, buffer, ebml.pos);

  if (broadcast_msg != NULL) {
    return krad_app_server_broadcaster_broadcast ( broadcaster, &broadcast_msg );
  }

  return -1;
}

int krad_radio_broadcast_subunit_control (krad_app_broadcaster_t *broadcaster, kr_address_t *address_in, int control, float value, void *client) {
  return krad_radio_broadcast_subunit_update (broadcaster, address_in, control, KR_FLOAT, &value, client);
}

int krad_radio_broadcast_subunit_update (krad_app_broadcaster_t *broadcaster, kr_address_t *address_in, int control, int type, void *value, void *client) {

  unsigned char buffer[1024];
  krad_broadcast_msg_t *broadcast_msg;
  kr_ebml2_t ebml;
  unsigned char *message_loc;
  kr_address_t address;
  unsigned char *payload_loc;
  float *value_float;
  int *value_int;

  kr_ebml2_set_buffer ( &ebml, buffer, 1024);

  address.path.unit = address_in->path.unit;
  if (address.path.unit == KR_MIXER) {
    address.path.subunit.mixer_subunit = address_in->path.subunit.mixer_subunit;
    strncpy (address.id.name, address_in->id.name, sizeof (address.id.name));
    address.control.portgroup_control = control;
  } else {
    if (address.path.unit == KR_COMPOSITOR) {
      address.path.subunit.compositor_subunit = address_in->path.subunit.compositor_subunit;
      address.control.compositor_control = control;
    } else {
      if (address.path.unit == KR_TRANSPONDER) {
        address.path.subunit.transponder_subunit = address_in->path.subunit.transponder_subunit;
      }
    }
    address.id.number = address_in->id.number;
  }

  address.sub_id = address_in->sub_id;
  address.sub_id2 = address_in->sub_id2;

  krad_radio_address_to_ebml2 (&ebml, &message_loc, &address);
  kr_ebml2_pack_int32 (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_TYPE, EBML_ID_KRAD_SUBUNIT_CONTROL);
  kr_ebml2_start_element (&ebml, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload_loc);
  switch (type) {
    case KR_FLOAT:
      value_float = (float *)value;
      kr_ebml2_pack_float (&ebml, EBML_ID_KRAD_SUBUNIT_CONTROL, *value_float);
      break;
    case KR_STRING:
      kr_ebml2_pack_string (&ebml, EBML_ID_KRAD_SUBUNIT_CONTROL, value);
      break;
    case KR_INT32:
      value_int = (int *)value;
      kr_ebml2_pack_int32 (&ebml, EBML_ID_KRAD_SUBUNIT_CONTROL, *value_int);
      break;
  }

  kr_ebml2_finish_element (&ebml, payload_loc);
  kr_ebml2_finish_element (&ebml, message_loc);

  broadcast_msg = krad_broadcast_msg_create (broadcaster, buffer, ebml.pos);
  broadcast_msg->skip_client = client;

  if (broadcast_msg->skip_client != NULL) {
    //printk ("want to Goint to skip a client!!\n");
  }

  if (broadcast_msg != NULL) {
    return krad_app_server_broadcaster_broadcast ( broadcaster, &broadcast_msg );
  }

  return -1;
}

void krad_radio_pack_shipment_terminator (kr_ebml2_t *ebml) {

  kr_address_t address;
  unsigned char *response;

  memset (&address, 0, sizeof (kr_address_t));

  address.path.unit = KR_STATION;
  address.path.subunit.station_subunit = KR_STATION_UNIT;

  krad_radio_address_to_ebml2 (ebml, &response, &address);
  kr_ebml2_pack_uint32 ( ebml,
                         EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                         EBML_ID_KRAD_SHIPMENT_TERMINATOR);
  kr_ebml2_finish_element (ebml, response);
}

void *krad_radio_client_create(void *ptr) {

  krad_radio_t *station;
  kr_radio_client *client;

  station = (krad_radio_t *)ptr;
  client = calloc (1, sizeof(kr_radio_client));
  client->krad_radio = station;

  return client;
}

void krad_radio_client_destroy (void *ptr) {
  krad_radio_client_t *krad_radio_client = (krad_radio_client_t *)ptr;
  free (krad_radio_client);
}

int validate_header ( kr_io2_t *in ) {

  kr_ebml2_t ebml;
  int ret;
  char doctype[32];
  uint32_t version;
  uint32_t read_version;

  kr_ebml2_set_buffer ( &ebml, in->rd_buf, in->len );

  ret = kr_ebml2_unpack_header (&ebml, doctype, 32, &version, &read_version);

  if (ret > 0) {
    if ((version == KRAD_APP_DOCTYPE_VERSION) && (read_version == KRAD_APP_DOCTYPE_READ_VERSION) &&
        (strlen(KRAD_APP_CLIENT_DOCTYPE) == strlen(doctype)) &&
        (strncmp(doctype, KRAD_APP_CLIENT_DOCTYPE, strlen(KRAD_APP_CLIENT_DOCTYPE)) == 0)) {
        kr_io2_pulled (in, ret);
        return ret;
    } else {
      printke ("validate header mismatch %u %u %s \n", version, read_version, doctype);
    }
  } else {
    printke ("validate header err %d\n", ret);
  }

  return -1;
}

void pack_client_header (kr_io2_t *out) {

  kr_ebml2_t ebml;

  kr_ebml2_set_buffer ( &ebml, out->buf, out->space );

  kr_ebml2_pack_header (&ebml,
                        KRAD_APP_SERVER_DOCTYPE,
                        KRAD_APP_DOCTYPE_VERSION,
                        KRAD_APP_DOCTYPE_READ_VERSION);

  kr_io2_advance (out, ebml.pos);
}

int validate_client ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client ) {

  int ret;

  ret = validate_header (in);
  if (ret > 0) {
    client->valid = 1;
    pack_client_header (out);
    return 1;
  } else {
    printke ("Invalid client!");
    return -1;
  }
}

uint32_t full_command (kr_io2_t *in) {

  kr_ebml2_t ebml;
  uint32_t element;
  uint64_t size;
  int ret;

  if (!(kr_io2_has_in (in))) {
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml, in->rd_buf, in->len );

  ret = kr_ebml2_unpack_id (&ebml, &element, &size);

  if (ret < 0) {
    printke ("full_command EBML ID Not found");
    return 0;
  }

  size += ebml.pos;

  if (in->len < size) {
    printke ("full_command Not Enough bytes..");
    return 0;
  }

  return element;
}

void krad_radio_to_ebml ( kr_ebml2_t *ebml, krad_radio_t *krad_radio ) {
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_RADIO_SYSTEM_INFO, krad_system_info());
  kr_ebml2_pack_string(ebml, EBML_ID_KRAD_RADIO_LOGNAME, krad_radio->log.filename);
  kr_ebml2_pack_uint64(ebml, EBML_ID_KRAD_RADIO_UPTIME, krad_system_daemon_uptime());
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_RADIO_UPTIME, krad_app_server_num_clients(krad_radio->app));
  kr_ebml2_pack_uint32(ebml, EBML_ID_KRAD_RADIO_SYSTEM_CPU_USAGE, krad_system_get_cpu_usage());
}

int krad_radio_client_command ( kr_io2_t *in, kr_io2_t *out, krad_radio_client_t *client ) {

  krad_radio_t *krad_radio;
  kr_address_t address;
  unsigned char *response;
  unsigned char *payload;
  kr_ebml2_t ebml_in;
  kr_ebml2_t ebml_out;
  uint32_t command;
  uint32_t element;
  uint64_t size;
  int ret;
  unsigned char *tag;
  char *tag_name;
  char *tag_val;
  char string[512];
  char string1[512];
  char string2[512];
  char string3[512];
  uint32_t numbers[10];
  kr_remote_t remote;
  krad_app_server_t *app;
  int i;
  krad_tags_t *krad_tags;

  krad_tags = NULL;
  i = 0;
  string1[0] = '\0';
  string2[0] = '\0';
  string3[0] = '\0';

  krad_radio = client->krad_radio;
  app = krad_radio->app;

  if (!(kr_io2_has_in (in))) {
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_in, in->rd_buf, in->len );

  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if ((ret < 0) || (command != EBML_ID_KRAD_RADIO_CMD)) {
    printke ("krad_radio_client_command invalid EBML ID Not found");
    return 0;
  }

  ret = kr_ebml2_unpack_id (&ebml_in, &command, &size);
  if (ret < 0) {
    printke ("krad_radio_client_command EBML ID Not found");
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml_out, out->buf, out->space );

  switch ( command ) {
    case EBML_ID_KRAD_RADIO_CMD_GET_SYSTEM_INFO:
      address.path.unit = KR_STATION;
      address.path.subunit.station_subunit = KR_STATION_UNIT;

      krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
      kr_ebml2_pack_uint32 ( &ebml_out,
                             EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                             EBML_ID_KRAD_UNIT_INFO);

      kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      krad_radio_to_ebml (&ebml_out, krad_radio);
      kr_ebml2_finish_element (&ebml_out, payload);
      kr_ebml2_finish_element (&ebml_out, response);
      break;
    case EBML_ID_KRAD_RADIO_CMD_LIST_TAGS:
      kr_ebml2_unpack_element_string (&ebml_in, &element, string1, sizeof(string1));
      if (strncmp("station", string1, 7) == 0) {
        krad_tags = krad_radio->krad_tags;
      } else {
        krad_tags = krad_radio_find_tags_for_item ( krad_radio, string1 );
      }
      if (krad_tags != NULL) {
        i = 0;
        address.path.unit = KR_STATION;
        address.path.subunit.station_subunit = KR_TAGS;
        krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
        kr_ebml2_pack_uint32 ( &ebml_out,
                               EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                               EBML_ID_KRAD_SUBUNIT_INFO);

        kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        while (krad_tags_get_next_tag ( krad_tags, &i, &tag_name, &tag_val)) {
          kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_TAG, &tag);
          kr_ebml2_pack_string ( &ebml_out, EBML_ID_KRAD_RADIO_TAG_ITEM, string1);
          kr_ebml2_pack_string ( &ebml_out, EBML_ID_KRAD_RADIO_TAG_NAME, tag_name);
          kr_ebml2_pack_string ( &ebml_out, EBML_ID_KRAD_RADIO_TAG_VALUE, tag_val);
          kr_ebml2_finish_element (&ebml_out, tag);
        }
        kr_ebml2_finish_element (&ebml_out, payload);
        kr_ebml2_finish_element (&ebml_out, response);
      } else {
        printke ("Could not find %s", string1);
      }
      break;
    case EBML_ID_KRAD_RADIO_CMD_SET_TAG:
      ret = kr_ebml2_unpack_id (&ebml_in, &element, &size);
      kr_ebml2_unpack_element_string (&ebml_in, &element, string1, sizeof(string1));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string2, sizeof(string2));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string3, sizeof(string3));
      if (strncmp("station", string1, 7) == 0) {
        krad_tags_set_tag ( krad_radio->krad_tags, string2, string3);
      } else {
        krad_tags = krad_radio_find_tags_for_item ( krad_radio, string1 );
        if (krad_tags != NULL) {
          krad_tags_set_tag ( krad_tags, string2, string3);
          printk ("Set Tag %s on %s to %s", string2, string1, string3);
        } else {
          printke ("Could not find %s", string1);
        }
      }
      //krad_app_server_broadcast_tag ( app, tag_item, tag_name, tag_value);
      break;
    case EBML_ID_KRAD_RADIO_CMD_GET_TAG:
      ret = kr_ebml2_unpack_id (&ebml_in, &element, &size);
      kr_ebml2_unpack_element_string (&ebml_in, &element, string1, sizeof(string1));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string2, sizeof(string2));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string3, sizeof(string3));
      if (strncmp("station", string1, 7) == 0) {
        tag_val = krad_tags_get_tag (krad_radio->krad_tags, string2);
      } else {
        tag_val = "";
        krad_tags = krad_radio_find_tags_for_item ( krad_radio, string1 );
        if (krad_tags != NULL) {
          tag_val = krad_tags_get_tag ( krad_tags, string2 );
          printk ("Got Tag %s on %s - %s", string2, string1, tag_val);
        } else {
          printke ("Could not find %s", string1);
        }
      }
      if (strlen(tag_val)) {
        address.path.unit = KR_STATION;
        address.path.subunit.station_subunit = KR_TAGS;
        krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
        kr_ebml2_pack_uint32 ( &ebml_out,
                               EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                               EBML_ID_KRAD_SUBUNIT_INFO);
        kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_TAG, &tag);
        kr_ebml2_pack_string ( &ebml_out, EBML_ID_KRAD_RADIO_TAG_ITEM, string1);
        kr_ebml2_pack_string ( &ebml_out, EBML_ID_KRAD_RADIO_TAG_NAME, string2);
        kr_ebml2_pack_string ( &ebml_out, EBML_ID_KRAD_RADIO_TAG_VALUE, tag_val);
        kr_ebml2_finish_element (&ebml_out, tag);
        kr_ebml2_finish_element (&ebml_out, payload);
        kr_ebml2_finish_element (&ebml_out, response);
      }
      break;
    case EBML_ID_KRAD_RADIO_CMD_GET_REMOTE_STATUS:
      printk ("command remote status");
      address.path.unit = KR_STATION;
      address.path.subunit.station_subunit = KR_REMOTE;
      for (i = 0; i < MAX_REMOTES; i++) {
        if (app->tcp_port[i]) {
          address.id.number = i;
          krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
          kr_ebml2_pack_uint32 ( &ebml_out,
                                 EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                                 EBML_ID_KRAD_UNIT_INFO);
          kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
          kr_ebml2_pack_string ( &ebml_out, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, app->tcp_interface[i]);
          kr_ebml2_pack_uint16 ( &ebml_out, EBML_ID_KRAD_RADIO_SYSTEM_CPU_USAGE, app->tcp_port[i]);
          kr_ebml2_finish_element (&ebml_out, payload);
          kr_ebml2_finish_element (&ebml_out, response);
        }
      }
      break;
    case EBML_ID_KRAD_RADIO_CMD_REMOTE_ENABLE:
      printk ("command remote enable");
      kr_ebml2_unpack_element_string (&ebml_in, &element, remote.interface, sizeof(string1));
      kr_ebml2_unpack_element_uint16 (&ebml_in, &element, &remote.port);
      if (krad_app_server_enable_remote (app, remote.interface, remote.port)) {
        //FIXME this is wrong in the case of adapter based matches with multiple ips
        // perhaps switch to callback based create broadcast?
        address.path.unit = KR_STATION;
        address.path.subunit.station_subunit = KR_REMOTE;
        for (i = 0; i < MAX_REMOTES; i++) {
          if (app->tcp_port[i] == remote.port) {
            address.id.number = i;
            break;
          }
        }
        krad_radio_broadcast_subunit_created ( app->app_broadcaster, &address, (void *)&remote);
      }
      break;
    case EBML_ID_KRAD_RADIO_CMD_REMOTE_DISABLE:
      printk ("command remote disable");
      kr_ebml2_unpack_element_string (&ebml_in, &element, string1, sizeof(string1));
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      while (1) {
        i = krad_app_server_disable_remote (app, string1, numbers[0]);
        if (i < 0) {
          break;
        }
        address.path.unit = KR_STATION;
        address.path.subunit.station_subunit = KR_REMOTE;
        address.id.number = i;
        krad_radio_broadcast_subunit_destroyed (app->app_broadcaster, &address);
      }
      break;
    case EBML_ID_KRAD_RADIO_CMD_OSC_ENABLE:
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      krad_osc_listen (krad_radio->remote.osc, numbers[0]);
      break;
    case EBML_ID_KRAD_RADIO_CMD_OSC_DISABLE:
      krad_osc_stop_listening (krad_radio->remote.osc);
      break;
    case EBML_ID_KRAD_RADIO_CMD_WEB_ENABLE:
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      kr_ebml2_unpack_element_string (&ebml_in, &element, string1, sizeof(string1));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string2, sizeof(string2));
      kr_ebml2_unpack_element_string (&ebml_in, &element, string3, sizeof(string3));

      if (krad_radio->remote.interweb != NULL) {
        krad_interweb_server_destroy (&krad_radio->remote.interweb);
      }

      krad_radio->remote.interweb = krad_interweb_server_create (krad_radio->sysname, numbers[0],
                                                                 string1, string2, string3);
      break;
    case EBML_ID_KRAD_RADIO_CMD_WEB_DISABLE:
      if (krad_radio->remote.interweb != NULL) {
        krad_interweb_server_destroy (&krad_radio->remote.interweb);
      }
      break;
    case EBML_ID_KRAD_RADIO_CMD_SET_DIR:
      kr_ebml2_unpack_element_string (&ebml_in, &element, string, sizeof(string1));
      if (strlen(string)) {
        krad_radio_set_dir ( krad_radio, string );
      }
      break;
    case EBML_ID_KRAD_RADIO_CMD_BROADCAST_SUBSCRIBE:
      kr_ebml2_unpack_element_uint32 (&ebml_in, &element, &numbers[0]);
      krad_app_server_add_client_to_broadcast ( app, numbers[0] );
      break;
    default:
      printke ("Krad Radio Command Unknown! %u", command);
      return -1;
  }

  if ((ebml_out.pos > 0) && (!krad_app_server_current_client_is_subscriber (app))) {
    krad_radio_pack_shipment_terminator (&ebml_out);
  }

  kr_io2_pulled (in, ebml_in.pos);
  kr_io2_advance (out, ebml_out.pos);

  return 0;
}

int krad_radio_client_handler ( kr_io2_t *in, kr_io2_t *out, void *ptr ) {

  krad_radio_client_t *client;
  int ret;
  uint32_t command;

  if (in == NULL) {
    printke("krad_radio_client handler called with null input buffer");
    return -1;
  }

  if (out == NULL) {
    printke("krad_radio_client handler called with null output buffer");
    return -1;
  }

  if (ptr == NULL) {
    printke("krad_radio_client handler called with null client pointerr");
    return -1;
  }

  client = (krad_radio_client_t *)ptr;

  if (!client->valid) {
    ret = validate_client (in, out, client);
    if (ret != 1) {
      return -1;
    }
  }

  while (1) {
    command = full_command (in);
    if (command == 0) {
      return 0;
    }

    //printk ("we have a full command la de da its %zu bytes", in->len);

    switch (command) {
      case EBML_ID_KRAD_MIXER_CMD:
        ret = krad_mixer_command (in, out, client);
        break;
      case EBML_ID_KRAD_COMPOSITOR_CMD:
        ret = krad_compositor_command (in, out, client);
        break;
      case EBML_ID_KRAD_TRANSPONDER_CMD:
        ret = krad_transponder_command (in, out, client);
        break;
      case EBML_ID_KRAD_RADIO_CMD:
        ret = krad_radio_client_command (in, out, client);
        break;
      default:
        ret = -1;
        break;
    }
    if (ret != 0) {
      return -1;
    }
  }

  return 0;
}
