#include "krad_transponder_interface.h"

/*static void kr_transponder_info_to_ebml(kr_ebml *ebml, kr_xpdr_info *info) {
  kr_ebml_pack_uint16(ebml, EBML_ID_KRAD_RADIO_TCP_PORT, info->active_paths);
}*/

static void transponder_info_ebml(kr_ebml *ebml, kr_transponder *xpdr) {
  kr_transponder_info info;
  memset(&info, 0, sizeof(kr_transponder_info));
  kr_transponder_get_info(xpdr, &info);
  kr_transponder_info_to_ebml(ebml, &info);
}

void test_cb_fun() {
  printk("this is test cb fun");
}

void test_function(kr_transponder *xpdr, kr_transponder_path_info *info) {

  kr_xpdr_path_setup setup;
  char text[2048];

  char *user = "this is my user pointer content";

  printk("the test function was called!");

  printk("path name %s",info->name);

  if (kr_transponder_path_info_to_text(text,info,2048) > 0) {
    printk("\n%s\n",text);
  }

  memcpy(&setup.info,info,sizeof(kr_transponder_path_info));
  setup.ev_cb = test_cb_fun;
  setup.user = xpdr;
  kr_transponder_mkpath(xpdr,&setup);

}

int kr_transponder_cmd(kr_io2_t *in, kr_io2_t *out, kr_radio_client *client) {

  kr_radio *radio;
  kr_transponder *transponder;
  kr_address address;
  kr_app_server *app;
  unsigned char *response;
  unsigned char *payload;
  kr_ebml2_t ebml_in;
  kr_ebml2_t ebml_out;
  uint32_t command;
  uint64_t size;
  int ret;
  radio = client->krad_radio;
  transponder = radio->transponder;
  app = radio->app;
  kr_transponder_path_info path_info;

  if (!(kr_io2_has_in(in))) {
    return 0;
  }

  kr_ebml2_set_buffer(&ebml_in, in->rd_buf, in->len);

  ret = kr_ebml2_unpack_id(&ebml_in, &command, &size);
  if ((ret < 0) || (command != EBML_ID_KRAD_TRANSPONDER_CMD)) {
    printke ("krad_mixer_command invalid EBML ID Not found");
    return 0;
  }

  ret = kr_ebml2_unpack_id(&ebml_in, &command, &size);
  if (ret < 0) {
    printke ("krad_mixer_command EBML ID Not found");
    return 0;
  }

  kr_ebml2_set_buffer(&ebml_out, out->buf, out->space);

  switch (command) {
    case EBML_ID_KRAD_TRANSPONDER_CMD_GET_INFO:
      address.path.unit = KR_TRANSPONDER;
      address.path.subunit.transponder_subunit = KR_UNIT;
      krad_radio_address_to_ebml2(&ebml_out, &response, &address);
      kr_ebml_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
       EBML_ID_KRAD_UNIT_INFO);
      kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
      transponder_info_ebml(&ebml_out, transponder);
      kr_ebml2_finish_element(&ebml_out, payload);
      kr_ebml2_finish_element(&ebml_out, response);
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_LIST: {
      break;
    }
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_CREATE: {
      kr_transponder_path_info_fr_ebml(&ebml_in,&path_info);
      test_function(transponder,&path_info);
      break;
    }


  /*
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_LIST:
      num = kr_xpdr_count (krad_transponder->xpdr);
      for (i = 0; i < KRAD_TRANSPONDER_MAX_SUBUNITS; i++) {
        if (!krad_transponder_subunit_to_rep ( krad_transponder, i, &transponder_subunit_rep )) {
          continue;
        }
        krad_transponder_subunit_address ( &transponder_subunit_rep, i, &address );
        krad_radio_address_to_ebml2 (&ebml_out, &response, &address);
        kr_ebml_pack_uint32 ( &ebml_out,
                               EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
                               EBML_ID_KRAD_SUBUNIT_INFO);
        kr_ebml2_start_element (&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD, &payload);
        krad_transponder_subunit_rep_to_ebml ( &ebml_out, &transponder_subunit_rep );
        kr_ebml2_finish_element (&ebml_out, payload);
        kr_ebml2_finish_element (&ebml_out, response);
      }
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_DESTROY:
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_UPDATE:
      break;
    case EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_CREATE:
*/
    case EBML_ID_KRAD_TRANSPONDER_CMD_LIST_ADAPTERS:
      address.path.unit = KR_TRANSPONDER;
      address.path.subunit.transponder_subunit = KR_ADAPTER;
      /*
#ifdef KR_LINUX
      devices = krad_v4l2_detect_devices();
      for (i = 0; i < devices; i++) {
        if (krad_v4l2_get_device_filename(i, string) > 0) {
          address.id.number = i;
          krad_radio_address_to_ebml2(&ebml_out, &response, &address);
          kr_ebml_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
           EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD,
           &payload);
          kr_ebml_pack_string(&ebml_out,
           EBML_ID_KRAD_TRANSPONDER_V4L2_DEVICE_FILENAME, string);
          kr_ebml2_finish_element(&ebml_out, payload);
          kr_ebml2_finish_element(&ebml_out, response);
        }
      }
#endif
      devices = krad_decklink_detect_devices();
      for (i = 0; i < devices; i++) {
        if (krad_decklink_get_device_name(i, string) > 0) {
          address.id.number = i;
          krad_radio_address_to_ebml2(&ebml_out, &response, &address);
          kr_ebml_pack_uint32(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
           EBML_ID_KRAD_SUBUNIT_INFO);
          kr_ebml2_start_element(&ebml_out, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD,
           &payload);
          kr_ebml_pack_string(&ebml_out,
           EBML_ID_KRAD_TRANSPONDER_DECKLINK_DEVICE_NAME, string);
          kr_ebml2_finish_element(&ebml_out, payload);
          kr_ebml2_finish_element(&ebml_out, response);
        }
      }
      */
      break;
    default:
      return -1;
  }

  if (((ebml_out.pos > 0) ||
       (command == EBML_ID_KRAD_TRANSPONDER_CMD_SUBUNIT_LIST)) &&
       (!krad_app_server_current_client_is_subscriber(app))) {
    krad_radio_pack_shipment_terminator(&ebml_out);
  }
  kr_io2_pulled(in, ebml_in.pos);
  kr_io2_advance(out, ebml_out.pos);
  return 0;
}
