
int32_t interweb_ws_parse_frame_header(kr_iws_client_t *client) {

  interwebs_t *ws;
  uint8_t *size_bytes;
  uint8_t payload_sz_8;
  uint64_t payload_sz_64;
  uint16_t payload_sz_16;
  int32_t bytes_read;
  uint8_t frame_type;

  bytes_read = 0;

  ws = &client->ws;
  ws->input_len = client->in->len;
  ws->input = client->in->rd_buf;

  if (ws->input_len < 6) {
    return 0;
  }

  frame_type = ws->input[0];

  //printk("pframe type = %2X", frame_type);

  if (frame_type & WS_FIN_FRM) {
    //printk ("We have a fin frame!");
    frame_type ^= WS_FIN_FRM;
  }
  //printk("poframe type = %2X", frame_type);

  if (frame_type == WS_PING_FRM) {
    //printk ("We have a ping frame!");
  } else {
    if (frame_type == WS_CLOSE_FRM) {
      //printk ("We have a close frame!");
    } else {
      if (frame_type == WS_BIN_FRM) {
        //printk ("We have a bin frame!");
      } else {
        if (frame_type == WS_TEXT_FRM) {
          //printk ("We have a text frame!");
        } else {
          if (frame_type == WS_CONT_FRM) {
            //printk ("We have a CONT frame!");
          } else {
            printke ("Unknown frame type!");
            return -9;
          }
        }
      }
    }
  }

  payload_sz_8 = ws->input[1];

  if (payload_sz_8 & WS_MASK_BIT) {
    payload_sz_8 ^= WS_MASK_BIT;
  } else {
    printke("Mask Bit is NOT set");
    return -4;
  }

  if (payload_sz_8 < 126) {
    //printk("payload size is %u", payload_sz_8);
    ws->mask[0] = ws->input[2];
    ws->mask[1] = ws->input[3];
    ws->mask[2] = ws->input[4];
    ws->mask[3] = ws->input[5];
    ws->len = payload_sz_8;
    bytes_read = 6;
  } else {
    if (ws->input_len < 8) {
      return 0;
    }
    if (payload_sz_8 == 126) {
      size_bytes = (uint8_t *)&payload_sz_16;
      size_bytes[1] = ws->input[2];
      size_bytes[0] = ws->input[3];
      ws->mask[0] = ws->input[4];
      ws->mask[1] = ws->input[5];
      ws->mask[2] = ws->input[6];
      ws->mask[3] = ws->input[7];
      ws->len = payload_sz_16;
      bytes_read = 8;
    } else {
      if (ws->input_len < 14) {
        return 0;
      }
      size_bytes = (uint8_t *)&payload_sz_64;
      size_bytes[7] = ws->input[2];
      size_bytes[6] = ws->input[3];
      size_bytes[5] = ws->input[4];
      size_bytes[4] = ws->input[5];
      size_bytes[3] = ws->input[6];
      size_bytes[2] = ws->input[7];
      size_bytes[1] = ws->input[8];
      size_bytes[0] = ws->input[9];
      ws->mask[0] = ws->input[10];
      ws->mask[1] = ws->input[11];
      ws->mask[2] = ws->input[12];
      ws->mask[3] = ws->input[13];
      ws->len = payload_sz_64;
      bytes_read = 14;
    }
  }

  if (ws->len > 8192 * 6) {
    printke("input ws frame size too big");
    ws->len = 0;
    ws->pos = 0;
    return -10;
  }

  ws->pos = 0;
  ws->frames++;
  //printk("payload size is %"PRIu64"", ws->len);

  kr_io2_pulled (client->in, bytes_read);

  return bytes_read;
}

int32_t interweb_ws_parse_frame_data(kr_iws_client_t *client) {

  interwebs_t *ws;

  ws = &client->ws;
  ws->input_len = client->in->len;
  ws->input = client->in->rd_buf;

  if (ws->input_len < ws->len) {
    printk("Incomplete WS frame: %u / %"PRIu64"", ws->input_len,
     ws->len);
    return 0;
  }

  int32_t pos;
  int32_t max;
  uint8_t output[1024];

  ws->output = output;
  ws->output_len = sizeof(output);

  pos = 0;

  if ((ws->len == 0) || (ws->pos == ws->len) || (ws->input_len == 0) ||
      (ws->output_len == 0)) {
    return 0;
  }

  max = MIN(MIN((ws->len - ws->pos), ws->input_len), ws->output_len);

  //printk ("max is %d", max);

  for (pos = 0; pos < max; pos++) {
    ws->output[pos] = ws->input[ws->pos] ^ ws->mask[ws->pos % 4];
    ws->pos++;
  }

  output[pos] = '\0';
  //printk("unmasked %d bytes %s", pos, (char *)output);

  json_to_cmd (client, (char *)output);

  kr_io2_pulled (client->in, pos);

  if (ws->pos == ws->len) {
    ws->len = 0;
    ws->pos = 0;
  }

  return pos;
}

int32_t interweb_ws_pack_gen_accept_resp(char *resp, char *key) {

  static char *ws_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  int32_t ret;
  char string[128];
  uint8_t hash[20];

  if ((resp == NULL) || (key == NULL)) {
    return -1;
  }

  snprintf(string, sizeof(string), "%s%s", key, ws_guid);
  string[127] = '\0';

  kr_sha1((uint8_t *)string, strlen(string), hash);

  ret = kr_base64((uint8_t *)resp, hash, 20, 64);

  return ret;
}

uint32_t interweb_ws_pack_frame_header(uint8_t *out, uint32_t size) {

  uint16_t size_16;
  uint64_t size_64;
  uint8_t *size_bytes;

  out[0] = WS_FIN_FRM | WS_TEXT_FRM;
  if (size < 126) {
    out[1] = size;
    return 2;
  } else {
    if (size < 65536) {
      out[1] = 126;
      size_16 = size;
      size_bytes = (uint8_t *)&size_16;
      out[2] = size_bytes[1];
      out[3] = size_bytes[0];
      return 4;
    } else {
      out[1] = 127;
      size_64 = size;
      size_bytes = (uint8_t *)&size_64;
      out[2] = size_bytes[7];
      out[3] = size_bytes[6];
      out[4] = size_bytes[5];
      out[5] = size_bytes[4];
      out[6] = size_bytes[3];
      out[7] = size_bytes[2];
      out[8] = size_bytes[1];
      out[9] = size_bytes[0];
      return 10;
    }
  }
}

int32_t interweb_ws_kr_client_connect(kr_iws_client_t *client) {

  client->ws.krclient = kr_client_create ("websocket client");

  if (client->ws.krclient == NULL) {
    return -1;
  }

  if (!kr_connect (client->ws.krclient, client->server->sysname)) {
    kr_client_destroy (&client->ws.krclient);
    return -1;
  }

  kr_mixer_info (client->ws.krclient);
  kr_mixer_portgroup_list (client->ws.krclient);
  kr_compositor_subunit_list (client->ws.krclient);
  kr_subscribe_all (client->ws.krclient);

  return 0;
}

int32_t interweb_ws_hello(kr_iws_client_t *client) {

  cJSON *msg;

  //JSON first start
  client->ws.json = cJSON_CreateArray();

  cJSON_AddItemToArray (client->ws.json, msg = cJSON_CreateObject());
  cJSON_AddStringToObject (msg, "com", "kradradio");
  cJSON_AddStringToObject (msg, "info", "sysname");
  cJSON_AddStringToObject (msg, "infoval", client->server->sysname);

  interweb_json_pack (client);

  return 0;
}

int32_t interweb_ws_shake(kr_iws_client_t *client) {

  int32_t pos;
  char *buffer;
  char acceptkey[64];

  pos = 0;
  buffer = (char *)client->out->buf;
  memset(acceptkey, 0, sizeof(acceptkey));

  interweb_ws_pack_gen_accept_resp (acceptkey, client->ws.key);

  pos += sprintf (buffer + pos, "HTTP/1.1 101 Switching Protocols\r\n");
  pos += sprintf (buffer + pos, "Upgrade: websocket\r\n");
  pos += sprintf (buffer + pos, "Connection: Upgrade\r\n");
  pos += sprintf (buffer + pos, "Sec-WebSocket-Protocol: krad-ws-api\r\n");
  pos += sprintf (buffer + pos, "Sec-WebSocket-Accept: %s\r\n", acceptkey);
  pos += sprintf (buffer + pos, "\r\n");

  kr_io2_advance (client->out, pos);
  client->ws.shaked = 1;
  set_socket_nodelay(client->sd);
  interweb_ws_hello(client);
  interweb_ws_kr_client_connect(client);

  return 0;
}

int32_t krad_interweb_ws_client_handle(kr_iws_client_t *client) {

  int ret;

  if (!client->ws.shaked) {
    ret = interweb_ws_shake(client);
  } else {
    for (;;) {
      if (client->ws.len == 0) {
        ret = interweb_ws_parse_frame_header(client);
        if (ret < 0) {
          break;
        }
      }
      if (client->ws.len > 0) {
        ret = interweb_ws_parse_frame_data(client);
        if (ret <= 0) {
          break;
        }
      } else {
        ret = 0;
        break;
      }
    }
  }

  return ret;
}

